#include "bus_methods.h"
#include "edr/edr.h"
#include "inspector/inspector.h"
#include "scanner/scanner.h"

EDR *edr;

void *
thread(void *args)
{
  // Workaround for `invalid pointer` (Abort)
  SCANNER *_scanner_copy = calloc(1, sizeof(SCANNER));
  memcpy(_scanner_copy, (SCANNER*)args, sizeof(SCANNER));
  if (IS_ERR_FAILURE(scan_listen_inotify(_scanner_copy)))
  {
    fprintf(stderr, LOG_MESSAGE_FORMAT("Error scan inotify\n"));
  }

  free(_scanner_copy);
  return NULL;
}

int
init_all(void)
{
  EDR_CONFIG config = (EDR_CONFIG){
  #ifdef DEBUG
            .settings_json_path = "../../../config/"
                                  "appsettings.development.json",

  #else
            .settings_json_path = "../../../config/appsettings.json"
  #endif
  };

  pthread_t  tid;

  init_edr(&edr, config);

  init_cjson_main(&edr);
  init_logger_main(&edr);
  init_inspector_main(&edr);
  init_inotify_main(&edr);
  init_scanner_main(&edr);

  edr->scanner->config.inotify   = edr->inotify;
  edr->scanner->config.inspector = edr->inspector;
  edr->scanner->config.inotify->config.mask = (IN_MODIFY | IN_CLOSE_WRITE | IN_CREATE);
  edr->scanner->config.inotify->config.time = -1;

  set_watch_paths(edr->inotify);

  pthread_create(&tid, NULL, thread, (void *)edr->scanner);

  return tid;
}

void
end_all()
{
  if (IS_NULL_PTR(edr)) exit(EXIT_SUCCESS);

  if (!IS_NULL_PTR(edr->cjson)) exit_json(&edr->cjson);

  if (!IS_NULL_PTR(edr->logger)) exit_logger(&edr->logger);

  if (!IS_NULL_PTR(edr->scanner))
    if (IS_ERR_FAILURE(exit_scanner(&edr->scanner)))
      printf(LOG_MESSAGE_FORMAT("Error in exit scanner"));

  if (!IS_NULL_PTR(edr->inotify)) exit_inotify(&edr->inotify);

  if (!IS_NULL_PTR(edr->inspector)) exit_inspector(&edr->inspector);

  exit_edr(&edr);
}

int
method_init_params(sd_bus_message *m, void *userdata, sd_bus_error *ret_error)
{
  // unused parameters
  userdata = userdata;
  ret_error = ret_error;

  bool    verbose   = false;
  int     max_depth = 0;
  uint8_t scan_type = 0;

  int r = 0;

  r = sd_bus_message_read(m, "biy", &verbose, &max_depth, &scan_type);
  if (r < 0)
  {
    printf("(InitParams) Failed to connect to system bus: %i, %s\n", r,
           strerror(r));
  }

  edr->scanner->config.max_depth = max_depth;
  edr->scanner->config.verbose   = verbose;
  edr->scanner->config.scan_type = scan_type;

  return sd_bus_reply_method_return(m, "i", r);
}

int
method_scan(sd_bus_message *m, void *userdata, sd_bus_error *ret_error)
{
  // unused parameters
  userdata = userdata;
  ret_error = ret_error;

  char *filepath = NULL;
  int   r        = 0;

  r = sd_bus_message_read(m, "s", &filepath);

  edr->scanner->config.filepath  = filepath;
  edr->scanner->config.inspector = edr->inspector;

  if (!IS_NULL_PTR(edr->scanner))
    if (IS_ERR_FAILURE(scan_files_and_dirs(edr->scanner)))
      fprintf(stderr, LOG_MESSAGE_FORMAT("Error in scan\n"));

  return sd_bus_reply_method_return(m, "i", r);
}

int
method_driver_crowarmor(sd_bus_message *m, void *userdata,
                        sd_bus_error *ret_error)
{
  // unused parameters
  userdata = userdata;
  ret_error = ret_error;

  init_crowarmor_main(&edr);
  check_driver_crowarmor_activated(edr->crowarmor);

  return sd_bus_reply_method_return(m, "i", 0);
}

int
method_quarantine_view(sd_bus_message *m, void *userdata,
                       sd_bus_error *ret_error)
{
  // unused parameters
  userdata = userdata;
  ret_error = ret_error;

  const char *json = NULL;

  if (IS_ERR_FAILURE(view_json_dump_inspector(edr->inspector, &json)))
  {
    fprintf(stderr, LOG_MESSAGE_FORMAT("Error View quarantine\n"));
  }

  return sd_bus_reply_method_return(m, "s", json);
}

int
method_quarantine_sync(sd_bus_message *m, void *userdata,
                       sd_bus_error *ret_error)
{
  // unused parameters
  userdata = userdata;
  ret_error = ret_error;

  int r = 0;

  if (IS_ERR_FAILURE(sync_quarantine_inspector(edr->inspector,
                                               DEFAULT_SYNC_QUARANTINE)))
  {
    fprintf(stderr, LOG_MESSAGE_FORMAT("Error Sync quarantine\n"));
    r = 1;
  }

  return sd_bus_reply_method_return(m, "i", r);
}

int
method_quarantine_restore(sd_bus_message *m, void *userdata,
                          sd_bus_error *ret_error)
{
  // unused parameters
  userdata = userdata;
  ret_error = ret_error;

  int r, id;

  r = sd_bus_message_read(m, "u", &id);

  if (r < 0)
  {
    printf("(Restore) Failed to connect to system bus: %i, %s\n", r,
           strerror(r));
  }

  QUARANTINE_FILE file = (QUARANTINE_FILE){.id = id};

  if (IS_ERR_FAILURE(restore_quarantine_inspector(edr->inspector, &file)))
  {
    fprintf(stderr, LOG_MESSAGE_FORMAT("Error View quarantine\n"));
    return 1;
  }

  return sd_bus_reply_method_return(m, "i", r);
}

int
method_quarantine_delete(sd_bus_message *m, void *userdata,
                         sd_bus_error *ret_error)
{
  // unused parameters
  userdata = userdata;
  ret_error = ret_error;

  int r, id;

  r = sd_bus_message_read(m, "u", &id);

  if (r < 0)
  {
    printf("(Delete) Failed to connect to system bus: %i, %s\n", r,
           strerror(r));
  }

  QUARANTINE_FILE file = (QUARANTINE_FILE){.id = id};

  if (IS_ERR_FAILURE(del_quarantine_inspector(edr->inspector, &file)))
  {
    fprintf(stderr, LOG_MESSAGE_FORMAT("Error View quarantine\n"));
    return 1;
  }

  return sd_bus_reply_method_return(m, "i", r);
}