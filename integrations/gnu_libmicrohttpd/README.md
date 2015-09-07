# GNU libmicrohttpd integration

## Status

In progress.

## How to test

````shell
$ pwd
/foo/bar/client_cpp
$ bazel build //...
$ cd integrations/gnu_libmicrohttpd/
$ make
$ LD_LIBRARY_PATH=../../bazel-bin/prometheus:../../bazel-bin/prometheus/proto:$LD_LIBRARY_PATH ./example
````

The demo server will listen on port 8080. You can try it like so:

````shell
$ curl -v http://127.0.0.1:8080/metrics | tail -n 6
# HELP process_cpu_seconds_total Total user and system CPU time spent in seconds.
# TYPE process_cpu_seconds_total gauge
process_cpu_seconds_total = 0.01
# HELP prometheus_client_collection_errors_total Count of exceptions raised by collectors during the metric collection process.
# TYPE prometheus_client_collection_errors_total counter
prometheus_client_collection_errors_total = 0
$
````
