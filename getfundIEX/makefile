PROG = getfundIEX
LIB = getfundIEX.a

XLIB = /usr/local/lib64/invlib.a /usr/local/lib64/dbylib.a /usr/local/lib64/shslib.a  /usr/lib64/libmysqlclient.so  -lcurl -lz -lssl -luuid -lcrypto -lm

DEF = /usr/local/include
PRG = /usr/local/bin/$(PROG)

FILES = \
	$(LIB)(getfundIEX.o)	\
	$(LIB)(getargs.o)		\
	$(LIB)(EachStock.o)

.SILENT:

$(PRG): $(LIB) $(XLIB)
	echo "using gcc to load $(PRG)"
	gcc -o $(PRG) $(LIB) $(XLIB) $(MLIB)

$(LIB): $(FILES)

$(FILES): getfundIEX.h $(DEF)/shslib.h $(DEF)/dbylib.h $(DEF)/fileinvest.h $(DEF)/invlib.h

clean:
	rm -f $(LIB)

all:
	make clean
	make

.PRECIOUS: $(LIB)

