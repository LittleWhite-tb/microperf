NAME=PluginChange
EXE=$(NAME).so

OPT_INCLUDE = -I../../../Core/Include
OPT = -fPIC -shared $(OPT_INCLUDE) -Wall -Wextra -O3

all: $(EXE)

$(EXE): $(NAME).cpp
	g++ $(NAME).cpp -o $(EXE) $(OPT)

clean:
	rm *.so

