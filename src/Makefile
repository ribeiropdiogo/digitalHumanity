PROJ=digimanity
IDIR=../include
ODIR=obj
CDIR=backend

CC=gcc
CFLAGS = -g -Ofast `pkg-config --cflags glib-2.0` -I$(IDIR)

DEPS=$(IDIR)/$(wildcard *.h)
SOURCES=$(wildcard $(CDIR)/*.c)
SOURCES_OBJ=$(patsubst $(CDIR)/%.c,$(ODIR)/%.o,$(SOURCES))

LIBS=`pkg-config --libs glib-2.0`

print-% : ; @echo $* = $($*)

$(ODIR)/%.o : $(CDIR)/%.c $(DEPS)
		$(CC) $(CFLAGS) -c -o $@ $<

install:
	yacc -d digimanity.y
	flex digimanity.l
	gcc -o digimanity y.tab.c
	cp $(PROJ) /usr/local/bin/
	make clean

clean:
	rm -rf $(ODIR)/*.o
	rm -rf ${PROJ}
	rm -rf $(CDIR)/lex.*.c
	rm -rf $(CDIR)/y.*.c
	rm -rf $(IDIR)/y.*.h
	rm -rf lex.*.c
	rm -rf y.*.c
	rm -rf y.*.h
