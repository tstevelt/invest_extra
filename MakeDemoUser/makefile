
PROG=MakeDemoUser

LIB = MakeDemoUser.a

XLIB = /usr/local/lib64/invlib.a /usr/local/lib64/dbylib.a \
		/usr/local/lib64/shslib.a  \
		/usr/local/lib/libmysqlclient.a \
		/usr/local/lib/libcrypto.a \
		/usr/local/lib/libssl.a \
		/usr/local/Cellar/zlib/1.3.1/lib/libz.a

DEF = /usr/local/include

PRG = /usr/local/bin/$(PROG)

FILES = \
	$(LIB)(MakeDemoUser.o)

.SILENT:

$(PRG): $(LIB) $(XLIB)
	echo "using gcc to load $(PRG)"
	gcc $(LDFLAGS) -o $(PRG) $(LIB) $(XLIB)

$(LIB): $(FILES)

$(FILES): MakeDemoUser.h $(DEF)/shslib.h $(DEF)/invlib.h  $(DEF)/dbylib.h $(DEF)/fileinvest.h

clean:
	rm -f $(LIB)

all:
	make clean
	make

.PRECIOUS: $(LIB)
