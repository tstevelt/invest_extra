/*----------------------------------------------------------------------------
	Program : UpdateMarket/SaveMarket.c
	Author  : Tom Stevelt
	Date    : 2019 - 2024
	Synopsis: Insert/Update market record
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

void SaveMarket ( char *LatestDate, char StockIndex, int Acount, int Dcount )
{
	sprintf ( WhereClause, "MKTdate = '%s' and MKTindex = '%c'", LatestDate, StockIndex );	
	if ( LoadMarket ( &MySql, WhereClause, &xmarket, 0 ) == 1 )
	{
		snprintf ( Statement, sizeof(Statement),
			"update market set MKTaccum = %d, MKTdist = %d where %s", Acount, Dcount, WhereClause );

		dbyUpdate ( "invest", &MySql, Statement, 0, LogFileName );

		UpdateCount++;
	}
	else
	{
		snprintf ( Statement, sizeof(Statement),
			"insert into market (MKTdate,MKTindex,MKTaccum,MKTdist) value ( '%s', '%c', %d, %d )",
						LatestDate, StockIndex, Acount, Dcount );

		dbyInsert ( "invest", &MySql, Statement, 0, LogFileName );

		InsertCount++;
	}
}
