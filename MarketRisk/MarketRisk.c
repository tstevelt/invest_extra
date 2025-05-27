/*----------------------------------------------------------------------------
	Program : MarketRisk.c
	Author  : Tom Stevelt
	Date    : 02/27/2025
	Synopsis: Compare previous week avg against previous 12 avg.  Based on
			  logic from Victor Hagani at ELM.
	
	Notes	: This program works on RISK, not Expected Return.
			  ELM uses ER = 1/PCAPE - 10yrTIPS. 
			  CAPE: https://en.wikipedia.org/wiki/Cyclically_adjusted_price-to-earnings_ratio
			  PCAPE: https://www.morningstar.com/markets/improving-cape-10

	Who		Date		Modification
	---------------------------------------------------------------------
	tms		02/28/2025	Realized that first version is basically the 
						same as comparing average.Ama5 to average.Ama200.
	tms		02/28/2025	Added MODE_RISK (same as version 1) and MODE_ALLOC
----------------------------------------------------------------------------*/
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

#define		MAIN
#include	"MarketRisk.h"

int main ( int argc, char *argv[] )
{
	StartMySQL ( &MySql, "invest" );

	getargs ( argc, argv );

	switch ( RunMode )
	{
		case MODE_RISK:
			DoRisk ();
			break;
		case MODE_ALLOC:
			DoAlloc ();
			break;
		case MODE_ER:
			DoExpected ();
			break;
	}

	return ( 0 );
}
