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

install: regex
	make backend

regex:
	yacc -d -v ${PROJ}.y
	flex ${PROJ}.l
	mv y.*.c $(CDIR) 2> /dev/null
	mv y.*.h $(IDIR) 2> /dev/null

backend: $(SOURCES_OBJ)
	$(CC) $(CFLAGS) $(wildcard $(ODIR)/*.o) -o ${PROJ} $(LIBS)

clean:
	rm -rf $(ODIR)/*.o
	rm -rf ${PROJ}
	rm -rf $(CDIR)/lex.*.c
	rm -rf $(CDIR)/y.*.c
	rm -rf $(IDIR)/y.*.h
	rm -rf $(CDIR)/lex.*.c
	rm -rf $(CDIR)/y.*.c
	rm -rf $(CDIR)/y.*.h
