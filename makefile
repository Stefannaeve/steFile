TARGET = main
INCLDIR = ./include
SRCDIR = src
BINDIR = bin
LIBS = 
CC = gcc
CFLAGS = -O0 -g
CFLAGS += -I$(INCLDIR) -lSDL2
RM = rm -f

OBJDIR = obj

# PATHS
_DEPS = main.h steFile.h SNLogger.h

DEPS = $(patsubst %, $(INCLDIR)/%, $(_DEPS))

_OBJS = main.o steFile.o SNLogger.o
OBJS = $(patsubst %, $(OBJDIR)/%, $(_OBJS))

# RULES
$(OBJDIR)/%.o: $(SRCDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
	
$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: remove valgrindCREA valgrindREAD runCREA runREAD clean

remove:
	 rm -f *.ste && rm -f debug.txt

valgrindCREA: remove main
	make
	valgrind --leak-check=yes --show-leak-kinds=all --track-origins=yes ./$(TARGET) CREA valgrind 8 8

valgrindREAD: remove main
	make
	valgrind --leak-check=yes --show-leak-kinds=all --track-origins=yes ./$(TARGET) READ

runCREA: remove main
	make
	./$(TARGET) CREA run 16 16

runREAD: remove main
	make
	./$(TARGET) READ

clean:
	$(RM) $(OBJDIR)/*.o *~core $(INCLDIR)/*~ $(TARGET)
