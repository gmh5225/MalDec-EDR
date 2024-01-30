#include "inspector/inspector.h"
#include "logger/logger.h"
#include <errno.h>
#include <fcntl.h>
#include <features.h>
#include <sys/stat.h>
#include <unistd.h>

#define INSPECTOR_PERMISSIONS 0775
#define QUARANTINE_PERMISSIONS (S_IRUSR | S_IWUSR | S_IXUSR)

inline ERR
init_inspector(INSPECTOR **inspector, INSPECTOR_CONFIG config)
{
  *inspector = malloc(sizeof(struct INSPECTOR));
  ALLOC_ERR_FAILURE(*inspector);

  (*inspector)->config = config;

  struct stat sb;
  if (stat((*inspector)->config.dir, &sb) == 0 && S_ISDIR(sb.st_mode)) {}
  else
  {
    if (mkdir((*inspector)->config.dir, INSPECTOR_PERMISSIONS) == -1)
    {
      LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE %d  (%s), '%s'", errno,
                                   strerror(errno), (*inspector)->config.dir));
      return ERR_FAILURE;
    }
  }

  (*inspector)->ins_fd_dir = open((*inspector)->config.dir, O_RDWR);
  if ((*inspector)->ins_fd_dir < 0)
  {
    LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE %d  (%s), '%s'", errno,
                                 strerror(errno), (*inspector)->config.dir));
    return ERR_FAILURE;
  }

  if (fstatat((*inspector)->ins_fd_dir, (*inspector)->config.quarantine.dir,
              &sb, 0) == 0 &&
      S_ISDIR(sb.st_mode))
  {
  }
  else
  {
    if (mkdirat((*inspector)->ins_fd_dir, (*inspector)->config.quarantine.dir,
                QUARANTINE_PERMISSIONS) == -1)
    {
      LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE %d (%s), '%s'", errno,
                                   strerror(errno),
                                   (*inspector)->config.quarantine.dir));
      return ERR_FAILURE;
    }
  }

  (*inspector)->qua_fd_dir = openat((*inspector)->ins_fd_dir,
                                    (*inspector)->config.quarantine.dir,
                                    O_RDWR);

  if ((*inspector)->qua_fd_dir < 0)
  {
    LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE %d  (%s), '%s'", errno,
                                 strerror(errno),
                                 (*inspector)->config.quarantine.dir));
    return ERR_FAILURE;
  }

  return ERR_SUCCESS;
}

inline ERR
add_quarentine_inspector(INSPECTOR *inspector)
{
  printf("%s\n", inspector->config.path);
  return ERR_SUCCESS;
}

inline ERR
del_quarentine_inspector(INSPECTOR *inspector)
{
  unlinkat(inspector->qua_fd_dir, inspector->config.path, 0);
  return ERR_SUCCESS;
}

inline ERR
mov_quarentine_inspector(INSPECTOR *inspector)
{
  printf("%s\n", inspector->config.path);
  return ERR_SUCCESS;
}

inline ERR
view_quarentine_inspector(INSPECTOR *inspector)
{
  printf("%s\n", inspector->config.path);
  return ERR_SUCCESS;
}

inline void
exit_inspector(INSPECTOR **inspector)
{
  close((*inspector)->ins_fd_dir);
  free(*inspector);
  NO_USE_AFTER_FREE(*inspector);
}