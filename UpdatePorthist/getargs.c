/*----------------------------------------------------------------------------
	Program : UpdatePorthist/getargs.c
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

#include    "UpdatePorthist.h"

static int Usage ()
{
	printf ( "USAGE: UpdatePorthist -member # [-date yyyy-mm-dd] [-d]\n" );
	printf ( "USAGE: UpdatePorthist -all      [-date yyyy-mm-dd] [-d]\n" );
	printf ( " -d  debug, no update db\n" );
	exit ( 1 );
}

void getargs ( int argc, char *argv[] )
{
	int		xa;

	if ( argc < 2 )
	{
		Usage ();
	}

	Debug = 0;
	RunMode = 0;
	ascHistDate[0] = '\0';

	for ( xa = 1; xa < argc; xa++ )
	{
		if ( nsStrcmp ( argv[xa], "-all" ) == 0 )
		{
			RunMode = MODE_ALL;
		}
		else if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-member" ) == 0 )
		{
			RunMode = MODE_ONE;
			xa++;
			xmember.xid = nsAtol ( argv[xa] );
		}
		else if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-date" ) == 0 )
		{
			xa++;
			sprintf ( ascHistDate, "%s", argv[xa] );
		}
		else if ( nsStrcmp ( argv[xa], "-d"  ) == 0 )
		{
			Debug = 1;
		}
		else
		{
			Usage ();
		}
	}

	switch ( RunMode )
	{
		case MODE_ALL:
		case MODE_ONE:
			break;

		default:
			Usage ();
			break;
	}
}
