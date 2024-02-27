/*----------------------------------------------------------------------------
	Program : UpdateMarket.c
	Author  : Tom Stevelt
	Date    : 03/12/2019
	Synopsis: 
	Distribution Days: These are days where the Nasdaq or S&P 500 are down 
	significantly (0.20%) on higher volume than the prior day — a sign of
	institutional selling (also known as distribution). When you see a string
	of distribution days in a short amount of time, it often means the current
	uptrend is starting to roll over into a correction.For example, if you 
	get 5 - 7 distribution days within a 4 - 5 week period, the Market Pulse
	will typically change from "Confirmed uptrend" to "Uptrend under pressure."
	And if distribution days continue to mount, the Market Pulse will likely 
	switch from "Uptrend under pressure" to "Market in correction."

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

#define	MAIN
#include	"UpdateMarket.h"

int main ( int argc, char *argv[] )
{
	int		Acount, Dcount;

	getargs ( argc, argv );
	
	StartMySQL ( &MySql, "invest" );

	if ( CalcBothIndex == 0 )
	{
		/*----------------------------------------------------------------------
			this is either one stock, or one index.  report only, no update
		----------------------------------------------------------------------*/
		LoadStockCB ( &MySql, WhereClause, NULL, &xstock, (int(*)()) EachStock, 0 );

		CountDays ( &Acount, &Dcount, 1 );
	}
	else
	{
		memset ( Array, '\0', sizeof(Array) );
		Count = 0;
		sprintf ( WhereClause, "Slast is not NULL and Ssp500 = 'Y'" );
		LoadStockCB ( &MySql, WhereClause, NULL, &xstock, (int(*)()) EachStock, 0 );
		CountDays ( &Acount, &Dcount, 0 );
		SaveMarket ( Array[Count-1].Date, 'S', Acount, Dcount );


		memset ( Array, '\0', sizeof(Array) );
		Count = 0;
		sprintf ( WhereClause, "Slast is not NULL and Snasdaq = 'Y'" );
		LoadStockCB ( &MySql, WhereClause, NULL, &xstock, (int(*)()) EachStock, 0 );
		CountDays ( &Acount, &Dcount, 0 );
		SaveMarket ( Array[Count-1].Date, 'N', Acount, Dcount );
	}

	printf ( "UpdateMarket: inserted %d, updated %d market records.\n", InsertCount, UpdateCount );

	return ( 0 );
}
