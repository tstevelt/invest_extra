/*----------------------------------------------------------------------------
	Program : getnews.c
	Author  : Tom Stevelt
	Date    : Jul 2021
	Synopsis: Get news from IEX for stocks in portfolios and watchlists.
	Return  : 

	Who		Date		Modification
	---------------------------------------------------------------------

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

#define		MAIN
#include	"getnews.h"

typedef struct
{
	char	Ticker[20];
	long	DateTime;
} RECORD;

#define		MAXSTOCKS		1000
static	RECORD	Array[MAXSTOCKS];
static	int		Count;

static int LoadArray ()
{
	int		ndx;

	for ( ndx = 0; ndx < Count; ndx++ )
	{
		if ( nsStrcmp ( Array[ndx].Ticker, xstock.xsticker ) == 0 )
		{
			return ( 0 );
		}
	}
	if ( Count >= MAXSTOCKS )
	{
		fprintf ( stderr, "Exceeds MAXSTOCKS\n" );
		exit ( 1 );
	}

	sprintf ( Array[Count].Ticker, "%s", xstock.xsticker );
	Array[Count].DateTime = xstock.xsnews;
	Count++;

	return ( 0 );
}

int main ( int argc, char *argv[] )
{
	long	CutoffTime;
	int		ndx;

	getargs ( argc, argv );

	GetInvestCfg ( 0 );

	StartMySQL ( &MySql, "invest" );

	StockCount = RetrieveCount = UpdateCount = ErrorCount = 0;
	time ( &CutoffTime );
	if ( Debug )
	{
		printf ( "Now %ld,", CutoffTime );
	}
	CutoffTime -= (Hours * 60 * 60);
	if ( Debug )
	{
		printf ( " Cutoff %ld\n", CutoffTime );
	}
	Count = 0;
	switch ( RunMode )
	{
		case MODE_ACTIVE:
			sprintf ( WhereClause, "Stype != 'B' and Stype != 'C' and Snews < %ld and (select count(*) from portfolio where Pticker = Sticker) > 0", CutoffTime );
			LoadStockCB ( &MySql, WhereClause, "Sticker", &xstock, (int(*)())LoadArray, 0 );
			sprintf ( WhereClause, "Snews < %ld and (select count(*) from watchlist where Wticker = Sticker) > 0", CutoffTime );
			LoadStockCB ( &MySql, WhereClause, "Sticker", &xstock, (int(*)())LoadArray, 0 );
			for ( ndx = 0; ndx < Count; ndx++ )
			{
				EachStock ( Array[ndx].Ticker, Array[ndx].DateTime );
			}
			break;

		case MODE_ONE:
			sprintf ( WhereClause, "Sticker = '%s' and Snews < %ld", OneTicker, CutoffTime );
			LoadStockCB ( &MySql, WhereClause, "Sticker", &xstock, (int(*)())LoadArray, 1 );
			for ( ndx = 0; ndx < Count; ndx++ )
			{
				EachStock ( Array[ndx].Ticker, Array[ndx].DateTime );
			}
			break;

		default:
			ErrorCount++;
			break;
	
	}
	printf ( "getnews: StockCount %d, RetrieveCount %d, UpdateCount %d, ErrorCount %d\n",
				StockCount, RetrieveCount, UpdateCount, ErrorCount );

	return ( 0 );
}
