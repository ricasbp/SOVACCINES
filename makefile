
OBJ_dir = obj
OBJECTOS = main.o client.o memory.o process.o proxy.o server.o synchronization.o sotime.o sosignal.o configuration.o stats.o

main.o = memory.h synchronization.h
client.o = memory.h main.h 
memory.o = memory-private.h
process.o = memory.h main.h
proxy.o = memory.h main.h
server.o = memory.h main.h
synchronization.o = semaphore.h 
sotime.o = sotime.h
sosignal.o = sosignal.h
configuration.o = configuration.h
stats.o = stats.h
CC = gcc
LIBS = -lrt -lpthread

sovaccines: $(OBJECTOS)
	$(CC) $(addprefix $(OBJ_dir)/,$(OBJECTOS)) -o bin/sovaccines $(LIBS)

%.o: src/%.c $($@)
	$(CC) -I include -o $(OBJ_dir)/$@ -c $<
	
