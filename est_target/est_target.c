/*----------------------------------------------------------------------------
	Program : est_target.c
	Author  : Silver Hammer Software LLC
	Author  : Tom Stevelt
	Date    : Feb 2024
	Synopsis: Given total value, rmd required (7 year), cash req (6 mo)
			  calculate percentages for cash, bond, domestic and foreign
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

#include	<stdio.h>
#include	<stdlib.h>
#include	<unistd.h>
#include	<string.h>
#include	"shslib.h"

int main ( int argc, char *argv[] )
{
	double	Total, RMD, Cash, Ratio;
	double	CashPct, BondPct, DomPct, ForPct, EquityPct;

	if ( argc < 5 )
	{
		printf ( "\n" );
		printf ( "Estimate target asset allocation percentages\n" );
		printf ( "\n" );
		printf ( "USAGE: est_target total rmd cash dom_for_ratio\n" );
		printf ( " total - total portfolio value\n" );
		printf ( " rmd   - rmd total for the next X years\n" );
		printf ( " cash  - emergency cash amount\n" );
		printf ( " dom_for_ratio - domestic/foreign equity ratio\n" );
		exit ( 1 );
	}
	Total = nsAtof ( argv[1] );
	RMD   = nsAtof ( argv[2] );
	Cash  = nsAtof ( argv[3] );
	Ratio = nsAtof ( argv[4] );

	CashPct = Cash / Total;
	BondPct = RMD / Total;

	EquityPct = 1.0 - CashPct - BondPct;
	DomPct = EquityPct * Ratio;
	ForPct = EquityPct - DomPct;

	printf ( "CASH     %5.1f\n", 100.0 * CashPct );
 	printf ( "BOND     %5.1f\n", 100.0 * BondPct );
	printf ( "DOMESTIC %5.1f\n", 100.0 * DomPct );
	printf ( "FOREIGN  %5.1f\n", 100.0 * ForPct );





	return ( 0 );
}
