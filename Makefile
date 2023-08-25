CXX=g++
BIN := ./bin

LDFLAGS=-lz
INCLUDE=-I./include
LIBS=-L./lib

CXXFLAGS= -g  -O3  $(INCLUDE) $(LIBS)
#  you can add the [ -I /path/.../samtools-1.6/htslib-1.6   -L /path/.../samtools-1.6/htslib-1.6 ] here #no need by fqcheck


all: $(BIN)/fqcheck

$(BIN)/fqcheck: $(BIN)/../src/fqcheck.o 
	$(CXX)   $^ -o $@   $(LDFLAGS)  $(INCLUDE) $(LIBS)

$(BIN)/%.o: %.cpp
	$(CXX)  -c $(CXXFLAGS)  $< -o $@ 

clean:
	$(RM) $(BIN)/*.o  $(BIN)/../src/*.o

