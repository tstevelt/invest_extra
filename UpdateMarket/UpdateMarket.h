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

#include	"shslib.h"

#include	"dbylib.h"

#define		STOCK
#define		HISTORY
#define		MARKET
#include	"fileinvest.h"
#include	"invlib.h"

TYPE	char	StockIndex;
TYPE	int		CalcBothIndex;
TYPE	int		Weeks;
TYPE	char	EndDate[12];
TYPE	int		InsertCount, UpdateCount;
TYPE	int		Debug;

#define		MAXWEEKS		10
#define		MAXRECS		(MAXWEEKS*5)

typedef struct 
{
	char	Date[12];
	double	SumPrice;
	double	SumVolume;
	int		Count;
	double	AvgPrice;
	double	AvgVolume;
} RECORD;

TYPE	RECORD	Array[MAXRECS];
TYPE	int		Count;

/*----------------------------------------------------------
	mysql stuff
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


/* UpdateMarket.c */
int main ( int argc , char *argv []);

/* cmprec.c */
int cmprec ( RECORD *a , RECORD *b );

/* CountDays.c */
void CountDays ( int *Acount , int *Dcount , int PrintReport );

/* EachHistory.c */
int EachHistory ( void );

/* EachStock.c */
int EachStock ( void );

/* getargs.c */
void getargs ( int argc , char *argv []);

/* SaveMarket.c */
void SaveMarket ( char *LatestDate , char StockIndex , int Acount , int Dcount );
