LD=gcc

example: example.o prometheus_microhttpd.o
	$(LD) -o example $(LDFLAGS) prometheus_microhttpd.o example.o -lmicrohttpd -L../../

prometheus_microhttpd.o: prometheus_microhttpd.cc prometheus_microhttpd.h
	$(CXX) $(CXXFLAGS) -std=c++14 -I../.. -I../../bazel-genfiles -c -o prometheus_microhttpd.o prometheus_microhttpd.cc

example.o: example.c prometheus_microhttpd.h
	$(CC) $(CFLAGS) -c -o example.o example.c

clean:
	rm -f example.o prometheus_microhttpd.o example

.PHONY: clean
