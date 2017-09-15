CC=		gcc
CFLAGS=		-g -gdwarf-2 -Wall -std=gnu99
LD=		gcc
LDFLAGS=	-L.
TARGETS=	spidey\
		spidey.o\
		forking.o\
		handler.o\
		request.o\
		single.o\
		socket.o\
		utils.o

all:		$(TARGETS)

spidey: spidey.o forking.o handler.o request.o single.o socket.o utils.o 
	@echo "Linking $@..."
	@$(LD) $(LDFLAGS) -o spidey spidey.o forking.o handler.o request.o single.o socket.o utils.o

spidey.o:	spidey.c
	@echo "Compiling $@..."
	@$(CC) $(CFLAGS) -c -o spidey.o spidey.c

forking.o:       forking.c
	@echo "Compiling $@..."
	@$(CC) $(CFLAGS) -c -o forking.o forking.c

handler.o:       handler.c
	@echo "Compiling $@..."
	@$(CC) $(CFLAGS) -c -o handler.o handler.c

request.o:       request.c
	@echo "Compiling $@..."
	@$(CC) $(CFLAGS) -c -o request.o request.c

single.o:       single.c
	@echo "Compiling $@..."
	@$(CC) $(CFLAGS) -c -o single.o single.c

socket.o:       socket.c
	@echo "Compiling $@..."
	@$(CC) $(CFLAGS) -c -o socket.o socket.c

utils.o:       utils.c
	@echo "Compiling $@..."
	@$(CC) $(CFLAGS) -c -o utils.o utils.c

clean: 
	@echo Cleaning...
	@rm -f $(TARGETS) *.o *.log *.input

.PHONY:		all clean
