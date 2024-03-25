/*----------------------------------------------------------------------------
	Program : CheckExpire/EachMember.c
	Author  : Tom Stevelt
	Date    : 2019 - 2024
	Synopsis: Send alert message to member if any alerts triggered.
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

#include	"CheckExpire.h"

int EachMember ()
{
	char	cmdline[1024];

	if ( Debug )
	{
		printf ( "%ld %s\n", xmember.xid, xmember.xmname );
	}

	MemberCount = 0;
	
	// sprintf ( WhereClause, "Pmember = %ld and Ptrailvalue > 0.01", xmember.xid );
	sprintf ( WhereClause, "Pmember = %ld and Palerttype = '%c'", xmember.xid, ALERT_TRAIL_STOP );
	LoadPortfolioCB ( &MySql, WhereClause, "Pticker", &xportfolio, (int(*)()) EachPrice, 0 );

	if ( MemberCount > 0 )
	{
		fprintf ( fpOutput, "</table>\n" );
		fprintf ( fpOutput, "</body>\n" );
		fprintf ( fpOutput, "</html>\n" );
		nsFclose ( fpOutput );

		if  ( Debug )
		{
			sprintf ( cmdline, "cat %s", OutFileName );
			system ( cmdline );
		}
		else
		{
			snprintf ( cmdline, sizeof(cmdline), "Portfolio Trailing Stops for %d stocks", MemberCount);
			SendAttached ( REPORT_ADDRESS, xmember.xmemail, "", "", cmdline, 1, OutFileName, 0, NULL );
			unlink ( OutFileName );

			ReportCount++;
		}
	}

	return ( 0 );
}
