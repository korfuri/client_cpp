CXX=g++
LD=g++
CXXFLAGS=-Wall -Wextra -pedantic -std=c++14
OBJS=client_cpp.o

client_cpp: $(OBJS)
	$(LD) -o $@ $<

clean:
	rm -f client_cpp $(OBJS)

.PHONY: clean
