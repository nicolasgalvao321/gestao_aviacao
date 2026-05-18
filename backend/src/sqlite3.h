/*
** SQLite3 Header - Minimal version for embedded use
*/

#ifndef SQLITE3_H
#define SQLITE3_H

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sqlite3 sqlite3;
typedef struct sqlite3_stmt sqlite3_stmt;

#define SQLITE_OK           0
#define SQLITE_ERROR        1
#define SQLITE_INTERNAL     2
#define SQLITE_PERM         3
#define SQLITE_ABORT        4
#define SQLITE_BUSY         5
#define SQLITE_LOCKED       6
#define SQLITE_NOMEM        7
#define SQLITE_READONLY     8
#define SQLITE_INTERRUPT    9
#define SQLITE_IOERR       10
#define SQLITE_CORRUPT     11
#define SQLITE_NOTFOUND    12
#define SQLITE_FULL        13
#define SQLITE_CANTOPEN    14
#define SQLITE_PROTOCOL    15
#define SQLITE_EMPTY       16
#define SQLITE_SCHEMA      17
#define SQLITE_TOOBIG      18
#define SQLITE_CONSTRAINT  19
#define SQLITE_MISMATCH    20
#define SQLITE_MISUSE      21
#define SQLITE_NOLFS       22
#define SQLITE_AUTH        23
#define SQLITE_FORMAT      24
#define SQLITE_RANGE       25
#define SQLITE_NOTADB      26
#define SQLITE_NOTICE      27
#define SQLITE_WARNING     28
#define SQLITE_ROW        100
#define SQLITE_DONE       101

int sqlite3_open(const char *filename, sqlite3 **ppDb);
int sqlite3_close(sqlite3 *db);
int sqlite3_exec(sqlite3 *db, const char *sql, int (*callback)(void*,int,char**,char**), void *arg, char **errmsg);
int sqlite3_prepare_v2(sqlite3 *db, const char *zSql, int nByte, sqlite3_stmt **ppStmt, const char **pzTail);
int sqlite3_step(sqlite3_stmt *pStmt);
int sqlite3_finalize(sqlite3_stmt *pStmt);
int sqlite3_bind_int(sqlite3_stmt *pStmt, int index, int value);
int sqlite3_bind_double(sqlite3_stmt *pStmt, int index, double value);
int sqlite3_bind_text(sqlite3_stmt *pStmt, int index, const char *value, int n, void (*destructor)(void*));
int sqlite3_column_int(sqlite3_stmt *pStmt, int iCol);
double sqlite3_column_double(sqlite3_stmt *pStmt, int iCol);
const unsigned char *sqlite3_column_text(sqlite3_stmt *pStmt, int iCol);
void sqlite3_free(char *p);
const char *sqlite3_errmsg(sqlite3 *db);

#define SQLITE_STATIC ((void(*)(void*))0)
#define SQLITE_TRANSIENT ((void(*)(void*))-1)

#ifdef __cplusplus
}
#endif

#endif /* SQLITE3_H */
