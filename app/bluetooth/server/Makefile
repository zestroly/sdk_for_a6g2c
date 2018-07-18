EXEC	= rfcomm_server
OBJS    = rfcomm_server.o

CROSS	= arm-linux-gnueabihf-
CC	     = $(CROSS)gcc 
STRIP	= $(CROSS)strip
CFLAGS	= -Wall -g -O2 -lbluetooth -Iinclude -Llib

all:  clean $(EXEC)

$(EXEC):$(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)
	$(STRIP) $@

clean:
	-rm -f $(EXEC) *.o
