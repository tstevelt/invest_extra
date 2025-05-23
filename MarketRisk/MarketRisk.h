
#include	<stdio.h>
#include	<stdlib.h>
#include	<unistd.h>
#include	<string.h>
#include	<ctype.h>

#include <stdint.h>
#include <assert.h>
#include <time.h>
#include <libgen.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/evp.h>
#include <sys/types.h>

#include	"shslib.h"

#include	"dbylib.h"

#define		STOCK
#define		HISTORY
#define		AVERAGE
#include	"fileinvest.h"
#include	"invlib.h"

TYPE	int		RunMode;
#define	MODE_RISK	11
#define	MODE_ALLOC	12
#define	MODE_ER		13
TYPE	int		Days, Method;
TYPE	double	CAPE, TIPS;

#ifdef MAIN
char	*TickerArray [] = { "ITOT", "IDEV", "EEM", "EMXC", "IAU" };
int		TickerCount = sizeof(TickerArray) / sizeof(char *);
#else
extern	char	*TickerArray [];
extern	int		TickerCount;
#endif

/*----------------------------------------------------------
	mysql and dbylib stuff
----------------------------------------------------------*/
TYPE  MYSQL   MySql;
TYPE	DBY_OPTIONS	DbyOptions;
TYPE	char		Statement[1024];
TYPE	char		WhereClause[1024];
TYPE	char		OrderByClause[1024];
#ifdef MAIN
char	*LogFileName = "/var/local/invest.log";
#else
TYPE	char	*LogFileName;
#endif

/*------------------------------
:.,$d
:r ! mkproto -p *.c
------------------------------*/

void DoAlloc ();
void DoRisk ();
void DoExpected ();
void getargs ( int argc, char *argv[] );

