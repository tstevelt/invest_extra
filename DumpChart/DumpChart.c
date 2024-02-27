/*----------------------------------------------------------------------------
	Program : DumpChart.c
	Author  : Tom Stevelt
	Date    : 03/18/2019
	Synopsis: Print data for one-year chart to stdout.

	Who		Date		Modification
	---------------------------------------------------------------------
	tms		04/08/2019	Moved averages from history to average table.
	tms		05/27/2019	Added average.Arsi
						Broke EachStock into three calls.

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
#include	"DumpChart.h"

int main ( int argc, char *argv[] )
{
	getargs ( argc, argv );

	StartMySQL ( &MySql, "invest" );

	/*----------------------------------------------------------
		WhereClause is set in getargs().
	----------------------------------------------------------*/
	LoadStockCB ( &MySql, WhereClause, "Sticker", &xstock, (int(*)()) EachStock, 0 );

	return ( 0 );
}
