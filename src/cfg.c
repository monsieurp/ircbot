/*
 * Copyright (c) boulz.org team
 *
 * See the LICENCE file for redistribution information.
 */

#include <stdlib.h>
#include <string.h>
#include <libconfig.h>

#include "cfg.h"
#include "log.h"

/*
 * Get the configs values. Store them in the globals defined in config.h
 * Return 0 on success.
 */
int
cfg_get()
{
  config_init(&cfg);

  /* Read the .cfg file and report error */
  if (config_read_file(&cfg, CFG_FILE) == CONFIG_FALSE)
  {
    fprintf(stderr, "Couldn't read %s at line %i : %s \n", CFG_FILE, cfg.error_line, cfg.error_text);
    exit(1);
  }

  /* Get the configs values */
  config_lookup_string(&cfg, "daemon_user", &daemon_user);
  config_lookup_string(&cfg, "daemon_dir", &daemon_dir);
  config_lookup_string(&cfg, "irc_server", &irc_server);
  config_lookup_int(&cfg, "irc_port", &irc_port);
  config_lookup_string(&cfg, "irc_channel", &irc_channel);
  config_lookup_string(&cfg, "bot_nickname", &bot_nickname);
  config_lookup_string(&cfg, "bot_realname", &bot_realname);
  config_lookup_string(&cfg, "bot_name", &bot_name);
  config_lookup_string(&cfg, "auth_pwd", &auth_pwd);
  config_lookup_string(&cfg, "join_msg", &join_msg);

  /* Get the allowed nicks */
  allowed_nicks = get_config_array(&cfg, "allowed_nicks");
  /* Get the forbidden words */
  forbidden_words = get_config_array(&cfg, "forbidden_words");

  return 0;
}

const char**
get_config_array(struct config_t *cfg, const char *key)
{
  const char **array;
  const config_setting_t *setting;
  int i, nb;

  setting = config_lookup(cfg, key);
  nb = config_setting_length(setting);
  array = malloc(sizeof(char *) * (nb + 1)); /* +1 for the NULL */
  for(i = 0; i < nb; i++)
  {
    array[i] = config_setting_get_string_elem(setting, i);
  }
  array[nb] = NULL; /* NULL terminate the array for easier looping */
  return array;
}

void
cfg_free()
{
  config_destroy(&cfg);
  free(allowed_nicks);
  free(forbidden_words);
}
