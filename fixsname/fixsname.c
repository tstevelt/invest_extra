/*----------------------------------------------------------------------------
	Program : fixsname.c
	Author  : Tom Stevelt
	Date    : 02/11/2019
	Synopsis: Update stock names from file with good names.
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
#include	"fixsname.h"

int main ( int argc, char *argv[] )
{
	char	xbuffer[256];
	char	*tokens[3];
	int		tokcnt, lineno, xi, xl;

	getargs ( argc, argv );

	if (( fpInput = fopen ( InputFilename, "r" )) == (FILE *)0 )
	{
		printf ( "Cannot open %s for input\n", InputFilename );
		exit ( 1 );
	}

	StartMySQL ( &MySql, "invest" );

	Errors = NotInDB = NoChange = TotalUpdated = lineno = 0;

	while ( fgets ( xbuffer, sizeof(xbuffer), fpInput ) != (char *)0 )
	{
		lineno++;

		if (( tokcnt = GetTokensA ( xbuffer, ",\n\r", tokens, 3 )) < 2 )
		{
			printf ( "bad line %d\n", lineno );
			continue;
		}

		sprintf ( WhereClause, "Sticker = '%s'", tokens[0] );
		if ( LoadStock ( &MySql, WhereClause, &xstock, 0, 0 ) < 1 )
		{
			NotInDB++;
			continue;
		}

		xl = nsStrlen ( tokens[1] );
		for ( xi = 0; xi < xl; xi++ )
		{
			switch ( tokens[1][xi] )
			{
				case '\'':
					tokens[1][xi] = '.';
					break;
				case '%':
					tokens[1][xi] = ' ';
					break;
			}
		}

		if ( nsStrcmp ( xstock.xsname, tokens[1] ) == 0 )
		{
			NoChange++;
			continue;
		}
		
		if ( xl > 78 )
		{
			sprintf ( Statement, "update stock set Sname = '%78.78s' where Sticker = '%s'", tokens[1], xstock.xsticker );
		}
		else
		{
			sprintf ( Statement, "update stock set Sname = '%s' where Sticker = '%s'", tokens[1], xstock.xsticker );
		}
		if ( dbyUpdate ( "fixsname", &MySql, Statement, 0, LogFileName ) < 1 )
		{
			printf ( "update failed on %s.\n", xstock.xsticker );
			printf ( "%s\n", Statement );
			fflush ( stdout );
			Errors++;
		}
		else
		{
			TotalUpdated++;
		}
	}

	printf ( "Read      %d\n", lineno );
	printf ( "No in DB  %d\n", NotInDB );
	printf ( "No change %d\n", NoChange );
	printf ( "Errors    %d\n", Errors );
	printf ( "Updated   %d\n", TotalUpdated );

	return ( 0 );
}
