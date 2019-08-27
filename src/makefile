OBJS = index_builder.o ranker.o min_queue.o
CC = c++-8
CFLAGS = -c
LFLAGS = 
VFLAGS = -std=c++11 -O2

all: main clean
	
main: $(OBJS)
	@mkdir -p ../bin/
	$(CC) $(LFLAGS) $(VFLAGS) main.cc $(OBJS) -o ../bin/main

%.o : %.cc
	$(CC) $(CFLAGS) $(VFLAGS) $< -o $@

clean:
	@rm -f *.o
	