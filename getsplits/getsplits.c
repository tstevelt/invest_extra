/*----------------------------------------------------------------------------
	Program : getsplits.c
	Author  : Tom Stevelt
	Date    : Jul 2021
	Synopsis: Scan stock history for price jumps, verify with IEX /splits/
	Return  : 

	Who		Date		Modification
	---------------------------------------------------------------------
	tms		07/21/2021	Don't bother with average comparisons for ONE TICKER

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
#include	"getsplits.h"

int main ( int argc, char *argv[] )
{
	getargs ( argc, argv );

	GetInvestCfg ( 0 );

	StartMySQL ( &MySql, "invest" );

	switch ( RunMode )
	{
		case MODE_ACTIVE:
			sprintf ( WhereClause, "(select count(*) from portfolio where Pticker = Sticker) > 0" );
			LoadStockCB ( &MySql, WhereClause, "Sticker", &xstock, (int(*)()) EachStock, 0 );
			break;

		case MODE_ALL:
			LoadStockCB ( &MySql, NULL, "Sticker", &xstock, (int(*)()) EachStock, 0 );
			break;

		case MODE_ONE:
			sprintf ( WhereClause, "Sticker = '%s'", OneTicker );
			LoadStockCB ( &MySql, WhereClause, "Sticker", &xstock, (int(*)()) EachStock, 1 );
			break;
	
	}

	return ( 0 );
}
