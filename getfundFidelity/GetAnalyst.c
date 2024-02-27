/*----------------------------------------------------------------------------
	Program : getfundFidelity/GetAnalyst.c
	Author  : Tom Stevelt
	Date    : 2019 - 2024
	Synopsis: Get MorningStar and Factset rating from  Fidelity analyst .csv file.
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

#include	"getfundFidelity.h"

/*----------------------------------------------------------
Analyst.csv

==> Analyst.csv <==
Symbol,FactSet,Morningstar (TM) Rating

----------------------------------------------------------*/
void GetAnalyst ()
{
	RECORD	Key, *ptr;

	if (( fp = fopen ( "Analyst.csv", "r" )) == NULL )
	{
		printf ( "Cannot open Analyst.csv for reading\n" );
		exit ( 1 );
	}

	lineno = 0;
	while ( fgets ( xbuffer, sizeof(xbuffer), fp ) != NULL )
	{
		lineno++;
		if (( tokcnt = GetTokensD ( xbuffer, ",\n\r", tokens, MAXTOKS )) < 3 )
		{
			continue;
		}
		if ( strcmp ( tokens[0], "Symbol" ) == 0 )
		{
			continue;
		}

		snprintf ( Key.Ticker, sizeof(Key.Ticker), "%s", tokens[0] );
		Key.Factset     = tokens[1][0];
		Key.Morningstar = tokens[2][0];

		if (( ptr = bsearch ( &Key, Array, Count, sizeof(RECORD), (int(*)()) cmprec )) == NULL )
		{
			/*---------------------------------------------------------------------------
				stock was not in Basic.csv, most likely bad cleanup of junk.
			---------------------------------------------------------------------------*/
			if ( Verbose )
			{
				printf ( "GetAnalyst: Cannot find %s in Array\n", Key.Ticker );
			}
			continue;
		}

		/*---------------------------------------------------------------------------
			stock is not in the database, but it is rated.
		---------------------------------------------------------------------------*/
		if ( Verbose && ptr->HaveStock == 0 )
		{
			if ( strchr( "12345", Key.Morningstar ) != NULL || strchr ( "ABCDEF", Key.Factset ) != NULL )
			{
				printf ( "%s is a rated ETF %c %c\n", Key.Ticker, Key.Factset, Key.Morningstar );
			}
		}

		ptr->Factset     = tokens[1][0];
		ptr->Morningstar = tokens[2][0];
	}

	fclose ( fp );
}
