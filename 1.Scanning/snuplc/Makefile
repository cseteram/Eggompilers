CC=g++
CCFLAGS=-std=c++0x -g -O0

SRC_DIR=src
OBJ_DIR=obj

DEPS=scanner.h
SCANNER=scanner.cpp

DEPS_=$(patsubst %,$(SRC_DIR)/%,$(DEPS))
OBJ_SCANNER=$(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SCANNER))

.PHONY: clean doc

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(DEPS_)
	$(CC) $(CCFLAGS) -c -o $@ $<

all: test_scanner

test_scanner: $(OBJ_DIR)/test_scanner.o $(OBJ_SCANNER)
	$(CC) $(CCFLAGS) -o $@ $(OBJ_DIR)/test_scanner.o $(OBJ_SCANNER)

doc:
	doxygen

clean:
	rm -rf $(OBJ_DIR)/*.o test_scanner

mrproper: clean
	rm -rf doc/*

