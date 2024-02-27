/*----------------------------------------------------------------------------
	Program : getsplits/getargs.c
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

#include	"getsplits.h"

static void Usage ()
{
	printf ( "USAGE: getsplits -all           days [-d#]\n" );
	printf ( "USAGE: getsplits -active        days [-d#]\n" );
	printf ( "USAGE: getsplits -ticker TICKER days [-d#]\n" );
	printf ( " -all    = all stocks\n" );
	printf ( " -active = stocks in portfolios\n" );
	printf ( " -ticker = one stock\n" );
	printf ( " days    = number of days ago to compare 10 day averages\n" );
	printf ( "-d1  = debug\n" );
	printf ( "-d2  = debug, more verbose\n" );
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
	else if ( nsStrcmp ( argv[1], "-all" ) == 0 )
	{
		RunMode = MODE_ALL;
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

	if (( Days = nsAtoi ( argv[xa] )) == 0 )
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
		if ( nsStrcmp ( argv[xa], "-d1" ) == 0 )
		{
			Debug = 1;
		}
		else if ( nsStrcmp ( argv[xa], "-d2" ) == 0 )
		{
			Debug = 2;
		}
		else
		{
			printf ( "Unknown arg\n" );
			exit ( 1 );
		}
	}
}
