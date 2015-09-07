#include "prometheus_microhttpd.h"
#include <prometheus/registry.hh>
#include <prometheus/output_formatter.hh>
#include <stdio.h>
#include <microhttpd.h>
#include <string>

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
  printf("In handle_metrics!\n");
  std::string
  MHD_Response* response = MHD_create_response_from_data(4,
                                                         (void*)"toto",
                                                         MHD_NO,
                                                         MHD_NO);
  return response;
}
