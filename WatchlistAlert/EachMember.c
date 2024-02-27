/*----------------------------------------------------------------------------
	Program : WatchlistAlert/EachMember.c
	Author  : Tom Stevelt
	Date    : 2019 - 2024
	Synopsis: For each member, send alerts, if any.
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

#include	"WatchlistAlert.h"

int EachMember ()
{
	char	cmdline[1024];

	if ( Debug )
	{
		printf ( "%ld %s\n", xmember.xid, xmember.xmname );
	}

	MemberCount = 0;
	
	sprintf ( WhereClause, "Wmember = %ld and Walerttype != '%c'", xmember.xid, ALERT_NONE );
	LoadWatchlistCB ( &MySql, WhereClause, "Wticker", &xwatchlist, (int(*)()) EachPrice, 0 );

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
			snprintf ( cmdline, sizeof(cmdline), "Watchlist Alerts Triggered for %d stocks", MemberCount);
			SendAttached ( REPORT_ADDRESS, xmember.xmemail, "", "", cmdline, 1, OutFileName, 0, NULL );
			unlink ( OutFileName );

			ReportCount++;
		}
	}

	return ( 0 );
}
