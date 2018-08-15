IDIR=Include
SRC=Source
TEST=Test
CC=g++
CFLAGS=-I$(IDIR) -Ilibbson-1.0 -lbson-1.0 -lhiredis -levent -pthread -std=c++17

ODIR=Bin

_DEPS=$(shell ls $(IDIR))
DEPS=$(patsubst %,$(IDIR)/%,$(_DEPS))

SRCS:=$(shell ls $(SRC) && ls $(TEST))

OBJ=$(patsubst %.cpp,$(ODIR)/%.o,$(SRCS))

$(ODIR)/%.o: $(SRC)/%.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(ODIR)/%.o: $(TEST)/%.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

tests: $(filter-out $(ODIR)/testDecoder.o,$(OBJ))
	$(CC) -o $@ $^ $(CFLAGS)

testDecoder: $(filter-out $(ODIR)/tests.o,$(OBJ))
	$(CC) -o $@ $^ $(CFLAGS)	

debug:
	echo "$(OBJ)" && echo "$(ODIR)/"
