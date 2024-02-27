/*----------------------------------------------------------------------------
	Program : getfundFidelity/GetIncome.c
	Author  : Tom Stevelt
	Date    : 2019 - 2024
	Synopsis: Get Yield from Fidelity Income .csv file.
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
Income.csv

==> Income.csv <==
Symbol,Distribution Yield (TTM)
Symbol,Distribution Yield (TTM)
AIA,2.73%
DGT,2.64%
EEM,2.43%
EFA,2.37%
EPP,3.96%
EWA,4.09%
EWC,2.64%
EWD,0.63%
EWG,3.04%

----------------------------------------------------------*/
void GetIncome ()
{
	RECORD	Key, *ptr;

	if (( fp = fopen ( "Income.csv", "r" )) == NULL )
	{
		printf ( "Cannot open Income.csv for reading\n" );
		exit ( 1 );
	}

	lineno = 0;
	while ( fgets ( xbuffer, sizeof(xbuffer), fp ) != NULL )
	{
		lineno++;
		if (( tokcnt = GetTokensD ( xbuffer, ",\n\r", tokens, MAXTOKS )) < 2 )
		{
			continue;
		}
		if ( strcmp ( tokens[0], "Symbol" ) == 0 )
		{
			continue;
		}

		snprintf ( Key.Ticker, sizeof(Key.Ticker), "%s", tokens[0] );
		if (( ptr = bsearch ( &Key, Array, Count, sizeof(RECORD), (int(*)()) cmprec )) == NULL )
		{
			if ( Verbose )
			{
				printf ( "GetIncome: Cannot find %s in Array\n", Key.Ticker );
			}
			continue;
		}

		ptr->Yield = nsAtof ( tokens[1] );
	}

	fclose ( fp );
}
