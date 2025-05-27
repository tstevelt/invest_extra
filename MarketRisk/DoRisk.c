/*---------------------------------------------------------------------------
    Market Risk

    Copyright (C)  2025 Tom Stevelt

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
---------------------------------------------------------------------------*/

#include	"MarketRisk.h"

void DoRisk ()
{
	int		rv, ndx, Start, Count;
	double	PrevAvg, CurrAvg;

	rv = LoadHistoryArray ( &MySql, xstock.xsticker, xstock.xslast, SORT_ASCEND, Days );
	printf ( "%s: Loaded %d history records, earlist date %s\n", xstock.xsticker, rv, HistoryArray[0].Date );
	if ( Method == 2 && rv < Days )
	{
		printf ( "Not enough history for current calculation\n" );
		exit ( 1 );
	}

	switch ( Method )
	{
		case 1:
			printf ( "Date,Previous,Current\n" );
			Days = rv;
			for ( Start = 0; Start < Days - 260; Start++ )
			{
				PrevAvg = CurrAvg = 0.0;
				for ( ndx = Start, Count = 0; Count < 255; ndx++, Count++ )
				{
					PrevAvg += HistoryArray[ndx].Close;
				}
				PrevAvg = PrevAvg / Count;
				for ( Count = 0; ndx < Days && Count < 5; ndx++, Count++ )
				{
					CurrAvg += HistoryArray[ndx].Close;
				}
				CurrAvg = CurrAvg / Count;
				printf ( "%s,%.2f,%.2f\n", HistoryArray[ndx-1].Date, PrevAvg, CurrAvg );
			}
			break;
		case 2:
			PrevAvg = CurrAvg = 0.0;
			for ( ndx = 0, Count = 0; ndx < Days - 5; ndx++, Count++ )
			{
				PrevAvg += HistoryArray[ndx].Close;
			}
			PrevAvg = PrevAvg / Count;
			for ( Count = 0; ndx < Days; ndx++, Count++ )
			{
				CurrAvg += HistoryArray[ndx].Close;
			}
			CurrAvg = CurrAvg / Count;
			printf ( "%s: Previous Average %.2f, Current Average %.2f\n", xstock.xsticker, PrevAvg, CurrAvg );
			break;
	}
}
