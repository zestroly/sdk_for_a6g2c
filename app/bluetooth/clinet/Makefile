EXEC	= rfcomm_client
OBJS    = rfcomm_client.o

CROSS	= arm-linux-gnueabihf-
CC	     = $(CROSS)gcc 
STRIP	= $(CROSS)strip

CFLAGS	= -Wall -g -O2 -lbluetooth

all:  clean $(EXEC)

$(EXEC):$(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)
	$(STRIP) $@

clean:
	-rm -f $(EXEC) *.o
