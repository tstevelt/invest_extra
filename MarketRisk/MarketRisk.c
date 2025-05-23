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
