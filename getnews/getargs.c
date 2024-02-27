/*----------------------------------------------------------------------------
	Program : getnews/getargs.c
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

#include	"getnews.h"

static void Usage ()
{
	printf ( "USAGE: getnews -active        hours [-d#]\n" );
	printf ( "USAGE: getnews -ticker TICKER hours [-d#]\n" );
	printf ( " -active = stocks in portfolios and watchlists\n" );
	printf ( " -ticker = one stock\n" );
	printf ( " hours   = number of hours since last retrieved\n" );
	printf ( "-d  = debug\n" );
	exit ( 1 );
}

void getargs ( int argc, char *argv[] )
{
	int		xa;

	if ( argc < 2 )
	{
		Usage ();
	}

	xa = 2;
	if ( nsStrcmp ( argv[1], "-active" ) == 0 )
	{
		RunMode = MODE_ACTIVE;
		xa = 2;
	}
	else if ( argc >= 3 && nsStrcmp ( argv[1], "-ticker" ) == 0 )
	{
		RunMode = MODE_ONE;
		sprintf ( OneTicker, "%s", argv[2] );
		xa = 3;
	}
	else
	{
		Usage ();
	}

	if (( Hours = nsAtoi ( argv[xa] )) == 0 )
	{
		Usage ();
	}
	else
	{
		xa++;
	}

	Debug = 0;

	for ( ; xa < argc; xa++ )
	{
		if ( nsStrcmp ( argv[xa], "-d" ) == 0 )
		{
			Debug = 1;
		}
		else
		{
			printf ( "Unknown arg\n" );
			exit ( 1 );
		}
	}
}
