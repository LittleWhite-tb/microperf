EXE = microlaunch 

MLDIR="\"$(shell pwd)\""
MAIN_OBJ := $(patsubst %.c,obj/%.o,$(wildcard *.c))
CORE_OBJ := $(patsubst Core/Src/%.c,obj/%.o,$(wildcard Core/Src/*.c))
LIBS = -ldl -rdynamic
CC = gcc
OPT = -O3 -Wall -Wextra -g -DX86 `xml2-config --cflags` `xml2-config --libs` -ICore/Include -DMLDIR=$(MLDIR)

TIMER_LIB = Libraries/timer/timer.so
THREADPIN_LIB = Libraries/threadpinner/pinthread.so
WALLCLOCK_LIB = Libraries/wallclock/wallclock.so
FULL_OBJ = $(MAIN_OBJ) $(CORE_OBJ)

ALLOC_DEDICATED_ARRAYS = Libraries/allocator/dedicated_arrays/
ALLOC_GLIBC = Libraries/allocator/glibc_malloc/
EMPTY_OVERHEAD = example/empty/
RESUME_DIR = resumeData/

all: $(EXE) $(TIMER_LIB) $(THREADPIN_LIB) $(CPUTEMP_LIB) $(SNB_ELIB) $(SNB_PLIB) $(WALLCLOCK_LIB) $(ALLOC_DEDICATED_ARRAYS)
	make -C $(ALLOC_DEDICATED_ARRAYS) all
	make -C $(ALLOC_GLIBC) all
	make -C $(EMPTY_OVERHEAD) all

$(EXE):	$(FULL_OBJ)
	$(CC) -o $(EXE) $(FULL_OBJ) $(OPT) $(LIBS)

$(MAIN_OBJ):obj/%.o: %.c %.h
	$(CC) -c $< -o $@ $(OPT) 

$(CORE_OBJ):obj/%.o: Core/Src/%.c Core/Include/%.h
	$(CC) -c $< -o $@ $(OPT) 

$(TIMER_LIB):%.so: %.c %.h
	$(CC) $< -o $@ -fPIC -shared $(OPT) -I.
	
$(THREADPIN_LIB):%.so: %.c
	$(CC) $< -o $@ -fPIC -shared $(OPT) -ldl

$(WALLCLOCK_LIB):%.so: %.c
	$(CC) $< $(OPT) -o $@ -fPIC -shared
	
clean:
	rm -f $(TIMER_LIB) $(WALLCLOCK_LIB) $(FULL_OBJ) $(THREADPIN_LIB) $(EXE) output/*.csv output/*.xls tmp Log.txt summarycreator/csv_files/* `find . -name "*~"` 2> /dev/null $(RESUME_DIR)/*
	make -C $(ALLOC_DEDICATED_ARRAYS) clean
	make -C $(EMPTY_OVERHEAD) clean
	make -C $(ALLOC_GLIBC) clean
	echo "Done cleaning"

count:
	wc `find . -name "*\.c"` `find . -name "*\.h"`

countfiles:
	@echo "CPP: " `find . -name "*c" |wc -l`; \
    echo "Headers:" `find . -name "*h" | wc -l`;\
    echo "Txt:" `find data -name "*txt" |wc -l`
