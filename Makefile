SOURCE = $(wildcard *.c) $(wildcard renderers/*.c) $(wildcard colors/*.c) $(wildcard caches/*.c)
HEADERS = $(wildcard includes/*.h) $(wildcard includes/renderers/*.h) $(wildcard includes/colors/*.h) $(wildcard includes/caches/*.h)
OBJECTS = $(join $(addsuffix obj/, $(dir $(SOURCE))), $(notdir $(SOURCE:.c=.o)))

CC        = gcc
CCFLAGS   = -O -Iincludes -g 
LIBRARIES = -lz -lm -lpng
DOXYGEN   = doxygen
DOXYFILE  = Doxyfile
EXEC_NAME = minemap
CTAGS     = ctags -R

all: minemap 

minemap: $(OBJECTS)
	$(CC) $(LIBRARIES) -o $@ $(OBJECTS) 

obj/%.o: %.c
	$(CC) $(CCFLAGS) $(LIBRARIES) -c -o $@ $<

renderers/obj/%.o: renderers/%.c
	$(CC) $(CCFLAGS) $(LIBRARIES) -c -o $@ $<

colors/obj/%.o: colors/%.c
	$(CC) $(CCFLAGS) $(LIBRARIES) -c -o $@ $<

caches/obj/%.o: caches/%.c
	$(CC) $(CCFLAGS) $(LIBRARIES) -c -o $@ $<

clean: 
	rm -f $(OBJECTS) minemap

html: $(HEADERS) 
	$(DOXYGEN) $(DOXYFILE)

tags: $(SOURCE) $(HEADERS) 
	$(CTAGS)
