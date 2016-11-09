#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <microhttpd.h>
#include "prometheus_microhttpd.h"

#define PAGE "<html><head><title>libmicrohttpd demo</title>"\
             "</head><body>libmicrohttpd demo</body></html>"

static int ahc_echo(void * cls,
                    struct MHD_Connection * connection,
                    const char * url,
                    const char * method,
                    const char * version,
                    const char * upload_data,
                    size_t * upload_data_size,
                    void ** ptr) {
  static int dummy;
  const char * page = cls;
  struct MHD_Response * response;
  int ret;

  record_stats_before_access_handler(connection, url, method, version, upload_data, upload_data_size);
  if (0 != strcmp(method, "GET"))
    return MHD_NO; /* unexpected method */
  if (!strcmp(url, "/metrics")) {
    response = handle_metrics(connection);
  } else {
    if (&dummy != *ptr)
      {
        /* The first time only the headers are valid,
           do not respond in the first round... */
        *ptr = &dummy;
        return MHD_YES;
      }
    if (0 != *upload_data_size)
      return MHD_NO; /* upload data in a GET!? */
    *ptr = NULL; /* clear context pointer */
    response = MHD_create_response_from_data(strlen(page),
                                             (void*) page,
                                             MHD_NO,
                                             MHD_NO);
  }
  record_stats_before_queue_response(MHD_HTTP_OK, response);
  ret = MHD_queue_response(connection,
                           MHD_HTTP_OK,
                           response);
  MHD_destroy_response(response);
  return ret;
}

int main() {
  install_process_exports();
  struct MHD_Daemon * d;
  d = MHD_start_daemon(MHD_USE_THREAD_PER_CONNECTION,
                       8080,
                       NULL,
                       NULL,
                       &ahc_echo,
                       PAGE,
                       MHD_OPTION_END);
  if (d == NULL)
    return 1;
  for (;;) { sleep(60); }
  return 0;
}
