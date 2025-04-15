TARGET = main
INCLDIR = ./include
SRCDIR = src
BINDIR = bin
IMAGEDIR = images
LIBS = 
CC = gcc
CFLAGS = -O0 -g
CFLAGS += -I$(INCLDIR) -lSDL2
RM = rm -f

OBJDIR = obj

# PATHS
_DEPS = main.h steFile.h SNLogger.h createSteFile.h windowApplicationSteFile.h

DEPS = $(patsubst %, $(INCLDIR)/%, $(_DEPS))

_OBJS = main.o SNLogger.o createSteFile.o windowApplicationSteFile.o
OBJS = $(patsubst %, $(OBJDIR)/%, $(_OBJS))

# RULES
$(OBJDIR)/%.o: $(SRCDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
	
$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: remove valgrindCREA valgrindREAD runCREA runREAD clean xxd

remove:
	 rm -f *.ste && rm -f debug.txt

valgrindCREA: remove main
	make
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes -s ./$(TARGET) -c

valgrindREAD: remove main
	make
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes -s ./$(TARGET) -r

runCREA: remove main
	make
	./$(TARGET) CREA run 16 16

runREAD: remove main
	make
	./$(TARGET) READ

clean:
	$(RM) $(OBJDIR)/*.o $(IMAGEDIR)/*.ste $(TARGET)

# g is how many bytes, -u is uppercase hex letters, -c is columns
xxd:
	xxd -g 1 -u -c 8 $(IMAGEDIR)/snakeHead.ste
