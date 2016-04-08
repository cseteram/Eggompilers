CC=g++
CCFLAGS=-std=c++0x -g -O0

SRC_DIR=src
OBJ_DIR=obj
LIB_DIR=lib

DEPS=scanner.h \
		 parser.h \
		 type.h \
		 data.h \
		 symtab.h \
		 ast.h \
		 ir.h
SCANNER=scanner.cpp
PARSER=parser.cpp \
			 type.cpp \
			 data.cpp \
			 symtab.cpp \
			 ast.cpp \
			 ir.cpp

DEPS_=$(patsubst %,$(SRC_DIR)/%,$(DEPS))
LIB_SCANNER=$(patsubst %.cpp,$(LIB_DIR)/%.o,$(SCANNER))
OBJ_PARSER=$(patsubst %.cpp,$(OBJ_DIR)/%.o,$(PARSER))

.PHONY: clean doc

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(DEPS_)
	$(CC) $(CCFLAGS) -c -o $@ $<

all: test_scanner test_parser

test_scanner: $(OBJ_DIR)/test_scanner.o
	$(CC) $(CCFLAGS) -o $@ $(OBJ_DIR)/test_scanner.o $(LIB_SCANNER)

test_parser: $(OBJ_DIR)/test_parser.o $(OBJ_PARSER)
	$(CC) $(CCFLAGS) -o $@ $(OBJ_DIR)/test_parser.o $(LIB_SCANNER) $(OBJ_PARSER)

doc:
	doxygen

clean:
	rm -rf $(OBJ_DIR)/*.o test_scanner test_parser

mrproper: clean
	rm -rf doc/*

