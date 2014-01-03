/*
 * Copyright (c) boulz.org team
 *
 * See the LICENCE file for redistribution information.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <libircclient.h>

#include "cfg.h"
#include "cmd.h"
#include "database.h"
#include "events.h"
#include "log.h"

/* Map the command name to the function */
#define NB_CMDS sizeof(cmd_table) / sizeof(cmd_table[0])
struct {
  const char *name;
  void (*fun)(irc_session_t *, const char *, char *);
} cmd_table[] = {
  { "!addquote", cmd_addquote },
  { "!db_stat", cmd_db_stat },
  { "!help", cmd_help },
  { "!quit", cmd_quit },
  { "!quote", cmd_quote },
  { "!restart", cmd_restart },
};


/*
 * Check who's comin' on the channel.
 * session = the actual opened IRC session, which generates events
 * event   = the type of event (JOIN, PART, PRIVMSG, CTCP, etc) as text form
 * origin  = the nickname/server who has triggered the event
 * params  = [0] the channel
 *           [1] what is saying on this channel
 * count   = the total number of params supplied.
 */

void
event_join(irc_session_t *session, const char *event, const char *origin, const char **params, unsigned int count)
{
  /* This function is used to log everything in the irclog file. */
  log_events(session, event, origin, params, count);
  /* Someone is joining the channel */
  if(strcmp(origin, bot_nickname) != 0)
  {
    /* I send him CTCP VERSION */
    irc_cmd_ctcp_request(session, origin, "VERSION");
  }
  /* I'm joinin the channel for the very 1st time */
  else
  {
    char *auth_msg;
    /* I set myself mode +i (invisible) */
    irc_cmd_user_mode (session, "+i");
    /* I authenticate myself against NickServ */
    asprintf(&auth_msg, "IDENTIFY %s", auth_pwd);
    irc_cmd_msg(session, "NickServ", auth_msg);
    free(auth_msg);
    /* and I say something */
    irc_cmd_me(session, irc_channel, join_msg);
  }
}

/*
 * Perform commands when connected to a IRC server.
 */

void
event_connect(irc_session_t *session, const char *event, const char *origin, const char **params, unsigned int count)
{
  log_events(session, event, origin, params, count);
  /* I'm joining a channel */
  irc_cmd_join(session, irc_channel, 0);
}

/*
 * Perform commands after a CTCP response.
 */

void
event_ctcp_rep(irc_session_t * session, const char * event, const char * origin, const char ** params, unsigned int count)
{
  char *msg;
  log_events(session, event, origin, params, count);
  if(strcmp(bot_nickname, origin) != 0)
  {
    asprintf(&msg, "%s CTCP %s", origin, params[0]);
    /* I send on the channel the CTCP response I received */
    irc_cmd_msg(session, irc_channel, msg);
    free(msg);

    /* If the joiner is running an IRC client under m$ windows, we blame him */
    if(strcasestr(params[0], "windows") || strcasestr(params[0], "mirc"))
    {
      asprintf(&msg, "%s: %s", origin, "tu es *SALE*");
      irc_cmd_msg(session, irc_channel, msg);
      free(msg);
    }
  }
}

/*
 * Parse messages on the channel
 */

void
event_channel(irc_session_t *session, const char *event, const char *origin, const char **params, unsigned int count)
{
  char *pmsg, *bc, *db_res, *br;
  int i = 0;

  /* We check the existance of events on the channel
  * and if this event is spawned by someone (origin = nick) */
  if(count != 2 || !origin)
    return;

  log_events(session, event, origin, params, count);

  /* Kick when a forbidden word is detected */
  for(i = 0; forbidden_words[i] != NULL; i++) {
    if(strcasestr(params[1], forbidden_words[i]) != NULL) {
      db_query(SQL_RAND_RESPONSE, &db_res);
      irc_cmd_kick(session, origin, irc_channel, db_res);
      free(db_res);
      return;
    }
  }

  /* Reply when someone is talking to the bot */
  asprintf(&bc, "%s: ", bot_nickname);
  pmsg = strstr(params[1], bc);
  free(bc);
  if(pmsg != NULL && (strlen(params[1]) == strlen(pmsg)) && strlen(pmsg) > strlen(bot_nickname) + 2) {
    /* Extract the message from the string */
    pmsg += strlen(bot_nickname) + 2;
    /* Insert this message in the db */
    if(db_insert(table_responses, pmsg) != 0) {
      log_file(SYS_FILE, "[IRC] Can't insert the message in the db");
    }
    /* Reply with a message from the database */
    if (db_query(SQL_RAND_RESPONSE, &db_res) == 0) {
      asprintf(&br, "%s: %s", origin, db_res);
      irc_cmd_msg(session, irc_channel, br);
      free(br);
      free(db_res);
    }
    return;
  }

  /* Commands */
  int clen = 0, plen = 0;
  for(i = 0; i < NB_CMDS; i++) {
    pmsg = strstr(params[1], cmd_table[i].name);
    if(pmsg != NULL && (strlen(params[1]) == (plen = strlen(pmsg)))) {
      clen = strlen(cmd_table[i].name);
      if(plen > clen) {
        pmsg += clen + 1;
      } else {
        pmsg = NULL;
      }
      cmd_table[i].fun(session, origin, pmsg);
    }
  }

  /* Laught when "lol" is detected */
  if (strcasestr(params[1], "lol") && (strlen(params[1]) == 3))
  {
    asprintf(&br, "%s: OHl0L", origin);
    irc_cmd_msg(session, irc_channel, br);
    free(br);
    return;
  }
}
