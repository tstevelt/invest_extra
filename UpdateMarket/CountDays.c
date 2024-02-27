/*----------------------------------------------------------------------------
	Program : UpdateMarket/CountDays.c
	Author  : Tom Stevelt
	Date    : 2019 - 2024
	Synopsis: Count accumulation and distribution days.
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

#include	"UpdateMarket.h"


void CountDays ( int *Acount, int *Dcount, int PrintReport )
{
	int		ndx;
	double	Diff, Percent;

	for ( ndx = 0; ndx < Count; ndx++ )
	{
		Array[ndx].AvgPrice  = Array[ndx].SumPrice / (double) Array[ndx].Count;
		Array[ndx].AvgVolume = Array[ndx].SumVolume / (double) Array[ndx].Count;
	}

	*Acount = *Dcount = 0;
	for ( ndx = 0; ndx < Count; ndx++ )
	{
		if ( PrintReport )
		{
			printf ( "%s %8.2f %14.0f %3d", Array[ndx].Date, Array[ndx].AvgPrice, Array[ndx].AvgVolume, Array[ndx].Count );
		}

		if ( Debug )
		{
			printf ( "%s ", Array[ndx].Date );
		}

		if ( ndx > 0 )
		{
			Diff = Array[ndx].AvgPrice - Array[ndx-1].AvgPrice;
			Percent = 100.0 * Diff / Array[ndx-1].AvgPrice;

			if ( PrintReport )
			{
				printf ( " %6.2f", Percent );
			}

			if ( Array[ndx].AvgVolume > Array[ndx-1].AvgVolume )
			{
				if ( Percent > 0.2 )
				{
					if ( PrintReport || Debug )
					{
						printf ( " Accumulate" );
					}
					*Acount = *Acount + 1;
				}
				else if ( Percent < -0.2 )
				{
					if ( PrintReport || Debug )
					{
						printf ( " Distribute" );
					}
					*Dcount = *Dcount + 1;
				}
			}
		}

		if ( PrintReport || Debug )
		{
			printf ( "\n" );
		}
	}

	if ( PrintReport )
	{
		printf ( "Accumulation %d days\n", *Acount );
		printf ( "Distribution %d days\n", *Dcount );
	}
}
