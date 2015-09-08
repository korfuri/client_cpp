CC=gcc-4.9
CXX=g++-4.9
LD=gcc-4.9

example: example.o prometheus_microhttpd.o
	$(LD) -o example $(LDFLAGS) prometheus_microhttpd.o example.o -lmicrohttpd -lstdc++ -L../../bazel-bin/prometheus -lprometheus_client_lib_lite -lprometheus_client_lib -lprometheus_output_formatter_lib -L../../bazel-bin/prometheus/proto -lmetrics_proto

prometheus_microhttpd.o: prometheus_microhttpd.cc prometheus_microhttpd.h
	$(CXX) $(CXXFLAGS) -std=c++14 -I../.. -I../../bazel-genfiles -c -o prometheus_microhttpd.o prometheus_microhttpd.cc

example.o: example.c prometheus_microhttpd.h
	$(CC) $(CFLAGS) -c -o example.o example.c

clean:
	rm -f example.o prometheus_microhttpd.o example

.PHONY: clean
