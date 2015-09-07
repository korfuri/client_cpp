#ifndef PROMETHEUS_MICROHTTPD_H__
#define PROMETHEUS_MICROHTTPD_H__

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

  struct MHD_Connection;
  struct MHD_Response;

  // Handles a /metrics request. The caller is responsible for
  // checking that the URL path corresponds to /metrics or to the
  // locally configured alternative. handle_metrics will determine the
  // content-type to be used based on the Accept header sent by the
  // client (defaulting to text/plain if unspecified).
  // The callee gains ownership of the MHD_Response instance and
  // should destroy it with MHD_destroy_response.
  struct MHD_Response* handle_metrics(struct MHD_Connection* connection);

#ifdef __cplusplus
};
#endif  /* __cplusplus */

#endif  /* PROMETHEUS_MICROHTTPD_H__ */
