/*----------------------------------------------------------------------------
	Program : UpdateMarket/EachStock.c
	Author  : Tom Stevelt
	Date    : 2019 - 2024
	Synopsis: for each stock get history
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

int EachStock ()
{
	if ( CalcBothIndex == 0 )
	{
		sprintf ( WhereClause, "Hticker = '%s' and Hdate > date_sub( curdate(),interval %d day)", xstock.xsticker, Weeks * 7 + 1 );
	}
	else
	{
		sprintf ( WhereClause, 
			"Hticker = '%s' and Hdate <= '%s' and Hdate > date_sub( '%s',interval %d day)", 
					xstock.xsticker, EndDate, EndDate, Weeks * 7 + 1 );
	}
	LoadHistoryCB ( &MySql, WhereClause, "Hdate desc", &xhistory, (int(*)()) EachHistory, 0 );

	return ( 0 );
}
