TARGET = main
INCLDIR = ./include
SRCDIR = src
BINDIR = bin
LIBS = 
CC = gcc
CFLAGS = -O0 -g
CFLAGS += -I$(INCLDIR)
RM = rm -f

OBJDIR = obj

# PATHS
_DEPS = main.h steFile.h

DEPS = $(patsubst %, $(INCLDIR)/%, $(_DEPS))

_OBJS = main.o steFile.o
OBJS = $(patsubst %, $(OBJDIR)/%, $(_OBJS))

# RULES
$(OBJDIR)/%.o: $(SRCDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
	
$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: remove valgrind run clean

remove:
	 rm -f *.ste

valgrind: remove main
	make
	valgrind --leak-check=yes --show-leak-kinds=all --track-origins=yes ./$(TARGET) CREA valgrind 8 8

run: remove main
	make
	./$(TARGET) CREA run 16 16

clean:
	$(RM) $(OBJDIR)/*.o *~core $(INCLDIR)/*~ $(TARGET)
