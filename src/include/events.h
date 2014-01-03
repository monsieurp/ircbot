/*
 * Copyright (c) boulz.org team
 *
 * See the LICENCE file for redistribution information.
 */

#include <libircclient.h>

/* Prototypes for events functions */
void event_join(irc_session_t *session, const char *event, const char *origin, const char **params, unsigned int count);
void event_connect(irc_session_t *session, const char *event, const char *origin, const char **params, unsigned int count);
void event_channel(irc_session_t *session, const char *event, const char *origin, const char **params, unsigned int count);
void event_ctcp_rep(irc_session_t *session, const char *event, const char *origin, const char **params, unsigned int count);
