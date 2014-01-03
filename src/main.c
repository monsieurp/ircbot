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
#include "events.h"
#include "log.h"
#include "daemon.h"

/*
 * main()
 * The main function.
 */

int
main(int argc, char *argv[])
{
  irc_callbacks_t i_callbacks;
  irc_session_t *i_session;

  daemon_run();

  memset(&i_callbacks, 0, sizeof(i_callbacks));
  i_callbacks.event_join = event_join;
  i_callbacks.event_connect = event_connect;
  i_callbacks.event_channel = event_channel;
  i_callbacks.event_ctcp_rep = event_ctcp_rep;

  reconnect = 1;
  while(reconnect)
  {
    cfg_get();
    i_session = irc_create_session(&i_callbacks);

    if(!i_session)
    {
      log_file(SYS_FILE, "[IRC] Couldn't create session!");
      return 1;
    }

    if(irc_connect(i_session, irc_server, irc_port, 0, bot_nickname, bot_realname, bot_name))
    {
      log_file(SYS_FILE, "[IRC] Coudln't connect: %s", irc_strerror(irc_errno(i_session)));
      return 1;
    }

    irc_option_set(i_session, LIBIRC_OPTION_STRIPNICKS);
    irc_run(i_session); /* Main loop */

    irc_destroy_session(i_session);
    cfg_free();
  }

  remove(PID_FILE);
  return 0;
}

