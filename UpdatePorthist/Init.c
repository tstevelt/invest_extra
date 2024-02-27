/*----------------------------------------------------------------------------
	Program : UpdatePorthist/Init.c
	Author  : Tom Stevelt
	Date    : 2019 - 2024
	Synopsis: Find/Verify latest history date. Verify member, if used.
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

#include	"UpdatePorthist.h"

void Init ()
{
	DBY_QUERY	*Query = NULL;

	if ( ascHistDate[0] == '\0' )
	{
		sprintf ( Statement, "select max(Hdate) from history" ); 

		Query = dbySelect ( "invest", &MySql, Statement, LogFileName );

		if ( Query == NULL )
		{
			printf ( "Cannot find max(Hdate)\n" );
			exit ( 1 );
		}
		else if ( Query->NumRows == 0 )
		{
			printf ( "Cannot find max(Hdate)\n" );
			exit ( 1 );
		}
		else 
		{
			if (( Query->EachRow = mysql_fetch_row ( Query->Result )) != NULL )
			{
				sprintf ( ascHistDate, "%s", Query->EachRow[0] );
				if ( Debug )
				{
					printf ( "max(Hdate) = %s\n", ascHistDate );
				}
			}
			else
			{
				printf ( "Cannot find max(Hdate)\n" );
				exit ( 1 );
			}
		}
	}
	else
	{
		long		rv;

		sprintf ( WhereClause, "Hdate = '%s'", ascHistDate ); 
		
		rv = dbySelectCount ( &MySql, "history", WhereClause, LogFileName );

		if ( rv == 0 )
		{
			printf ( "No history for %s\n", ascHistDate );
			exit ( 1 );
		}
		if ( Debug )
		{
			printf ( "%ld history records\n", rv );
		}
	}

	if ( RunMode == MODE_ONE )
	{
		sprintf ( WhereClause, "id = %ld", xmember.xid );
		if ( LoadMember ( &MySql, WhereClause, &xmember, 0 ) == 0 )
		{
			printf ( "Unknown member %ld\n", xmember.xid );
			exit ( 1 );
		}

		if ( Debug )
		{
			printf ( "One member %s\n", xmember.xmname );
		}
	}
}
