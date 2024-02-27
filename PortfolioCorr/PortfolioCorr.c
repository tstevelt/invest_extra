/*----------------------------------------------------------------------------
	Program : PortfolioCorr.c
	Author  : Tom Stevelt
	Date    : 09/22/2022
	Synopsis: Print array for correlation eval by python script Corr.py
	Return  : 

	Who		Date		Modification
	---------------------------------------------------------------------
	tms		01/19/2024	Added F (file) option

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
#include	"PortfolioCorr.h"

int main ( int argc, char *argv[] )
{
	int		ndx, xs, xf;

	getargs ( argc, argv );

	StartMySQL ( &MySql, "invest" );

	sprintf ( WhereClause, "member.id = %ld", ReportMember );
	LoadMemberCB ( &MySql, WhereClause, "id", &xmember, (int(*)()) EachMember, 0 );

	printf ( "DATE" );
	for ( xs = 0; xs < DataIndex; xs++ )
	{
		printf ( ",%s",  TickerArray[xs] );
	}
	printf ( "\n" );
	for ( ndx = 0; ndx < DataCount; ndx++ )
	{
		for ( xs = 0, xf = 0; xs < DataIndex; xs++ )
		{
			if ( DataArray[ndx].Close[xs] < 0.01 )
			{
				xf = 1;
				break;
			}
		}
		if ( xf )
		{
			continue;
		}

		printf ( "%s", DataArray[ndx].Date );
		for ( xs = 0; xs < DataIndex; xs++ )
		{
			printf ( ",%8.2f", DataArray[ndx].Close[xs] );
		}
		printf ( "\n" );
	}

	return ( 0 );
}
