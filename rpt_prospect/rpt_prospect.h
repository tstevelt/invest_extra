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
#include	"rptlib.h"

#define		MEMBER
#define		STOCK
#define		HISTORY
#define		AVERAGE
#define		PORTFOLIO
#define		FUNDAMENTAL
#include	"fileinvest.h"
#include	"invlib.h"

#define		MINGROW5		3
#define		MINGROW1		3
#define		MINRATIO		4
#define		MAXRATIO		40
#define		MINYIELD		3

TYPE	int		Debug;
TYPE	int		ReportCount;
TYPE	char	OutFileName[256];
TYPE	FILE	*fpOutput;
TYPE	long	ReportMember;
TYPE	int		SortBy;
TYPE	char	StockIndex;

typedef struct
{
	char	Ticker[21];
	double	Grow5;
	long	EntVal;
	double	Ratio;
	int		Flag;
} PORT_RECORD;

#define			MAXPORT		500
TYPE	PORT_RECORD		PortArray[MAXPORT];
TYPE	int				PortCount;
TYPE	long		MinEntValue;
TYPE	long		MaxEntValue;

#define		MODE_REPORT		'R'
TYPE	int		RunMode;
TYPE	char	OneTicker[22];

TYPE	REPORT_OPTIONS	ReportOptions;

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

/* dorpt_base.c */
void dorpt_base ( char *DataFilename );

/* EachPortfolio.c */
long CalcEntValueAndRatio ( long *EntValue , double *Ratio );
int EachPortfolio ( void );

/* EachStock.c */
int EachStock ( void );

/* getargs.c */
void getargs ( int argc , char *argv []);

/* rpt_prospect.c */
int cmpticker ( PORT_RECORD *a , PORT_RECORD *b );
int main ( int argc , char *argv []);

/* sortdata.c */
void sortdata ( char *OutFileName );
