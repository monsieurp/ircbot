/*
 * Copyright (c) boulz.org team
 *
 * See the LICENCE file for redistribution information.
 */

#include <libconfig.h>

/* CONSTANTS */
#define SYS_FILE "bot_syslog.log"
#define IRC_FILE "bot_irclog.log"
#define PID_FILE "bot.run"
#define CFG_FILE "ircbot.cfg"

/* GLOBALS */
short reconnect;
struct config_t cfg;
const char *daemon_user;
const char *daemon_dir;
const char *irc_server;
long irc_port;
const char *bot_nickname;
const char *bot_name;
const char *bot_realname;
const char *irc_channel;
const char *join_msg;
const char *auth_pwd;
const char **allowed_nicks;
const char **forbidden_words;

/* PROTOTYPES */
int cfg_get();
void cfg_free();
const char** get_config_array(struct config_t *cfg, const char *key);
