/*----------------------------------------------------------------------------
	Program : UpdatePorthist.c
	Author  : Tom Stevelt
	Date    : 03/18/2019
	Synopsis: Insert/Update porthist record[s] for one date. If no date
				given on the command line, run for the latest date
				in history.
	Return  : 

	Who		Date		Modification
	---------------------------------------------------------------------

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
#include	"UpdatePorthist.h"

int main ( int argc, char *argv[] )
{
	getargs ( argc, argv );

	StartMySQL ( &MySql, "invest" );

	Init ();

	switch ( RunMode )
	{
		case MODE_ALL:
			sprintf ( WhereClause, "id > 0" );
			LoadMemberCB ( &MySql, WhereClause, NULL, &xmember, EachMember, 0 );
			break;

		case MODE_ONE:
			EachMember ();
			break;
	}

	printf ( "UpdatePorthist: Checked %d members\n", VisitCount );
	printf ( "UpdatePorthist: Insert  %d\n", InsertCount );
	printf ( "UpdatePorthist: Update  %d\n", UpdateCount );
	printf ( "UpdatePorthist: Warning %d\n", WarnCount );
	printf ( "UpdatePorthist: Errors  %d\n", ErrorCount );

	return ( 0 );
}
