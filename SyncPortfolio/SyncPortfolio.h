//     Invest extras
// 
//     Copyright (C)  2019 - 2024 Tom Stevelt
// 
//     This program is free software: you can redistribute it and/or modify
//     it under the terms of the GNU Affero General Public License as
//     published by the Free Software Foundation, either version 3 of the
//     License, or (at your option) any later version.
// 
//     This program is distributed in the hope that it will be useful,
//     but WITHOUT ANY WARRANTY; without even the implied warranty of
//     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//     GNU Affero General Public License for more details.
// 
//     You should have received a copy of the GNU Affero General Public License
//     along with this program.  If not, see <https://www.gnu.org/licenses/>.

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

#include <errno.h>
#include <fcntl.h>

#include	"shslib.h"

#include	"dbylib.h"

#define		STOCK
#define		HISTORY
#define		MEMBER
#define		PORTFOLIO
#include	"fileinvest.h"
#include	"invlib.h"


TYPE	char	*BrokerID;
TYPE	char	*Filename;
TYPE	int		Debug;

typedef struct
{
	char	Ticker[22];
	int		OnFile;
} RECORD;

#define		MAXPORTRECS		500
TYPE	RECORD		Array[MAXPORTRECS];
TYPE	int			Count;

#define		MAXTOKS		20
TYPE	char	xbuffer[1024];
TYPE	FILE	*fp;
TYPE	char	*tokens[MAXTOKS];
TYPE	int		tokcnt;
TYPE	int		ndx, rv, Errors, NotInDB, NoChange, TotalSold, TotalBought, lineno;
TYPE	char	Today[12];
TYPE	XPORTFOLIO	FilePortfolio;
TYPE	char		ans;
TYPE	char	strPurchaseDate[20];
TYPE	char	strBroker[sizeof(xportfolio.xpbroker)+2];
TYPE	DATEVAL	dvPurchaseDate;
TYPE	char	strMaturityDate[20];
TYPE	DATEVAL	dvMaturityDate;
TYPE	double	BondValueFile;
TYPE	double	BondValuePortfolio;

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

/* EachPortfolio.c */
int EachPortfolio ( void );
int cmprec ( RECORD *a , RECORD *b );

/* getargs.c */
void getargs ( int argc , char *argv []);

/* ProcessFidelity.c */
void ProcessFidelity ( void );

/* ProcessIBKR.c */
void ProcessIBKR ( void );

/* ProcessSchwab.c */
void ProcessSchwab ( void );

/* SyncPortfolio.c */
char GetValidChar ( char *ValidChars );
//fixit char GetAnswer ( char *Question , *ValidChars );
int main ( int argc , char *argv []);
