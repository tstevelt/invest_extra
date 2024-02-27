/*----------------------------------------------------------------------------
	Program : WatchlistAlertRT.c
	Author  : Tom Stevelt
	Date    : 07/05/2019
	Synopsis: RT stands for Real Time, but only runs as often as called from 
				crontab.  Send SMS message if watchlist alert is triggered.
	Return  : 

	Who		Date		Modification
	---------------------------------------------------------------------
	tms		07/13/2021	Get private api keys with GetInvestCfg().

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
#include	"WatchlistAlertRT.h"

int main ( int argc, char *argv[] )
{
	getargs ( argc, argv );

	GetInvestCfg ( 0 );

	StartMySQL ( &MySql, "invest" );

	/*-------------------------------------------------------------------------
		send messages to members if any their stocks are below trailing stop
	-------------------------------------------------------------------------*/
	switch ( RunMode )
	{
		case MODE_ONE:
			sprintf ( WhereClause, "member.id = %ld", ReportMember );
			break;
	}

	LoadMemberCB ( &MySql, WhereClause, "id", &xmember, (int(*)()) EachMember, 0 );

	printf ( "WatchlistAlertRT:  Sent %d\n",  ReportCount );

	return ( 0 );
}
