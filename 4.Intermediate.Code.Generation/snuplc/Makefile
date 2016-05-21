CC=g++
CCFLAGS=-std=c++0x -g -O0

SRC_DIR=src
OBJ_DIR=obj

DEPS=scanner.h \
		 parser.h \
		 type.h \
		 symtab.h \
		 data.h \
		 ast.h \
		 ir.h
SCANNER=scanner.cpp
PARSER=parser.cpp \
			 type.cpp \
			 symtab.cpp \
			 data.cpp \
			 ast.cpp \
			 ir.cpp
IR=
BACKEND=backend.cpp

DEPS_=$(patsubst %,$(SRC_DIR)/%,$(DEPS))
OBJ_SCANNER=$(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SCANNER))
OBJ_PARSER=$(patsubst %.cpp,$(OBJ_DIR)/%.o,$(PARSER) $(SCANNER))
OBJ_IR=$(patsubst %.cpp,$(OBJ_DIR)/%.o,$(IR) $(PARSER) $(SCANNER))

.PHONY: clean doc

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(DEPS_)
	$(CC) $(CCFLAGS) -c -o $@ $<

all: test_ir

test_scanner: $(OBJ_DIR)/test_scanner.o $(OBJ_SCANNER)
	$(CC) $(CCFLAGS) -o $@ $(OBJ_DIR)/test_scanner.o $(OBJ_SCANNER)

test_parser: $(OBJ_DIR)/test_parser.o $(OBJ_PARSER)
	$(CC) $(CCFLAGS) -o $@ $(OBJ_DIR)/test_parser.o $(OBJ_PARSER)

test_ir: $(OBJ_DIR)/test_ir.o $(OBJ_IR)
	$(CC) $(CCFLAGS) -o $@ $(OBJ_DIR)/test_ir.o $(OBJ_IR)

doc:
	doxygen

clean:
	rm -rf $(OBJ_DIR)/*.o test_scanner test_parser test_ir

mrproper: clean
	rm -rf doc/*

