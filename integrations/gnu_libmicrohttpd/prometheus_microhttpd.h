#ifndef PROMETHEUS_MICROHTTPD_H__
#define PROMETHEUS_MICROHTTPD_H__

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

  struct MHD_Connection;
  struct MHD_Response;

  // Records metrics before running the AccessHandlerCallback.  It's
  // recommended you call this function first thing in your AHC
  // callback.
  void record_stats_before_access_handler(struct MHD_Connection* connection,
                                          const char* url,
                                          const char* method,
                                          const char* version,
                                          const char* upload_data,
                                          size_t* upload_data_size);

  // Records stats just before sending a response.
  // Call this just before calling MHD_queue_response.
  void record_stats_before_queue_response(int http_status,
                                          struct MHD_Response* response);

  // Handles a /metrics request. The caller is responsible for
  // checking that the URL path corresponds to /metrics or to the
  // locally configured alternative. handle_metrics will determine the
  // content-type to be used based on the Accept header sent by the
  // client (defaulting to text/plain if unspecified).
  // The callee gains ownership of the MHD_Response instance and
  // should destroy it with MHD_destroy_response.
  struct MHD_Response* handle_metrics(struct MHD_Connection* connection);

  // Installs the standard exports in the current process. You
  // probably want to call this in your main(). Exports can't be
  // uninstalled.
  void install_process_exports();

#ifdef __cplusplus
};
#endif  /* __cplusplus */

#endif  /* PROMETHEUS_MICROHTTPD_H__ */
