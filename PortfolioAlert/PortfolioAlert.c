/*----------------------------------------------------------------------------
	Program : PortfolioAlert.c
	Author  : Tom Stevelt
	Date    : 01/24/2019
	Synopsis: Examine [user's] PortfolioAlert and send alerts as necessary
	Return  : 

	Who		Date		Modification
	---------------------------------------------------------------------
	tms		03/06/2019	Separated from portfolio program.
	tms		03/26/2019	Revised to match TrailStop architeture.
	tms		05/27/2019	Rewrite to use portfolio.Palerttype, send all
						types of alerts in one message. 
	tms		04/28/2022	Added hard-coded one-day change +-3% if ALERT_NONE
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
#include	"PortfolioAlert.h"

int main ( int argc, char *argv[] )
{
	getargs ( argc, argv );

	StartMySQL ( &MySql, "invest" );

	CurrentDateval ( &dvToday );
	DateAdd ( &dvToday, -7, &dvWeekAgo );
	sprintf ( strWeekAgo, "%04d-%02d-%02d", dvWeekAgo.year4, dvWeekAgo.month, dvWeekAgo.day );
	if ( Debug )
	{
		printf ( "Week ago %s\n", strWeekAgo );
	}

	switch ( RunMode )
	{
		case MODE_ALL:
			// fixit -- remove Palerttype.
			sprintf ( WhereClause, 
				"(select count(*) from portfolio where Pmember = member.id and Palerttype != '%c') > 0", ALERT_NONE );
			break;

		case MODE_ONE:
			sprintf ( WhereClause, "member.id = %ld", ReportMember );
			break;
	}

	LoadMemberCB ( &MySql, WhereClause, "id", &xmember, (int(*)()) EachMember, 0 );

	printf ( "PortfolioAlert: sent %d\n", ReportCount );

	return ( 0 );
}
