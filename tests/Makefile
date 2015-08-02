without_bazel.o: without_bazel.cpp
	g++ -I../bazel-client_cpp -o without_bazel.o -W -Wall -std=c++0x -c without_bazel.cpp

without_bazel: without_bazel.o
	g++ -o without_bazel without_bazel.o -L../bazel-bin/prometheus -L../bazel-bin/prometheus/proto -L../bazel-bin/external/protobuf -lclient_lib -lmetrics_proto -lprotobuf_lite -lprotobuf
