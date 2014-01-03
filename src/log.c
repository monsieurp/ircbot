/*
 * Copyright (c) boulz.org team
 *
 * See the LICENCE file for redistribution information.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <libircclient.h>

#include "cfg.h"

/*
 * log_file()
 * write in a file everthing passed as a parameter in the function.
 */

void
log_file(char * filename, const char * msg, ...)
{
  FILE * file;
  char buf[1024], datetime[255];
  time_t current = time(NULL);
  struct tm *today = localtime(&current);
  va_list va_alist;

  va_start (va_alist, msg);
  vsnprintf (buf, sizeof(buf), msg, va_alist);
  va_end (va_alist);
  strftime(datetime, 255, "[%d/%m/%y @ %H:%M:%S]", today);

  if ((file = fopen(filename, "ab")) != 0)
  {
    fprintf (file, "%s %s\n", datetime, buf);
    fclose (file);
  }
}

/*
 * log_events()
 * log all IRC events.
 */

void log_events(irc_session_t * session, const char * event, const char * origin, const char ** params, unsigned int count)
{
  int cnt;
  char buf[512];

  buf[0] = '\0';

  for (cnt = 0; cnt < count; cnt++)
  {
    if (cnt)
      strcat (buf, "] ");

    strcat (buf, params[cnt]);
  } 

  log_file(IRC_FILE, "%s from %s on [%s", event, origin ? origin : "NULL", buf);
}
