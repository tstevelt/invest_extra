/*----------------------------------------------------------------------------
	Program : TrailStop.c
	Author  : Tom Stevelt
	Date    : 03/26/2019
	Synopsis: Rachet up xportfolio.xptrailvalue as needed.  
			  Send messages if triggered.
	Return  : 

	Who		Date		Modification
	---------------------------------------------------------------------
	tms		05/24/2019	Added portfolio.Palerttype = ALERT_TRAIL_STOP
	tms		05/27/2019	Incorported notification in PortfolioAlert,  this 
						program now only updates portfolio.Ptrailvalue

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
#include	"TrailStop.h"

int main ( int argc, char *argv[] )
{
	getargs ( argc, argv );

	StartMySQL ( &MySql, "invest" );

	/*----------------------------------------------------------
		update trailstop values ...
	----------------------------------------------------------*/
	switch ( RunMode )
	{
		case MODE_ALL:
			// sprintf ( WhereClause, "Ptrailpct > 0.01" );
			sprintf ( WhereClause, "Palerttype = '%c'", ALERT_TRAIL_STOP );
			break;

		case MODE_ONE:
			sprintf ( WhereClause, "member.id = %ld", ReportMember );
			if ( LoadMember ( &MySql, WhereClause, &xmember, 0 ) < 1 )
			{
				printf ( "Unknown member id %ld\n", ReportMember );
				exit ( 1 );
			}
			// sprintf ( WhereClause, "Pmember = %ld and Ptrailpct > 0.01", ReportMember );
			sprintf ( WhereClause, "Pmember = %ld and Palerttype = '%c'", ReportMember, ALERT_TRAIL_STOP );
			break;
	}

	LoadPortfolioCB ( &MySql, WhereClause, "Pmember,Pticker", &xportfolio, (int(*)()) EachPortfolio, 0 );

	printf ( "TrailStop: Updated %d, Errors %d\n", UpdateCount, ErrorCount );

#ifdef NOTIFICATION_MOVED_TO_SEND_ALERTS_PROGRAM
	/*-------------------------------------------------------------------------
		send messages to members if any their stocks are below trailing stop
	-------------------------------------------------------------------------*/
	switch ( RunMode )
	{
		case MODE_ALL:
// sprintf ( WhereClause, "(select count(*) from portfolio where Pmember = member.id and Ptrailvalue > 0.01) > 0" );
sprintf ( WhereClause, "(select count(*) from portfolio where Pmember = member.id and Palerttype = '%c') > 0", ALERT_TRAIL_STOP );
			break;

		case MODE_ONE:
			sprintf ( WhereClause, "member.id = %ld", ReportMember );
			break;
	}

	LoadMemberCB ( &MySql, WhereClause, "id", &xmember, (int(*)()) EachMember, 0 );

	printf ( "TrailStop: Updated %d, Errors %d, Sent %d\n", UpdateCount, ErrorCount, ReportCount );
#endif

	return ( 0 );
}
