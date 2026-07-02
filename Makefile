# fqcheck v2.09
CXX=g++
BIN := ./bin
RAPIDGZIP_DIR := ./src/rapidgzip/src

LDFLAGS=-lz -fopenmp
INCLUDE=-I$(RAPIDGZIP_DIR)

CXXFLAGS= -g  -O3  -std=c++17 -fopenmp -march=native $(INCLUDE)

FQCHEX_OBJ := $(BIN)/../src/fqcheck.o
GLUE_OBJ   := $(BIN)/../src/rapidgzip/libglue_rapidgzip.o

all: $(BIN)/fqcheck

$(BIN)/fqcheck: $(FQCHEX_OBJ) $(GLUE_OBJ)
	$(CXX) $^ -o $@ $(LDFLAGS) $(INCLUDE)

$(FQCHEX_OBJ): src/fqcheck.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

$(GLUE_OBJ): src/rapidgzip/src/libglue_rapidgzip.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

clean:
	$(RM) $(BIN)/*.o $(BIN)/../src/*.o $(BIN)/../src/rapidgzip/*.o
