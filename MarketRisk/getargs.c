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

#include	"MarketRisk.h"

static void Usage ()
{
	printf ( "USAGE: MarketRisk -risk ETF {-all|-current}\n" );
	printf ( "USAGE: MarketRisk -alloc\n" );
	printf ( "USAGE: MarketRisk -er CAPE TIPS\n" );
	printf ( "Suggested ETF for risk assessment\n" );
	printf ( "  ITOT total US market\n" );
	printf ( "  IDEV International Developed\n" );
	printf ( "  EMXC Emerging Market EX-China\n" );
	printf ( "Get current 10 TIPS from:\n" );
	printf ( "  https://tipswatch.com/2025/01/23/new-10-year-tips-gets-real-yield-of-2-243-highest-for-this-term-in-16-years/\n" );
	exit ( 1 );
}


void getargs ( int argc, char *argv[] )
{
	RunMode = 0;

	if ( argc < 2 )
	{
		Usage ();
	}

	if ( argc >= 4 && strcmp ( argv[1], "-risk" ) == 0 )
	{
		RunMode = MODE_RISK;
		sprintf ( WhereClause, "Sticker = '%s'", argv[2] );
		if ( LoadStock ( &MySql, WhereClause, &xstock, 0, 0 ) != 1 )
		{
			printf ( "Unknown ETF\n" );
			exit ( 1 );
		}

		if ( strcmp ( argv[3], "-all" ) == 0 )
		{
			Method = 1;
			Days = 1000;
		}
		else if ( strcmp ( argv[3], "-current" ) == 0 )
		{
			Method = 2;
			Days = 260;
		}
		else
		{
			Usage ();
		}
	}
	else if ( strcmp ( argv[1], "-alloc" ) == 0 )
	{
		RunMode = MODE_ALLOC;
	}
	else if ( argc >= 3 && strcmp ( argv[1], "-er" ) == 0 )
	{
		RunMode = MODE_ER;
		CAPE = atof ( argv[2] );
		TIPS = atof ( argv[3] );
	}
	else
	{
		Usage ();
	}
}
