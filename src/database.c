/*
 * Copyright (c) boulz.org team
 *
 * See the LICENCE file for redistribution information.
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sqlite3.h>
#include <err.h>

#include "cfg.h"
#include "database.h"
#include "events.h"
#include "log.h"

/*
 * db_callback()
 * Callback function to retrieve the database query from db_query().
 * As described in sqlite3 documentation, after a sqlite3_exec(), the callback is retrieved in the 3rd argument of this function.
 * Always returns 0.
 * db_data contain the value passed as the 4rd argument of sqlite3_exec().
 * db_colnb contain the number of the column.
 * db_value contain the value of the column.
 * db_colname contain the name of the column.
 *
 * Example: Here, my db looks like this
 * BEGIN TRANSACTION;
 * CREATE TABLE quotes (id INTEGER PRIMARY KEY, message TEXT, time DATE);
 * END;
 * If I query the db with a "SELECT * FROM quotes;", output would be:
 *
 *   db_colname[0]           db_colname[1]                  db_colname[2]
 *       id                     message                         time
 *        |                        |                              |
 *    db_value[0]             db_value[1]                    db_value[2]
 *        1              This is the first value         2008-11-23 23:07:52
 *        |                        |                              |
 *    db_colnb[0]             db_colnb[1]                    db_colnb[2]
 *        1                        2                              3
 *
 */

static int
db_callback(void *ctx, int db_colnb, char **db_values, char **db_colnames)
{
  char **res = (char **)ctx;
  *res = strdup(db_values[0]);
  return 0;
}

/*
 * db_open()
 * Open a database.
 * Returns 0 if the database is open without errors.
 * Returns 1 in the other case.
 */

short
db_open(sqlite3 **db)
{
  if((sqlite3_open("irc.db", db)) != SQLITE_OK)
  {
    log_file(SYS_FILE, "[SQLite3] Open failed: %s\n", sqlite3_errmsg(*db));
    sqlite3_close(*db);
    return 1;
  }
  return 0;
}

/*
 * db_insert()
 * Insert values from the 2nd argument in the message column of the 1st arg (database)
 * Returns 0 if the insert has been done.
 * Returns 1 in the other case.
 */

short
db_insert(enum tables table, char *value)
{
  short db_errno = 0;
  sqlite3 *db;
  sqlite3_stmt *db_stmt;

  /* Open the db first */
  if(db_open(&db) != 0)
    return 1;

  /* Prepares the statement according to the table */
  switch(table)
  {
    case table_quotes:
      sqlite3_prepare(db, "INSERT INTO quotes (message) VALUES(?)", -1, &db_stmt, 0);
      break;
    case table_responses:
      sqlite3_prepare(db, "INSERT INTO responses (message) VALUES(?)", -1, &db_stmt, 0);
      break;
    default:
      log_file(SYS_FILE, "db_insert(): unknown table");
      return 1;
  }

  if(db_stmt == NULL)
  {
    log_file(SYS_FILE, "db_insert(): sqlite3_prepare() failed\n");
    return 1;
  }

  /* Bind the value to the statement */
  sqlite3_bind_text(db_stmt, 1, value, -1, SQLITE_STATIC);

  if(sqlite3_step(db_stmt) != SQLITE_DONE)
  {
    log_file(SYS_FILE, "[SQLite3] Insert failed: %s\n", sqlite3_errmsg(db));
    db_errno = 1;
  }

  sqlite3_finalize(db_stmt);
  sqlite3_close(db);
  return db_errno;
}

/*
 * db_query()
 * Used to query the database with a SQL request passed as the 1st parameter.
 * Store the result in the second paramter (malloc'ed: to be free'ed by the caller).
 * Return 0 if the query has been done.
 * Return 1 in the other case.
 */

short
db_query(char *db_req, char **db_res)
{
  short db_errno = 0;
  char *err_msg;
  sqlite3 *db;

  if(db_open(&db) != 0)
    return 1;

  if(sqlite3_exec(db, db_req, db_callback, db_res, &err_msg) != SQLITE_OK)
  {
    log_file(SYS_FILE, "[SQLite3] Query failed: %s\n", err_msg);
    *db_res = strdup(err_msg);
    sqlite3_free(err_msg);
    db_errno = 1;
  }

  sqlite3_close(db);
  return db_errno;
}
