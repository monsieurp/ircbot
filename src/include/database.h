/*
 * Copyright (c) boulz.org team
 *
 * See the LICENCE file for redistribution information.
 */

#include <sqlite3.h>

#define SQL_COUNT_RESPONSE "SELECT COUNT(*) FROM responses;"
#define SQL_COUNT_QUOTE "SELECT COUNT(*) FROM quotes;"
#define SQL_RAND_RESPONSE "SELECT message FROM responses ORDER BY RANDOM() LIMIT 1;"
#define SQL_RAND_QUOTE "SELECT message FROM quotes ORDER BY RANDOM() LIMIT 1;"

enum tables {
  table_quotes,
  table_responses
};

short db_query(char *db_req, char **db_res);
short db_insert(enum tables table, char *value);
