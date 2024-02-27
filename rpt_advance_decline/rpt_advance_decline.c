/*----------------------------------------------------------------------------
	Program : rpt_advance_decline/rpt_advance_decline.c
	Author  : Tom Stevelt
	Date    : 2019 - 2024
	Synopsis: For stocks in selected index, count advanced and declined
				by date.  Can be run for 1, 6 or 12 months.

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
#include	"rpt_advance_decline.h"

int main ( int argc, char *argv[] )
{
	double	Percent;
	int		ndx;

	getargs ( argc, argv );
	TotalAdvanceCount = TotalDeclineCount = 0;
	
	StartMySQL ( &MySql, "invest" );

	SetWhereClause ( WhereClause, StockIndex );
	strcat ( WhereClause, " and Slast is not null" );
	LoadStockCB ( &MySql, WhereClause, "Sticker", &xstock, EachStock, 1 );

	printf ( "COUNT OF STOCK WHICH ADVANCED AND DECLINED ON DATE\n" );
	printf ( "%s\n", IndexName(StockIndex) );
	printf ( "DATE       ADVANCED  DECLINED  PERCENT\n" );
	//        x10xxxxxxx     xxxx      xxxx    xxx.x
	printf ( "--------------------------------------------------\n" );
	for ( ndx = 0; ndx < DailyCount; ndx++ )
	{
		Percent = 100.0 * (double) DailyArray[ndx].DayAdvanceCount / 
							(double)(DailyArray[ndx].DayAdvanceCount + DailyArray[ndx].DayDeclineCount);

		printf ( "%-10.10s     %4d      %4d    %5.1f\n",
			DailyArray[ndx].Date,
			DailyArray[ndx].DayAdvanceCount,
			DailyArray[ndx].DayDeclineCount,
			Percent );
	}

	printf ( "%-10.10s   ------    ------    -----\n", " " );

	Percent = 100.0 * (double) TotalAdvanceCount / (double)(TotalAdvanceCount + TotalDeclineCount);

	printf ( "%-10.10s   %6.1f    %6.1f    %5.1f\n",
		"AVERAGE", (double)TotalAdvanceCount/(double)DailyCount, (double)TotalDeclineCount/(double)DailyCount, Percent );

	return ( 0 );
}
