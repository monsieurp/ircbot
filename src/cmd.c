/*
 * Copyright (c) boulz.org team
 *
 * See the LICENCE file for redistribution information.
 */

#include <string.h>

#include "cfg.h"
#include "cmd.h"
#include "database.h"
#include "log.h"

int
nick_is_allowed(const char *nick)
{
  int i;
  for(i = 0; allowed_nicks[i] != NULL; i++) {
    if(strcmp(allowed_nicks[i], nick) == 0) {
      return 0;
    }
  }
  return 1;
}

void
cmd_addquote(irc_session_t *session, const char *origin, char *param)
{
  if(nick_is_allowed(origin) == 0) {
    if(db_insert(table_quotes, param) == 0) {
      log_file(SYS_FILE, "[IRC] Quote insert successful!");
      irc_cmd_notice(session, origin, "Quote insert successful!");
    } else {
      log_file(SYS_FILE, "[IRC] Quote insert failed!");
      irc_cmd_notice(session, origin, "Quote insert failed!");
    }
  }
}

void
cmd_db_stat(irc_session_t *session, const char *origin, char *param)
{
  char *count_response, *count_quote, *db_stat;
  db_query(SQL_COUNT_RESPONSE, &count_response);
  db_query(SQL_COUNT_QUOTE, &count_quote);

  asprintf(&db_stat, "#responses=%s #quotes=%s", count_response, count_quote);
  irc_cmd_msg(session, irc_channel, db_stat);

  free(count_quote);
  free(count_response);
  free(db_stat);
}

void
cmd_help(irc_session_t *session, const char *origin, char *param)
{
  irc_cmd_msg(session, irc_channel, "!addquote <THE QUOTE> -- insert a quote in the database");
  irc_cmd_msg(session, irc_channel, "!db_stat -- print database statistics");
  irc_cmd_msg(session, irc_channel, "!quote -- print a quote, randomly selected from the database ");
  irc_cmd_msg(session, irc_channel, "!quit -- terminate the ircbot process");
  irc_cmd_msg(session, irc_channel, "!restart -- disconnect from the server, then reconnect with fresh config options");
}

void
cmd_quit(irc_session_t *session, const char *origin, char *param)
{
  if(nick_is_allowed(origin) == 0) {
    log_file(SYS_FILE, "[QUIT] Quitting due to %s !quit command.", origin);
    irc_cmd_quit(session, "Quitting...");
    reconnect = 0;
  }
}

void
cmd_quote(irc_session_t *session, const char *origin, char *param)
{
  char *quote;
  db_query(SQL_RAND_QUOTE, &quote);
  irc_cmd_msg(session, irc_channel, quote);
  free(quote);
}

void
cmd_restart(irc_session_t *session, const char *origin, char *param)
{
  if(nick_is_allowed(origin) == 0) {
    log_file(SYS_FILE, "[START] Restarting due to %s !restart command", origin);
    irc_cmd_quit(session, "Restarting...");
  }
}
