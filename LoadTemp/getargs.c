/*----------------------------------------------------------------------------
	Program : LoadTemp/getargs.c
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

#include	"LoadTemp.h"

static void Usage ()
{
	printf ( "USAGE:LoadTemp -member ID -sector code\n" );
	printf ( "      LoadTemp -member ID -industry code\n" );
	exit ( 1 );
}

void getargs ( int argc, char *argv[] )
{
	int		xa;

	if ( argc < 5 )
	{
		Usage ();
	}

	MemberID = 0;
	WhereClause[0] = '\0';

	for ( xa = 1; xa < argc; xa++ )
	{
		if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-member" ) == 0 )
		{
			xa++;
			MemberID = nsAtol ( argv[xa] );
		}
		else if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-sector" ) == 0 )
		{
			xa++;
			sprintf ( WhereClause, "Ssector = '%s'", argv[xa] );
		}
		else if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-industry" ) == 0 )
		{
			xa++;
			sprintf ( WhereClause, "Sindustry = %d", nsAtoi (argv[xa] ) );
		}
		else
		{
			Usage ();
		}
	}

	if ( MemberID == 0 || WhereClause[0] == '\0' )
	{
		Usage ();
	}
}
