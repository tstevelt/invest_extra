/*----------------------------------------------------------------------------
	Program : PortfolioCorr/EachMember.c
	Author  : Tom Stevelt
	Date    : 2019 - 2024
	Synopsis: For given member, get stocks in portfolio and call EachParent()
	Return  : 0 to continue
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

#include	"PortfolioCorr.h"

int EachMember ()
{
	FILE	*fp;
	char	xbuffer[1024];
	char	*tokens[10];
	int		tokcnt;

	if ( Debug )
	{
		printf ( "%ld %s\n", xmember.xid, xmember.xmname );
	}

	MemberCount = 0;
	SawBenchmark = 0;

	switch ( ReportTable )
	{
		case 'P':
			sprintf ( WhereClause, "Pmember = %ld", xmember.xid );
			LoadPortfolioCB ( &MySql, WhereClause, "Pticker", &xportfolio, (int(*)()) EachParent, 0 );
			break;
		case 'W':
			sprintf ( WhereClause, "Wmember = %ld", xmember.xid );
			LoadWatchlistCB ( &MySql, WhereClause, "Wticker", &xwatchlist, (int(*)()) EachParent, 0 );
			break;
		case 'T':
			sprintf ( WhereClause, "Tmember = %ld", xmember.xid );
			LoadTempCB ( &MySql, WhereClause, "Tticker", &xtemp, (int(*)()) EachParent, 0 );
			break;
		case 'F':
			if (( fp = fopen ( Filename, "r" )) == NULL )
			{
				printf ( "Cannot open file %s\n", Filename );
				exit ( 1 );
			}
			while ( fgets ( xbuffer, sizeof(xbuffer), fp ) != NULL )
			{
				if (( tokcnt = GetTokensD ( xbuffer, ",\t\n\r", tokens, 10 )) < 1 )
				{
					continue;
				}

				sprintf ( WhereClause, "Sticker = '%s'", tokens[0] );
				if (LoadStock ( &MySql, WhereClause, &xstock, 0, 0 ) != 1 )
				{
					printf ( "%s not found\n", tokens[0] );
					continue;
				}

				EachParent();
			}


			fclose ( fp );
			break;
	}


	return ( 0 );
}
