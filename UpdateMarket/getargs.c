/*----------------------------------------------------------------------------
	Program : UpdateMarket/getargs.c
	Author  : Tom Stevelt
	Date    : 2019 - 2024
	Synopsis: Get command line args
	Return  : 
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

#include	"UpdateMarket.h"

static void Usage ()
{
	printf ( "USAGE: UpdateMarket -market enddate [-d]\n" );
	printf ( "USAGE: UpdateMarket {S|N}   weeks\n" );
	printf ( "USAGE: UpdateMarket -TICKER weeks\n" );
	printf ( " Only -market updates the market table\n" );
	exit ( 1 );
}

void getargs ( int argc, char *argv[] )
{
	int		xa;
	DATEVAL	TestDate;

	if ( argc < 3 )
	{
		Usage ();
	}

	CalcBothIndex = 0;
	Weeks = 0;
	Debug = 0;

	for ( xa = 1; xa < argc; xa++ )
	{
		if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-market" ) == 0 )
		{
			CalcBothIndex = 1;
			Weeks = 6;

			xa++;
			snprintf ( EndDate, sizeof(EndDate), "%s", argv[xa] );

			if ( StrToDatevalFmt ( EndDate, DATEFMT_YYYY_MM_DD, &TestDate ) != 0 )
			{
				printf ( "Bad date %s\n", EndDate );
				exit ( 1 );
			}
//			return;
		}
		else if ( argv[xa][0] == 'S' )
		{
			StockIndex = toupper ( argv[xa][0] );
			sprintf ( WhereClause, "Slast is not NULL and Ssp500 = 'Y'" );
		}
		else if ( argv[xa][0] == 'N' )
		{
			StockIndex = toupper ( argv[xa][0] );
			sprintf ( WhereClause, "Slast is not NULL and Snasdaq = 'Y'" );
		}
		else if ( nsStrcmp ( argv[xa], "-d" ) == 0 )
		{
			Debug = 1;
		}
		else if ( argv[xa][0] == '-' )
		{
			sprintf ( WhereClause, "Sticker = '%s'", &argv[xa][1] );
		}
		else if (( Weeks = nsAtoi ( argv[xa] )) == 0 )
		{
			Usage ();
		}
	}

	if ( Weeks > MAXWEEKS )
	{
		printf ( "Weeks limited to %d weeks\n", MAXWEEKS );
		Usage ();
	}
}
