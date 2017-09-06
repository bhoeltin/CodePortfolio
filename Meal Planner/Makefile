CC=		g++
CFLAGS=		-Wall -std=c++11
LD=		g++
LDFLAGS=	-L.
AR=		ar
ARFLAGS=	rcs
TARGETS=	script \
		test

all:		$(TARGETS)

test:		script webLoader.py
		@echo "Testing..."        
		@./webLoader.py

script:		script.o
		@echo "Linking $@..."
		@$(LD) $(LDFLAGS) -o script script.o

script.o:       script.cpp
		@echo "Compiling $@..."
		@$(CC) $(CFLAGS) -c -o script.o script.cpp

clean:
		@echo "Cleaning..."
		@rm -f $(TARGETS) *.o 
