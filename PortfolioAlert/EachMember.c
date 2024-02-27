/*----------------------------------------------------------------------------
	Program : PortfolioAlert/EachMember.c
	Author  : Tom Stevelt
	Date    : 2019 - 2024
	Synopsis: For each member, check alerts on portfolio stocks with alerts.
				Send alert if any found.
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

#include	"PortfolioAlert.h"

#undef SHOW_OPTION_EXPIRATION
#define SHOW_OPTION_EXPIRATION

#undef	INCLUDE_ALERT_NONE
#define	INCLUDE_ALERT_NONE

int EachMember ()
{
	char	cmdline[1024];

	if ( Debug )
	{
		printf ( "%ld %s\n", xmember.xid, xmember.xmname );
	}

	MemberCount = 0;
	SawBenchmark = 0;
	
#ifdef INCLUDE_ALERT_NONE
	sprintf ( WhereClause, "Pmember = %ld", xmember.xid );
#else
#ifdef SHOW_OPTION_EXPIRATION
	sprintf ( WhereClause, "Pmember = %ld and Palerttype != '%c'", xmember.xid, ALERT_NONE );
#else
	sprintf ( WhereClause, 
		"Pmember = %ld and Palerttype != '%c' and (Poptexp is null or Poptexp = '0000-00-00')", 
				xmember.xid, ALERT_NONE );
#endif
#endif

	LoadPortfolioCB ( &MySql, WhereClause, "Pticker", &xportfolio, (int(*)()) EachPortfolio, 0 );

	if ( MemberCount > 0 )
	{
		if ( SawBenchmark )
		{
			fprintf ( fpOutput, "<tr>\n" );
#ifdef SHOW_OPTION_EXPIRATION
			fprintf ( fpOutput, "<td class='bench' colspan='6'>\n" );
#else
			fprintf ( fpOutput, "<td class='bench' colspan='5'>\n" );
#endif
			fprintf ( fpOutput, "Benchmark values are ROI, not prices." );
			fprintf ( fpOutput, "</td>\n" );
			fprintf ( fpOutput, "</tr>\n" );

			fprintf ( fpOutput, "<tr>\n" );
#ifdef SHOW_OPTION_EXPIRATION
			fprintf ( fpOutput, "<td class='bench' colspan='6'>\n" );
#else
			fprintf ( fpOutput, "<td class='bench' colspan='5'>\n" );
#endif
			fprintf ( fpOutput, "CURRENT is stock ROI plus YIELD." );
			fprintf ( fpOutput, "</td>\n" );
			fprintf ( fpOutput, "</tr>\n" );

			fprintf ( fpOutput, "<tr>\n" );
#ifdef SHOW_OPTION_EXPIRATION
			fprintf ( fpOutput, "<td class='bench' colspan='6'>\n" );
#else
			fprintf ( fpOutput, "<td class='bench' colspan='5'>\n" );
#endif
			fprintf ( fpOutput, "VALUE is benchmark ROI minus LIMIT PERCENT." );
			fprintf ( fpOutput, "</td>\n" );
			fprintf ( fpOutput, "</tr>\n" );

		}
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
			snprintf ( cmdline, sizeof(cmdline), "Portfolio Alert for %d stocks", MemberCount);
			SendAttached ( REPORT_ADDRESS, xmember.xmemail, "", "", cmdline, 1, OutFileName, 0, NULL );
			unlink ( OutFileName );

			ReportCount++;
		}
	}

	return ( 0 );
}
