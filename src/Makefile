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
	yacc -d -v digimanity.y
	flex digimanity.l
	$(CC) $(CFLAGS) -c -o digimanity y.tab.c

clean:
	rm -rf $(ODIR)/*.o
	rm -rf ${PROJ}
	rm -rf $(CDIR)/lex.*.c
	rm -rf $(CDIR)/y.*.c
	rm -rf $(IDIR)/y.*.h
	rm -rf lex.*.c
	rm -rf y.*.c
	rm -rf y.*.h
