/**
 * @file inspector.h
 * @brief Header file for the inspector.
 */

#pragma once

#include "compression/zlib.h"
#include "config.h"
#include "err/err.h"
#include <sqlite3.h>

/**
 * @brief Macro to specify the default view quarantine function.
 */
#define DEFAULT_VIEW_QUARANTINE default_view_quarantine

/**
 * @brief Macro to specify the default sync quarantine function.
 */
#define DEFAULT_SYNC_QUARANTINE default_sync_quarantine

/**
 * @brief Structure representing an inspector.
 */
typedef struct INSPECTOR
{
  INSPECTOR_CONFIG config;     /**< Configuration for the inspector. */
  ZLIB            *zlib;       /**< Pointer to the zlib library. */
  int              ins_fd_dir; /**< File descriptor for inspection directory. */
  int              qua_fd_dir; /**< File descriptor for quarantine directory. */
  sqlite3         *db;         /**< Pointer to SQLite database. */
  sqlite3_stmt    *stmt;       /**< SQLite statement. */
} INSPECTOR;

/**
 * @brief Initializes an inspector.
 * @param[out] inspector Pointer to the allocated inspector structure.
 * @param[in] config Configuration for the inspector.
 * @return ERR code indicating success or failure.
 */
ERR
init_inspector(INSPECTOR      **inspector,
               INSPECTOR_CONFIG config) warn_unused_result;

/**
 * @brief Adds a file to quarantine.
 * @param[in] inspector Pointer to the inspector.
 * @param[in] file Pointer to the file to be quarantined.
 * @return ERR code indicating success or failure.
 */
ERR
add_quarantine_inspector(INSPECTOR       *inspector,
                         QUARANTINE_FILE *file) warn_unused_result;

/**
 * @brief Removes a file from quarantine.
 * @param[in] inspector Pointer to the inspector.
 * @param[in] file Pointer to the file to be removed from quarantine.
 * @return ERR code indicating success or failure.
 */
ERR
del_quarantine_inspector(INSPECTOR       *inspector,
                         QUARANTINE_FILE *file) warn_unused_result;

/**
 * @brief Restores a file from quarantine.
 * @param[in] inspector Pointer to the inspector.
 * @param[in] file Pointer to the file to be restored from quarantine.
 * @return ERR code indicating success or failure.
 */
ERR
restore_quarantine_inspector(INSPECTOR       *inspector,
                             QUARANTINE_FILE *file) warn_unused_result;

/**
 * @brief execute sql in quarantine files.
 * @param[in] inspector Pointer to the inspector.
 * @param[in] callback Callback function to process quarantine files.
 * @return ERR code indicating success or failure.
 */
ERR
view_quarantine_inspector(INSPECTOR *inspector,
                          int (*callback)(void *ins, int, char **,
                                          char **)) warn_unused_result;
ERR
view_json_dump_inspector(INSPECTOR *inspector,
                         const char **__restrict__ json_dump) warn_unused_result;
/**
 * @brief Syncs quarantine files.
 * @param[in] inspector Pointer to the inspector.
 * @param[in] callback Callback function to sync quarantine files.
 * @return ERR code indicating success or failure.
 */
ERR
sync_quarantine_inspector(INSPECTOR *inspector,
                          int (*callback)(void *ins, int, char **,
                                          char **)) warn_unused_result;

/**
 * @brief Exits the inspector.
 * @param[in,out] inspector Pointer to the pointer to the inspector.
 */
void
exit_inspector(INSPECTOR **inspector);

/**
 * @brief Default sync quarantine callback function.
 * @param[in] ins Pointer to the inspector.
 * @param[in] count Number of results.
 * @param[in] data Data.
 * @param[in] columns Column names.
 * @return Result of the operation.
 */
int
default_sync_quarantine(void *ins, const int count, char **data,
                        char **columns) warn_unused_result;

/**
 * @brief Inserts a quarantine file into the database.
 * @param[in,out] inspector Pointer to the pointer to the inspector.
 * @param[in,out] file Pointer to the pointer to the quarantine file.
 * @return ERR code indicating success or failure.
 */
ERR
insert_quarantine_db(INSPECTOR       **inspector,
                     QUARANTINE_FILE **file) warn_unused_result;

/**
 * @brief Selects quarantine files from the database based on a condition.
 * @param[in,out] inspector Pointer to the pointer to the inspector.
 * @param[in,out] file Pointer to the pointer to the quarantine file.
 * @return ERR code indicating success or failure.
 */
ERR
select_where_quarantine_db(INSPECTOR       **inspector,
                           QUARANTINE_FILE **file) warn_unused_result;

/**
 * @brief Deletes quarantine files from the database based on a condition.
 * @param[in,out] inspector Pointer to the pointer to the inspector.
 * @param[in,out] file Pointer to the pointer to the quarantine file.
 * @return ERR code indicating success or failure.
 */
ERR
delete_where_quarantine_db(INSPECTOR       **inspector,
                           QUARANTINE_FILE **file) warn_unused_result;

/**
 * @brief Selects all quarantine files from the database.
 * @param[in] inspector Pointer to the inspector.
 * @param[in] callback Callback function to process quarantine files.
 * @return ERR code indicating success or failure.
 */
ERR
select_all_quarantine_db(INSPECTOR **inspector,
                         int (*callback)(void *, int, char **,
                                         char **)) warn_unused_result;

ERR
select_all_quarantine_json_db(INSPECTOR **inspector,
                              const char **__restrict__ json_dump)
        warn_unused_result;

/**
 * @brief Exits and finalizes the SQLite statement.
 * @param[in,out] inspector Pointer to the pointer to the inspector.
 */
void
exit_stmt_finalize(INSPECTOR **inspector);
