/*----------------------------------------------------------------------------
	Program : SyncPortfolio/getargs.c
	Author  : Tom Stevelt
	Date    : 2019 - 2024
	Synopsis: Get command line args
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

#include	"SyncPortfolio.h"

static void Usage ( char *Msg )
{
	if ( Msg && nsStrlen ( Msg ) > 0 )
	{
		printf ( "%s\n", Msg );
	}

	printf ( "USAGE: SyncPortfolio  MemberID  BrokerID  Filename  [-d]\n" );
	printf ( "  BrokerID can be either FID, SCHW or IBKR\n" );
	exit ( 1 );
}

void getargs ( int argc, char *argv[] )
{
	int		xa;

	if ( argc < 4 )
	{
		Usage ( NULL );
	}

	xmember.xid = nsAtol ( argv[1] );
	BrokerID    = argv[2];
	Filename    = argv[3];

	if ( xmember.xid == 0 )
	{
		Usage ( "Bad member ID" );
	}

	if ( strstr ( " FID SCHW IBKR ", BrokerID ) == NULL )
	{
		Usage ( "Bad broker ID" );
	}

	if ( access ( Filename, R_OK ) != 0 )
	{
		Usage ( "Cannot access file" );
	}

	for ( xa = 4; xa < argc; xa++ )
	{
		if ( nsStrcmp ( argv[xa], "-d" ) == 0 )
		{
			Debug = 1;
		}
		else
		{
			Usage ( "Unknown arg" );
		}
	}
}
