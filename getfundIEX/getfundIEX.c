/*---------------------------------------------------------------------------
	Program : getfundIEX.c
	Author  : Tom Stevelt
	Date    : 11/27/2023
	Synopsis: Get fundamentals (stats) from IEX, to fix some crap from SEC
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
#include	"getfundIEX.h"

extern double TotalNapTime;
int TotalOld = 0;

int main ( int argc, char *argv[] )
{
	FILE	*fp;
	char	xbuffer[102400];
#define		MAXTOKS		50000
	char	*tokens[MAXTOKS];

// printf ( "sizeof long %ld\n", sizeof(long) );

	time ( &StartTime );

	getargs ( argc, argv );

	if (( curl = curl_easy_init () ) == NULL )
	{
		fprintf ( stderr, "curl init failed\n" );
		exit ( 1 );
	}

	GetInvestCfg ( 0 );

	StartMySQL ( &MySql, "invest" );

	dbyCommitInit ( &MySql );

	StockCount = ErrorCount = AgeCount = UpdateCount = InsertCount = 0;

	if ( StockIndex == 'F' )
	{
		if (( fp = fopen ( InputFileName, "r" )) == (FILE *)0 )
		{
			printf ( "Cannot open %s for input\n", InputFileName );
			exit ( 1 );
		}

		while ( fgets ( xbuffer, sizeof(xbuffer), fp ) != (char *)0 )
		{
			if ( GetTokensA ( xbuffer, " \t\r\n", tokens, 2 ) < 1 )
			{
				continue;
			}
			sprintf ( WhereClause, "Sticker = '%s'", tokens[0] );
			LoadStockCB ( &MySql, WhereClause, "Sticker", &xstock, (int(*)()) EachStock, 1 );
		}

		nsFclose ( fp );
	}
	else if ( StockIndex == 'E' )
	{
		sprintf ( WhereClause, "Stype = 'E'" );
		LoadStockCB ( &MySql, WhereClause, "Sticker", &xstock, (int(*)()) EachStock, 0 );
	}
	else if ( StockIndex == 'x' )
	{
		sprintf ( WhereClause, "Sticker = '%s'", Ticker );
		LoadStockCB ( &MySql, WhereClause, "Sticker", &xstock, (int(*)()) EachStock, 0 );
	}
	else
	{
		SetWhereClause ( WhereClause, StockIndex );
		LoadStockCB ( &MySql, WhereClause, "Sticker", &xstock, (int(*)()) EachStock, 0 );
	}

	time ( &EndTime );

	dbyCommitClose ( &MySql );

	curl_easy_cleanup ( curl );

	printf ( "getfundIEX: StockCount %d, NoChangeCount %d, UpdateCount %d, ErrorCount %d\n", 
					StockCount, NoChangeCount, UpdateCount, ErrorCount );

	printf ( "getfundIEX: Total Time %ld, Nap Time %.2f\n", EndTime - StartTime, (double) TotalNapTime / 1000.0 );

	return ( 0 );
}
