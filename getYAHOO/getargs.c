/*----------------------------------------------------------------------------
	Program : getYAHOO/getargs.c
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

#include	"getYAHOO.h"

static void Usage ()
{
	printf ( "USAGE: getYAHOO file [-raw] [-d]\n" );
	printf ( "-raw = do not adjust historical prices\n" );
	printf ( "-d   = debug\n" );
	exit ( 1 );
}

void getargs ( int argc, char *argv[] )
{
	int		xa;

	if ( argc < 2 )
	{
		Usage ();
	}

	AdjustHistory = 1;

	Filename = argv[1];

	for ( xa = 2; xa < argc; xa++ )
	{
		if ( nsStrcmp ( argv[xa], "-d" ) == 0 )
		{
			Debug = 1;
		}
		else if ( nsStrcmp ( argv[xa], "-raw" ) == 0 )
		{
			AdjustHistory = 0;
		}
		else
		{
			printf ( "Unknown arg\n" );
			exit ( 1 );
		}
	}
}
