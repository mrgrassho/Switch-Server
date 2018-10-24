vpath %.c src

LINK_TARGET = switch_server

OBJS =  \
	abm.o \
	tree.o \
	main.o

REBUILDABLES = $(OBJS) $(LINK_TARGET)

all :
	@echo '[+] Creating Directories...'
	@mkdir build

clean :
	@rm -f $(REBUILDABLES)
	@rm -rf build
	@rm -rf data

$(OBJS) : all

install : $(OBJS)
	@gcc -o $(LINK_TARGET) $(OBJS)

$(LINK_TARGET) : install
	@mv *.o build/
	@echo '[+] Compilado Correctamente.'

%.o : %.c
	@gcc -o $@ -c $<

main.o : tree.c abm.c main.c
abm.o : abm.c tree.c
tree.o : tree.c
