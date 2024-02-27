/*----------------------------------------------------------------------------
	Program : WatchlistAlertRT/EachMember.c
	Author  : Tom Stevelt
	Date    : 2019 - 2024
	Synopsis: for each member, check their watchlist
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

#include	"WatchlistAlertRT.h"

int EachMember ()
{
	printf ( "%ld %s\n", xmember.xid, xmember.xmname );

	if ( xmember.xmpager == (char *)0 || nsStrlen ( xmember.xmpager ) == 0 || nsStrchr ( xmember.xmpager, '@' ) == (char *)0 )
	{
		printf ( "%ld %s NO PAGER ON FILE\n", xmember.xid, xmember.xmname );
		return ( 0 );
	}

	MemberCount = 0;
	
	sprintf ( WhereClause, "Wmember = %ld and Walerttype != '%c'", xmember.xid, ALERT_NONE );
	LoadWatchlistCB ( &MySql, WhereClause, "Wticker", &xwatchlist, (int(*)()) EachPrice, 0 );

	return ( 0 );
}
