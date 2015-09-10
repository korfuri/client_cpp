#include "prometheus_microhttpd.h"
#include <prometheus/client.hh>
#include <prometheus/registry.hh>
#include <prometheus/output_formatter.hh>
#include <prometheus/standard_exports.hh>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <microhttpd.h>
#include <string>
#include <sstream>

#define TEXT_FORMAT_CONTENT_TYPE "text/plain; version=0.0.4"

void collect_as_text_format_to_ostream(std::ostream& os) {
  auto v = prometheus::impl::global_registry.collect();
  for (auto mf : v) {
    prometheus::metricfamily_proto_to_ostream(os, mf);
    prometheus::delete_metricfamily(mf);
  }
}

std::string collect_as_text_format_to_string() {
  std::ostringstream ss;
  collect_as_text_format_to_ostream(ss);
  return ss.str();
}

MHD_Response* handle_metrics(struct MHD_Connection* connection) {
  // Determines the content-type to use for the response.  We don't
  // implement a full content-type negociation here: if the client
  // specifies explicitly that it accepts
  // application/vnd.google.protobuf responses, we'll send a binary
  // response. Otherwise we send a text response.
  /* const char* accept_header = MHD_lookup_connection_value(connection, MHD_HEADER_KIND, "Accept"); */
  /* if (!accept_header) { */
  /*   accept_header = "text/plain"; */
  /* } */
  /* printf("Accepting: %s\n", accept_header); */
  std::string s = collect_as_text_format_to_string();
  MHD_Response* response = MHD_create_response_from_data(s.length(),
                                                         (void*)s.c_str(),
                                                         MHD_NO,
                                                         MHD_YES);
  if (response) {
    MHD_add_response_header(response, "Content-Type", TEXT_FORMAT_CONTENT_TYPE);
  }
  return response;
}

using prometheus::Counter;

Counter<2> requests_total("libmicrohttpd_http_requests_by_transport_method_total",
                          "Number of processed HTTP requests.",
                          {"transport", "method"});

void record_stats_before_access_handler(struct MHD_Connection* connection,
                                        const char* url,
                                        const char* method,
                                        const char* version,
                                        const char* upload_data,
                                        size_t* upload_data_size) {
  const union MHD_ConnectionInfo* connection_info;

  connection_info = MHD_get_connection_info(connection, MHD_CONNECTION_INFO_PROTOCOL);
  const std::string transport = (connection_info != NULL) ? "https" : "http";

  if (strcmp("GET", method) && strcmp("HEAD", method) &&
      strcmp("POST", method) && strcmp("PUT", method) &&
      strcmp("DELETE", method) && strcmp("TRACE", method) &&
      strcmp("OPTIONS", method) && strcmp("CONNECT", method) &&
      strcmp("PATCH", method)) {
    method = "<invalid method>";
  }

  requests_total.labels({transport, std::string(method)}).inc();
}

Counter<1> responses_total("libmicrohttpd_http_responses_by_status_total",
                           "Number of processed HTTP responses by HTTP status.",
                           {"status"});

void record_stats_before_queue_response(int http_status,
                                        struct MHD_Response* response) {
  responses_total.labels({std::to_string(http_status)}).inc();
}

void install_process_exports() {
  ::prometheus::install_process_exports();
}
