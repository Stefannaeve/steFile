TARGET = main
CC = gcc
CFLAGS = -O0 -g
CFLAGS += -I$(INCLDIR) -lSDL2 -DTEST # -DTEST can be used for compiling only for development causes
RM = rm -f

INCLDIR = ./include
SRCDIR = src
OBJDIR = obj
LIBDIR = lib

LIBS =
DEPS =
SRC =
OBJS =

$(shell mkdir -p $(INCLDIR) $(SRCDIR) $(OBJDIR) $(LIBDIR))

# PATHS
#_DEPS = main.h steFile.h SNLogger.h createSteFile.h windowApplicationSteFile.h
#DEPS = $(patsubst %, $(INCLDIR)/%, $(_DEPS))
DEPS = $(wildcard $(INCLDIR)/*.h)
SRC = $(wildcard $(SRCDIR)/*.c)

#_OBJS = main.o SNLogger.o createSteFile.o windowApplicationSteFile.o
#OBJS = $(patsubst %, $(OBJDIR)/%, $(_OBJS))
OBJS = $(SRC:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

# RULES
$(OBJDIR)/%.o: $(SRCDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
	
$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LIBS) $(CFLAGS)

.PHONY: remove valgrindCREA valgrindREAD runCREA runREAD clean xxd createLibrary

createLibrary:
	$(CC) -c $(SRCDIR)/createSteFile.c -O0 -g -o $(OBJDIR)/createSteFile.o
	ar rcs lib/createSteFile.a $(OBJDIR)/createSteFile.o

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
	./$(TARGET) -c

runREAD: remove main
	make
	./$(TARGET) -r

clean:
	$(RM) $(OBJDIR)/*.o $(LIBDIR)/*.a $(TARGET)

# g is how many bytes, -u is uppercase hex letters, -c is columns
xxd:
	xxd -g 1 -u -c 8 images/snakeHead.ste
