/*----------------------------------------------------------------------------
	Program : IndexRecon/getargs.c
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


#include	"IndexRecon.h"

static void MyUsage ()
{
	printf ( "USAGE: IndexRecon index testdate filename [-add] [-d]\n" );
	printf ( "index     D S N 1 2\n" );
	printf ( "testdate  yyyy-mm-dd\n" );
	printf ( "-add      add stock if not in database\n" );
	exit ( 1 );
}

void getargs ( int argc, char *argv[] )
{
	int		xa;

	if ( argc < 3 )
	{
		MyUsage();
	}

	IndexType = toupper ( argv[1][0] );
	switch ( IndexType )
	{
		case 'D':
			FieldName = "Sdj";
			SetValue = 'Y';
			break;
		case 'S':
			FieldName = "Ssp500";
			SetValue = 'Y';
			break;
		case 'N':
			FieldName = "Snasdaq";
			SetValue = 'Y';
			break;
		case '1':
			FieldName = "Srussell";
			SetValue = '1';
			break;
		case '2':
			FieldName = "Srussell";
			SetValue = '2';
			break;
		default:
			MyUsage ();
	}

	sprintf ( TestDate, "%s", argv[2] );

	if (( ifp = fopen ( argv[3], "r" )) == NULL )
	{
		MyUsage ();
	}

	for ( xa = 4; xa < argc; xa++ )
	{
		if ( strcmp ( argv[xa], "-add" ) == 0 )
		{
			AddNewStock = 1;
		}
		else if ( strcmp ( argv[xa], "-d" ) == 0 )
		{
			Debug = 1;
		}
	}

}
