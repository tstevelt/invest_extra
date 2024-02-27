/*----------------------------------------------------------------------------
	Program : UpdateMarket/EachHistory.c
	Author  : Tom Stevelt
	Date    : 2019 - 2024
	Synopsis: Add history data to Array
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

int EachHistory ()
{
	RECORD	Key, *ptr;
	int		NeedSort;

	sprintf ( Key.Date, "%s", xhistory.xhdate );
	NeedSort = 0;

	if ( Count > 0 )
	{
		ptr = bsearch ( &Key, Array, Count, sizeof(RECORD), (int(*)()) cmprec );
	}
	else
	{
		ptr = (RECORD *)0;
	}

	if ( ptr == (RECORD *)0 )
	{
		if ( Count >= MAXRECS )
		{
			printf ( "Exceeds MAXRECS\n" );
			exit ( 1 );
		}

		sprintf ( Array[Count].Date, "%s", xhistory.xhdate );
		ptr = &Array[Count];
		Count++;
		NeedSort = 1;
	}

	ptr->SumPrice  = ptr->SumPrice  + xhistory.xhclose;
	ptr->SumVolume = ptr->SumVolume + xhistory.xhvolume;
	ptr->Count     = ptr->Count     + 1;


	if ( NeedSort )
	{
		qsort ( Array, Count, sizeof(RECORD), (int(*)()) cmprec );
	}

	return ( 0 );
}
