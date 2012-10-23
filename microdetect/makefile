EXE = microdetect
OBJ := $(patsubst %.cpp,%.o,$(wildcard *.cpp))


LIBS = 
OPT = -O3 -Wall -Wextra -g

all: $(EXE)

$(EXE):	$(OBJ)
	g++ -o $(EXE) $(OBJ) $(OPT) $(LIBS)

$(OBJ):%.o: %.cpp %.h 
	g++ -c $< $(OPT)

clean:
	rm $(OBJ) $(EXE) *o tmp Log.txt `find . -name "*~"`  *s 2> /dev/null || echo "Done cleaning"

count:
	wc $(wildcard *.cpp) $(wildcard *h)

countfiles:
	find . -name "*cpp" |wc -l; find . -name "*h" | wc -l; find data -name "*txt" |wc -l
