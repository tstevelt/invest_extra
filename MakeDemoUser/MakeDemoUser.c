/*----------------------------------------------------------------------------
	Program : MakeDemoUser.c
	Author  : Tom Stevelt
	Date    : 06/03/2019
	Synopsis: Make Demo Account
	Return  : 

	Who		Date		Modification
	---------------------------------------------------------------------
	tms		06/25/2019	Add cash account transactions.
	tms		07/07/2023	Held for a least two years
	tms		11/15/2023	Added Stock vs ETF option

----------------------------------------------------------------------------*/
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

#define	MAIN
#include	"MakeDemoUser.h"

typedef struct
{
	char	*Table;
	char	*Field;
} DELETE_RECORD;

static	DELETE_RECORD	DeleteArray[] = 
{
	{ "portfolio", "Pmember" },
	{ "porthist",  "PHmember" },
	{ "watchlist", "Wmember" },
	{ "temp",      "Tmember" },
	{ "payments",  "PYmember" },
	{ "rptlog",    "Rmember" },
};

static	int		DeleteCount = sizeof(DeleteArray) / sizeof(DELETE_RECORD);

typedef struct
{
	char	Ticker[21];
	double	Random;
} STOCK_RECORD;

#define		MAXSTOCK	1000

static	STOCK_RECORD		StockArray[MAXSTOCK];
static	int				StockCount;

static int cmprec ( STOCK_RECORD *a, STOCK_RECORD *b )
{
	if ( a->Random < b->Random )
	{
		return ( -1 );
	}
	if ( a->Random > b->Random )
	{
		return ( 1 );
	}
	return ( 0 );
}

static int EachStock ()
{
	if ( StockCount >= MAXSTOCK )
	{
		return ( 1 );
	}

	sprintf ( StockArray[StockCount].Ticker, "%s", xstock.xsticker );
	StockArray[StockCount].Random = rand ( ) / (double) RAND_MAX ;

printf ( "%12.8f %s\n", StockArray[StockCount].Random,  StockArray[StockCount].Ticker );
	StockCount++;
	return ( 0 );
}

int main ( int argc, char *argv[] )
{
	int		rv, ndx, tries, randday;
	int		Debug = 0;
	char	Today[12];
	DATEVAL	dvToday;
	DATEVAL	dvOneYear;
	char	OneYear[12];
	DATEVAL	dvPurchDate;
	char	PurchDate[12];
	int		Inserted;
	char	StockType;

	if ( argc < 2 )
	{
		printf ( "USAGE: MakeDemoUser S|E|P\n" );
		printf ( "Demo user is %d, existing portfolio will be deleted\n", DEMO_MEMBER );
		exit ( 0 );
	}
	
	switch ( StockType = toupper ( argv[1][0] ))
	{
		case 'S':
		case 'E':
			fprintf ( stderr, "IEX dead\n" );
			exit ( 1 );
		case 'P':
			break;
		default:
			exit ( 0 );
	}

	srand ( time ( NULL ) );
	StartMySQL ( &MySql, "invest" );

	sprintf ( Today, "%s", fmtGetTodayDate ( DATEFMT_YYYY_MM_DD ) );
	dvToday.year4 = nsAtoi (  Today );
	dvToday.month = nsAtoi ( &Today[5] );
	dvToday.day   = nsAtoi ( &Today[8] );

	/*----------------------------------------------------------
		make sure demo user exists  -- if not, create demo
	----------------------------------------------------------*/
	sprintf ( WhereClause, "id = %d", DEMO_MEMBER );
	if (( rv = LoadMember ( &MySql, WhereClause, &xmember, 0 )) != 1 )
	{
		printf ( "Demo member not found, will create now, rv %d\n", rv );

		DateAdd ( &dvToday, 365, &dvOneYear );
		sprintf ( OneYear, "%04d-%02d-%02d", dvOneYear.year4, dvOneYear.month, dvOneYear.day );

		sprintf ( Statement, "insert into member \
( id, Mname, Memail, Mpassword, Mstatus, Mrole, Mipaddr, Minsdt, Msuborig, Msubstart, Msubpaid, Msubexpire) \
values ( %d, 'Demo', 'demo@silverhammersoftware.com', '%s',  '%c', '%c', '%s', '%s', '%s', '%s', 1.0, '%s')",
			DEMO_MEMBER,
			pw_sha_make_pw ( (unsigned char *)"demo" ),
			STATUS_VERIFIED,
			ROLE_PAID,
			getenv("REMOTE_ADDR"),
			Today,
			Today,
			Today,
			OneYear );

printf ( "%s\n", Statement );

		if (( rv = dbyInsert ( "invest", &MySql, Statement, 0, LogFileName )) != 1 )
		{
			printf ( "Error inserting demo user, rv %d\n", rv );
			exit ( 1 );
		}
		else
		{
			printf ( "Created Demo User, ID %d\n", DEMO_MEMBER );
		}

	}

	/*---------------------------------------------------------------------------
		delete existing portfolio, watchlist, temp, payment and rptlog records
	---------------------------------------------------------------------------*/
	for ( ndx = 0; ndx < DeleteCount; ndx++ )
	{
		printf ( "deleting %s\n", DeleteArray[ndx].Table );
		sprintf ( Statement, "delete from %s where %s = %d", DeleteArray[ndx].Table, DeleteArray[ndx].Field, DEMO_MEMBER );
		dbyDelete ( "invest", &MySql, Statement, 0, LogFileName );
	}

	/*----------------------------------------------------------
		Load Stocks
	----------------------------------------------------------*/
	switch ( StockType )
	{
		case 'S':
			sprintf ( WhereClause, "Srussell = '1'" );
			break;
		case 'E':
			sprintf ( WhereClause, "Stype = 'E'" );
			break;
		case 'P':
			sprintf ( WhereClause, "Sticker in (select Pticker from portfolio) and Stype in ('S','E')" );
			break;
	}

	LoadStockCB ( &MySql, WhereClause, NULL, &xstock, (int(*)()) EachStock, 0 );

	printf ( "Loaded %d stocks\n", StockCount );

	qsort ( StockArray, StockCount, sizeof(STOCK_RECORD), (int(*)()) cmprec );

	/*----------------------------------------------------------
		make portfolio of random stocks and random purchase
		dates, but with realistic prices.
	----------------------------------------------------------*/
	Inserted = 0;
	for ( ndx = 0; ndx < StockCount * 0.75 && ndx < StockCount; ndx++ )
	{
		for ( tries = 0; tries < 10; tries++ )
		{
#define RANDOM_PURCHASE_DATE
#ifdef RANDOM_PURCHASE_DATE
			randday = rand ();
			randday = 680 + randday % 50;
			// randday = 360 + randday % 50;
			randday = 0 - randday;
#else
			randday = -42;
#endif
			DateAdd ( &dvToday, randday, &dvPurchDate );
			sprintf ( PurchDate, "%04d-%02d-%02d", dvPurchDate.year4, dvPurchDate.month, dvPurchDate.day );

			sprintf ( WhereClause, "Hticker = '%s' and Hclose < %.2f and Hdate = '%s'", 
					StockArray[ndx].Ticker, xstock.xsclose * 0.9, PurchDate );
			// sprintf ( WhereClause, "Hticker = '%s' and Hdate = '%s'", 
			// 		StockArray[ndx].Ticker, PurchDate );

			if (LoadHistory ( &MySql, WhereClause, &xhistory, 0 ) == 1 )
			{
				break;
			}
		}

		if ( tries >= 10 )
		{
			sprintf ( WhereClause,
				"Hticker = '%s' and  Hdate = (select min(Hdate) from history where Hticker = '%s' and Hclose > %.2f)", 
						StockArray[ndx].Ticker, StockArray[ndx].Ticker, xstock.xsclose * 0.9 );

			if ( LoadHistory ( &MySql, WhereClause, &xhistory, 0 ) != 1 )
			{
				if ( Debug )
				{
					printf ( "Could not find history for %s\n", StockArray[ndx].Ticker );
					printf ( "%s\n", WhereClause );
				}
				continue;
			}
		}

		sprintf ( Statement,
			"insert into portfolio ( Pmember, Pticker, Pdate, Pshares, Pprice, Pbroker, Pbenchmark, Palerttype, Pbucket ) \
						values ( '%d', '%s', '%s', 100, %.2f, 'DEMO', 'SPY', '%c', 3 )",
				DEMO_MEMBER,
				StockArray[ndx].Ticker,
				PurchDate,
				xhistory.xhclose,
				ALERT_NONE );

		if (( rv = dbyInsert ( "invest", &MySql, Statement, 0, LogFileName )) != 1 )
		{
			printf ( "Error inserting portfolio, rv %d\n", rv );
			exit ( 1 );
		}
		else
		{
			Inserted++;
		}
	}

	printf ( "Inserted %d portfolio records\n", Inserted );

	/*----------------------------------------------------------
		make watchlist of random stocks
	----------------------------------------------------------*/
	Inserted = 0;
	for ( ;  ndx < StockCount; ndx++ )
	{
		sprintf ( Statement, "insert into watchlist ( Wmember, Wticker, Walerttype ) \
						values ( '%d', '%s', '%c' )",
				DEMO_MEMBER,
				StockArray[ndx].Ticker,
				ALERT_NONE );

		if (( rv = dbyInsert ( "invest", &MySql, Statement, 0, LogFileName )) != 1 )
		{
			printf ( "Error inserting watchlist, rv %d\n", rv );
			exit ( 1 );
		}
		else
		{
			Inserted++;
		}
	}

	printf ( "Inserted %d watchlist records\n", Inserted );

	return ( 0 );
}
