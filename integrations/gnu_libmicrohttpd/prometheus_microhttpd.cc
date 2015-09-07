#include "prometheus_microhttpd.h"
#include <prometheus/registry.hh>
#include <prometheus/output_formatter.hh>
#include <prometheus/standard_exports.hh>
#include <stdio.h>
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
  MHD_Response* response = MHD_create_response_from_buffer(s.length(),
                                                           (void*)s.c_str(),
                                                           MHD_RESPMEM_MUST_COPY);
  if (response) {
    MHD_add_response_header(response, "Content-Type", TEXT_FORMAT_CONTENT_TYPE);
  }
  return response;
}

void install_process_exports() {
  ::prometheus::install_process_exports();
}
