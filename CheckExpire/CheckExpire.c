/*----------------------------------------------------------------------------
	Program : CheckExpire.c
	Author  : Tom Stevelt
	Date    : 03/26/2019
	Synopsis: Check Poptexp against Sname for Stype = 'B'
			  Send messages if triggered.
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
#include	"CheckExpire.h"

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
			sprintf ( WhereClause, "Poptexp like '20%%'" );
			break;

		case MODE_ONE:
			sprintf ( WhereClause, "member.id = %ld", ReportMember );
			if ( LoadMember ( &MySql, WhereClause, &xmember, 0 ) < 1 )
			{
				printf ( "Unknown member id %ld\n", ReportMember );
				exit ( 1 );
			}
			sprintf ( WhereClause, "Pmember = %ld and Poptexp like '20%%'", ReportMember );
			break;
	}

	LoadPortfolioCB ( &MySql, WhereClause, "Pmember,Pticker", &xportfolio, (int(*)()) EachPortfolio, 0 );

	printf ( "CheckExpire: Checked %d, Errors %d\n", UpdateCount, ErrorCount );

	return ( 0 );
}
