/*----------------------------------------------------------------------------
	Program :LoadTemp.c
	Author  : Tom Stevelt
	Date    : 07/29/2019
	Synopsis: load temp table from various arguments. expand as needed.
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
#include	"LoadTemp.h"

static int Counter = 0;

static int EachStock ()
{
	int		rv;

	Counter++;

	if ( Counter == 1 )
	{
		sprintf ( Statement, "delete from temp where Tmember = %ld", MemberID );
		dbyDelete ( "invest", &MySql, Statement, 0, LogFileName );
	}

	sprintf ( Statement, "insert into temp (Tmember, Tticker) values ( %ld, '%s' )", MemberID, xstock.xsticker );
	rv = dbyUpdate ( "invest", &MySql, Statement, 0, LogFileName );

	if ( rv != 1 )
	{
		printf ( "dbyUpdate failed, stock %s, rv %d\n", xstock.xsticker, rv );	
	}

	return ( 0 );
}

int main ( int argc, char *argv[] )
{
	getargs ( argc, argv );

	StartMySQL ( &MySql, "invest" );

	LoadStockCB ( &MySql, WhereClause, NULL, &xstock, (int(*)()) EachStock, 0 );
	
	printf ( "Loaded %d\n", Counter );

	return ( 0 );
}
