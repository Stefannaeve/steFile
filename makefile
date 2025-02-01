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
	
valgrind:
	make
	valgrind --leak-check=yes --show-leak-kinds=all --track-origins=yes ./$(TARGET) CREA valgrind 16 16

run:
	make
	./$(TARGET) CREA something 16 16
	
.PHONY: clean

clean:
	$(RM) $(OBJDIR)/*.o *~core $(INCLDIR)/*~ $(TARGET)
