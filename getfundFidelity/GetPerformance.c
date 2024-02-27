/*----------------------------------------------------------------------------
	Program : getfundFidelity/GetPerformance.c
	Author  : Tom Stevelt
	Date    : 2019 - 2024
	Synopsis: Get average 3 and 5 year returns from Fidelity Performance file.
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
Performance.csv

==> Performance.csv <==
Symbol,Market Total Returns (3 Yr),Market Total Returns (5 Yr)
Symbol,Market Total Returns (3 Yr),Market Total Returns (5 Yr)
AIA,-8.42275,1.65144
DGT,13.14892,8.69306
EEM,-4.44003,0.91376
EFA,5.95587,4.36058
EPP,2.41513,2.37528
EWA,6.00536,4.63279
EWC,8.50848,6.02216
EWD,0.82057,4.34077
EWG,1.78581,1.0913
----------------------------------------------------------*/
void GetPerformance ()
{
	RECORD	Key, *ptr;

	if (( fp = fopen ( "Performance.csv", "r" )) == NULL )
	{
		printf ( "Cannot open Performance.csv for reading\n" );
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
		if (( ptr = bsearch ( &Key, Array, Count, sizeof(RECORD), (int(*)()) cmprec )) == NULL )
		{
			if ( Verbose )
			{
				printf ( "GetPerformance: Cannot find %s in Array\n", Key.Ticker );
			}
			continue;
		}

		ptr->AvgRet3 = nsAtof(tokens[1]);
		ptr->AvgRet5 = nsAtof(tokens[2]);
	}

	fclose ( fp );
}
