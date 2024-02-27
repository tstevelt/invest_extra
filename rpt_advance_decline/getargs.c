/*----------------------------------------------------------------------------
	Program : rpt_advance_decline/getargs.c
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

#include	"rpt_advance_decline.h"

static void Usage ()
{
	printf ( "USAGE: rpt_advance_decline index timeframe\n" );
	SindexUsage ( 0 );
	printf ( "timeframe: -1mo -6mo -1yr\n" );
	exit ( 1 );
}

void getargs ( int argc, char *argv[] )
{
	int		xa;

	if ( argc < 3 )
	{
		Usage ();
	}

	DateRange = 0;
	StockIndex = '?';
	ReportMember = 0;

	StockIndex = toupper ( argv[1][0] );
	ValidStockIndex ( StockIndex, 0, (int(*)()) Usage );

	for ( xa = 2; xa < argc; xa++ )
	{
		if ( nsStrcmp ( argv[xa], "-1mo" ) == 0 )
		{
			DateRange = DATE_RANGE_ONE_MONTH;
		}
		else if ( nsStrcmp ( argv[xa], "-6mo" ) == 0 )
		{
			DateRange = DATE_RANGE_SIX_MONTHS;
		}
		else if ( nsStrcmp ( argv[xa], "-1yr" ) == 0 )
		{
			DateRange = DATE_RANGE_ONE_YEAR;
		}
		else if ( nsStrcmp ( argv[xa], "-d" ) == 0 )
		{
			Debug = 1;
		}
		else
		{
			Usage ();
		}
	}
	if ( DateRange == 0 )
	{
		Usage ();
	}
}
