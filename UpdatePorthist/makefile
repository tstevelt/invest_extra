
PROG=UpdatePorthist

ifeq ($(LDFLAGS),"-m32")
LIB = UpdatePorthist.a
XLIB = /usr/local/lib/invlib.a /usr/local/lib/dbylib.a /usr/local/lib/shslib.a -luuid -lcrypto -lm
else
LIB = UpdatePorthist_64.a
XLIB = /usr/local/lib64/invlib.a /usr/local/lib64/dbylib.a /usr/local/lib64/shslib.a /usr/lib64/libmysqlclient.so  -lz -lssl -luuid -lcrypto -lm
endif

DEF = /usr/local/include

PRG = /usr/local/bin/$(PROG)

FILES = \
	$(LIB)(UpdatePorthist.o)	\
	$(LIB)(getargs.o)			\
	$(LIB)(Init.o)				\
	$(LIB)(EachMember.o)

.SILENT:

$(PRG): $(LIB) $(XLIB)
	echo "using gcc to load $(PRG)"
	gcc $(LDFLAGS) -o $(PRG) $(LIB) $(XLIB)

$(LIB): $(FILES)

$(FILES): UpdatePorthist.h $(DEF)/shslib.h $(DEF)/invlib.h $(DEF)/dbylib.h $(DEF)/fileinvest.h

clean:
	rm -f $(LIB)

all:
	make clean
	make

.PRECIOUS: $(LIB)
