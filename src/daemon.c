/*
 * Copyright (c) boulz.org team
 *
 * See the LICENCE file for redistribution information.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>

#include "cfg.h"
#include "daemon.h"
#include "log.h"

/*
 * sig_handler()
 * handle signals.
 */

void
sig_handler(int SIG)
{
  switch(SIG)
  {
    case SIGHUP:
      log_file(SYS_FILE, "[SIG] SIGHUP received.");
      break;
    case SIGSEGV:
      log_file(SYS_FILE, "[SIG] SIGSEGV received! Ouch!");
      remove(PID_FILE);
      exit(1);
    case SIGSTOP:
      log_file(SYS_FILE, "[SIG] SIGSTOP received.");
      break;
    case SIGTERM:
      log_file(SYS_FILE, "[SIG] SIGTERM received! Bye!");
      remove(PID_FILE);
      exit(0);
    break;
  }
}

/*
 * daemon_run()
 * fork program into background.
 */

void
daemon_run(void)
{
  int pid, pid_f;
  char buf[8];

  /* change directory */
  if(chdir(daemon_dir) != 0)
  {
    mkdir(daemon_dir, 655);
    chdir(daemon_dir);
  }

  /* fork starts from here */
  pid = fork();
  if (pid < 0)
  {
    log_file(SYS_FILE, "[DAEMON] Error daemonizing (fork)! %d - %s", errno, strerror(errno));
    exit(1);
  }
  if (pid > 0)
    exit(0);

  /* create a new process group session
     fail -> exit */
#ifdef __linux__
  setpgrp();
#else
  setpgrp(0, getpgid(getppid()));
#endif
  if (setsid() < 0)
  {
    log_file(SYS_FILE, "[DAEMON] Error daemonizing (setsid)! %d - %s", errno, strerror(errno));
    exit(0);
  }

  /* set umask */
  umask(022);

  /* 2 kinds of behaviour:
   * 1st - user launch the program as root
   * -> the setuid is done with the user dedicated for the bot
   * 2nd - user launch the program on his session
   * -> program herits of his rights */
  if (getuid() == 0)
  {
    if (daemon_user)
    {
      struct passwd *pwent;
      if ((pwent = getpwnam(daemon_user)) == NULL)
      {
        fprintf(stderr, "ERROR!\n"
                        "You have to create an user for the bot\n"
                        "OR launch the bot on your session.\n");
        exit(1);
      }
      else
      {
        if(initgroups(daemon_user, pwent->pw_gid) != 0)
          log_file(SYS_FILE, "[DAEMON] Could't init groups!: %d", errno);
        if(setuid(pwent->pw_uid) != 0)
          log_file(SYS_FILE, "[DAEMON] Couldn't set UID!: %d", errno);
        if(setgid(pwent->pw_gid) != 0)
          log_file(SYS_FILE, "[DAEMON] Couldn't set GID!: %d", errno);
      }
    }
  }

  /* if file doesn't exist, create it
   * else read it */
  pid_f = open(PID_FILE, O_RDWR|O_CREAT, 0644);

  /* can't open the file */
  if (pid_f < 0)
    exit(1);

  /* can't lock it*/
  if (lockf(pid_f, F_TLOCK, 0) < 0)
    exit(0);

  /* write into it the pid of the daemon */
  sprintf(buf, "%d\n", getpid());
  write(pid_f, buf, strlen(buf));

  /* ignore all theses signals */
  signal(SIGCHLD, SIG_IGN);
  signal(SIGTSTP, SIG_IGN);
  signal(SIGTTOU, SIG_IGN);
  signal(SIGTTIN, SIG_IGN);

  /* catch theses to the sig_handler function */
  signal(SIGSTOP, sig_handler);
  signal(SIGHUP, sig_handler);
  signal(SIGTERM, sig_handler);
  signal(SIGSEGV, sig_handler);

  /* close every std descriptors
   * that could be openened */
  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);

  /* and open them properly */
  open("/dev/null", O_RDWR);
  dup2(0, 1);
  dup2(0, 2);

  /* finally, we send a message to the log file, saying the process is starting */
  log_file(SYS_FILE, "[START] I'm starting !");
}
