CXX=clang++
LD=clang++
CXXFLAGS=-Wall -Wextra -Wno-missing-braces -pedantic -std=c++14
OBJS=client_cpp.o

client_cpp: $(OBJS)
	$(LD) -o $@ $<

test_client_cpp: test_client_cpp.o
	$(LD) -o $@ $<

clean:
	rm -f client_cpp $(OBJS)

test: test_client_cpp
	./test_client_cpp

.PHONY: clean test
