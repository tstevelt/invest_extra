/*---------------------------------------------------------------------------
    Market Risk

    Copyright (C)  2025 Tom Stevelt

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
---------------------------------------------------------------------------*/

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

