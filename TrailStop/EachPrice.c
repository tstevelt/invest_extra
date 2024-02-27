/*----------------------------------------------------------------------------
	Program : TrailStop/EachPrice.c
	Author  : Tom Stevelt
	Date    : 2019 - 2024
	Synopsis: If triggerd, output a line to the report.
	Return  : 
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

#include	"TrailStop.h"

int EachPrice ()
{
	sprintf ( WhereClause, "Sticker = '%s'", xportfolio.xpticker );
	if ( LoadStock ( &MySql, WhereClause, &xstock, 0, 0 ) != 1 )
	{
		return ( 0 );
	}

	sprintf ( WhereClause, "Hticker = '%s' and Hdate = '%s'", xstock.xsticker, xstock.xslast );
	if ( LoadHistory ( &MySql, WhereClause, &xhistory, 0 ) != 1 )
	{
		return ( 0 );
	}
	
	if ( xhistory.xhclose < xportfolio.xptrailvalue )
	{
		if ( MemberCount == 0 )
		{
			sprintf ( OutFileName, "%s/TrailStop_%ld.html", TEMPDIR, xportfolio.xpmember );
			if (( fpOutput = fopen ( OutFileName, "w" )) == (FILE *)0 )
			{
				printf ( "Cannot create %s for output\n", OutFileName );
				exit ( 1 );
			}

			fprintf ( fpOutput, "<html>\n" );
			fprintf ( fpOutput, "<head>\n" );
			fprintf ( fpOutput, "<style>\n" );
			fprintf ( fpOutput, ".title { color: black; background-color: lightgray; }\n" );
			fprintf ( fpOutput, ".heading { color: white; background-color: black; }\n" );
			fprintf ( fpOutput, ".odd { background-color: powderblue; }\n" );
			fprintf ( fpOutput, ".even { background-color: linen; }\n" );
			fprintf ( fpOutput, "</style>\n" );
			fprintf ( fpOutput, "</head>\n" );
			fprintf ( fpOutput, "<body>\n" );
			fprintf ( fpOutput, "<table>\n" );
			fprintf ( fpOutput, "<tr class='title'><td colspan='4'>%s</td></tr>\n", xmember.xmname );
			fprintf ( fpOutput, "<tr class='title'><td colspan='4'>Portfolio Stocks below your Trailing Stops</td></tr>\n" );
			fprintf ( fpOutput, "<tr class='heading'><td>TICKER</td><td>COMPANY</td><td>CLOSE</td><td>STOP</td></tr>\n" );
		}

		fprintf ( fpOutput, "<tr class='%s'><td>%-10.10s</td><td>%-20.20s</td><td>%8.2f</td><td>%8.2f</td></tr>\n", 
			MemberCount % 2 ? "odd" : "even",
			xportfolio.xpticker, xstock.xsname,  xhistory.xhclose, xportfolio.xptrailvalue );

		MemberCount++;
	}

	return ( 0 );
}
