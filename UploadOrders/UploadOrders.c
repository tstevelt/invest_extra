/*----------------------------------------------------------------------------
	Program : UploadOrders/UploadOrders.c
	Author  : Tom Stevelt
	Date    : 2019 - 2024
	Synopsis: Upload buy and sell orders, update portfolo and cashbal tables

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
#include	"UploadOrders.h"

int main ( int argc, char *argv[] )
{
	char	xbuffer[1024];
	FILE	*fp;
#define		MAXTOKS		10
	char	*tokens[MAXTOKS];
	int		tokcnt;
	int		rv, Errors, NotInDB, NoChange, TotalSold, TotalBought, lineno;
	char	Today[12];
//	double	Profit, TotalProfit;

	getargs ( argc, argv );

	if (( fp = fopen ( Filename, "r" )) == (FILE *)0 )
	{
		printf ( "Cannot open file %s for input\n", Filename );
		exit ( 1 );
	}

	sprintf ( Today, "%s", fmtGetTodayDate ( DATEFMT_YYYY_MM_DD ) );
	
	StartMySQL ( &MySql, "invest" );

	Errors = NotInDB = NoChange = TotalSold = TotalBought = lineno = 0;
//	Profit = TotalProfit = 0.0;

	while ( fgets ( xbuffer, sizeof(xbuffer), fp ) != (char *)0 )
	{
		lineno++;

		if (( tokcnt = GetTokensA ( xbuffer, ",\n\r", tokens, MAXTOKS )) < 4 )
		{
			printf ( "Syntax error %d\n", lineno );
			Errors++;
			continue;
		}

		if ( nsStrcmp ( tokens[0], "ACTION" ) == 0 )
		{
			continue;
		}

		if ( nsStrcmp ( tokens[0], "SELL" ) == 0 )
		{
			if (( rv = OrderEntrySell ( &MySql, xmember.xid, tokens[1], Today, 0.0 - nsAtof(tokens[2]), nsAtof(tokens[3]) )) != 0 )
			{
				printf ( "OrderEntrySell failed, rv %d, line %d\n", rv, lineno );
				Errors++;
			}
			else
			{
				TotalSold++;	
			}
		}
		else if ( nsStrcmp ( tokens[0], "BUY" ) == 0 )
		{
			if (( rv = OrderEntryBuy ( &MySql, xmember.xid, tokens[1], Today, nsAtof(tokens[2]), nsAtof(tokens[3]) )) != 0 )
			{
				printf ( "OrderEntryBuy failed, rv %d, line %d\n", rv, lineno );
				Errors++;
			}
			else
			{
				TotalBought++;	
			}
		}
		else
		{
			printf ( "Unknown line %d %s\n", lineno, tokens[0] );
			Errors++;
		}
	}

	nsFclose ( fp );

	printf ( "Read      %d\n", lineno );
//	printf ( "No in DB  %d\n", NotInDB );
//	printf ( "No change %d\n", NoChange );
	printf ( "Sold      %d\n", TotalSold );
	printf ( "Bought    %d\n", TotalBought );
	printf ( "Errors    %d\n", Errors );

	return ( 0 );
}
