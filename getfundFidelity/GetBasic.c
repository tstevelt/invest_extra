/*----------------------------------------------------------------------------
	Program : getfundFidelity/GetBasic.c
	Author  : Tom Stevelt
	Date    : 2019 - 2024
	Synopsis: Get from  Assets, Expense Ratio, Inception Date Fidelity baic .csv file.
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
Basic.csv

==> Basic.csv <==
Symbol,Net Assets,Net Expense Ratio,Inception Date
----------------------------------------------------------*/
void GetBasic ()
{
	DATEVAL		dv;
	int			HaveStock;

	if (( fp = fopen ( "Basic.csv", "r" )) == NULL )
	{
		printf ( "Cannot open Basic.csv for reading\n" );
		exit ( 1 );
	}

	lineno = 0;
	while ( fgets ( xbuffer, sizeof(xbuffer), fp ) != NULL )
	{
		lineno++;
		if (( tokcnt = GetTokensD ( xbuffer, ",\n\r", tokens, MAXTOKS )) < 4 )
		{
			continue;
		}
		if ( strcmp ( tokens[0], "Symbol" ) == 0 )
		{
			continue;
		}

		sprintf ( WhereClause, "Sticker = '%-.20s'", tokens[0] );
		if (LoadStock ( &MySql, WhereClause, &xstock, 0, 0 ) != 1 )
		{
			if ( Verbose )
			{
				printf ( "Stock %s not found, can not update fundamental\n",  tokens[0] );
				fflush ( stdout );
			}
			HaveStock = 0;
			MissingStockCount++;
		}
		else
		{
			HaveStock = 1;
			BasicCount++;
		}

		if ( Count >= MAXREC )
		{
			printf ( "Exceeds MAXREC\n" );
			exit ( 1 );
		}

/*----------------------------------------------------------
Symbol,Net Assets,Net Expense Ratio,Inception Date
AIA,$1.3B,0.5,11/13/2007
DGT,$158.7M,0.5,09/25/2000
EEM,$16.1B,0.69,04/07/2003
EFA,$45B,0.33,08/14/2001
EPP,$1.7B,0.47,10/25/2001
EWA,$1.9B,0.5,03/12/1996
EWC,$2.7B,0.5,03/12/1996
EWD,$306.3M,0.54,03/12/1996
EWG,$1.1B,0.5,03/12/1996
----------------------------------------------------------*/

		snprintf ( Array[Count].Ticker, sizeof(Array[Count].Ticker), "%s", tokens[0] );
		Array[Count].HaveStock = HaveStock;

		Array[Count].Assets = nsAtof ( &tokens[1][1] );
		int xl = strlen ( tokens[1] ) - 1;
		switch (  tokens[1][xl] )
		{
			case 'K':
				Array[Count].Assets *= 1000;
				break;
			case 'M':
				Array[Count].Assets *= 1000000;
				break;
			case 'B':
				Array[Count].Assets *= 1000000000;
				break;
			case 'T':
				Array[Count].Assets *= 1000000000000;
				break;
		}
		Array[Count].ExpenseRatio = nsAtof ( tokens[2] );
		StrToDateval ( tokens[3], &dv );
		sprintf ( Array[Count].Inception, "%04d-%02d-%02d", dv.year4, dv.month, dv.day );

		Count++;
	}

	qsort ( Array, Count, sizeof(RECORD), (int(*)()) cmprec );

	fclose ( fp );
}
