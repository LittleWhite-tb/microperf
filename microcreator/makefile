EXE=microcreator

MAIN_OBJ := $(patsubst %.cpp,obj/%.o,$(wildcard *.cpp))
CORE_OBJ := $(patsubst Core/Src/%.cpp,obj/%.o,$(wildcard Core/Src/*.cpp))
PASS_OBJ := $(patsubst Passes/Src/%.cpp,obj/%.o,$(wildcard Passes/Src/*.cpp))
LIBS = `pkg-config libxml++-2.6 --cflags --libs` -ldl

OPT_INCLUDE = -ICore/Include -IPasses/Include
OPT = -O3 -Wall -Wextra -g $(OPT_INCLUDE) 

FULL_OBJ = $(MAIN_OBJ) $(CORE_OBJ) $(PASS_OBJ)

all: $(EXE)

$(EXE):	$(FULL_OBJ)
	g++ -o $(EXE) $(FULL_OBJ) $(OPT) $(LIBS) -Wl,-E

$(CORE_OBJ):obj/%.o: Core/Src/%.cpp Core/Include/%.h 
	g++ -c $< -o $@ $(OPT)$(LIBS)

$(PASS_OBJ):obj/%.o: Passes/Src/%.cpp Passes/Include/%.h 
	g++ -c $< -o $@ $(OPT) $(LIBS)

$(MAIN_OBJ):obj/%.o: %.cpp %.h
	g++ -c $< -o $@ $(OPT) $(LIBS)

clean:
	rm $(OBJ) $(EXE) obj/*o tmp Log.txt doxygen_warn `find . -name "*~"` `find . -name "*so"` regression/*.txt output/* *out 2> /dev/null || echo "Done cleaning"

count:
	wc `find . -name "*\.cpp"|grep -v utils` `find . -name "*\.h"|grep -v utils` 

countfiles:
	@echo "CPP: " `find . -name "*cpp" |wc -l`; \
    echo "Headers:" `find . -name "*h" | wc -l`;\
    echo "Txt:" `find data -name "*txt" |wc -l`
    
