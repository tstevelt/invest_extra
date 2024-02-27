/*---------------------------------------------------------------------------
	Program : getfundFidelity.c
	Author  : Tom Stevelt
	Date    : 11/13/2023
	Synopsis: process fundamantals csv files from fidelity
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

#include	"getfundFidelity.h"

int main ( int argc, char *argv[] )
{
// printf ( "sizeof long %ld\n", sizeof(long) );

	getargs ( argc, argv );

	StartMySQL ( &MySql, "invest" );

	ChangesFilename = "/var/local/tmp/getfundFidelityChanges.txt";
	ChangesCount = 0;

	GetBasic ();
	GetIncome ();
	GetPerformance ();
	GetAnalyst ();

	if ( Debug == 0 )
	{
		UpdateFundamentals ();
	}

	if ( Verbose )
	{
		for ( int ndx = 0; ndx < Count; ndx++ )
		{
			printf ( "%-10.10s %d %d  %12.0f %6.2f %10.10s %6.2f %10.4f %10.4f %c %c\n",
					Array[ndx].Ticker,
					Array[ndx].HaveStock,
					Array[ndx].HaveFundamental,
					Array[ndx].Assets,
					Array[ndx].ExpenseRatio,
					Array[ndx].Inception,
					Array[ndx].Yield,
					Array[ndx].AvgRet3,
					Array[ndx].AvgRet5,
					Array[ndx].Factset     < 'A' ? ' ' : Array[ndx].Factset,
					Array[ndx].Morningstar < '1' ? ' ' : Array[ndx].Morningstar );

		}
		printf ( "%d updates\n", Count );
	}

	printf ( "Total %d,Loaded %d, Missing Stocks %d, No Change %d, Updated %d, Errors %d\n",
			Count, BasicCount, MissingStockCount, NoChangeCount, UpdateCount, ErrorCount );

	if ( ChangesCount > 0 )
	{
		printf ( "Change Report at %s\n", ChangesFilename );
	}

	return ( 0 );
}
