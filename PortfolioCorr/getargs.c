/*----------------------------------------------------------------------------
	Program : PortfolioCorr/getargs.c
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

#include	"PortfolioCorr.h"

static void Usage ()
{
	printf ( "\n" );
	printf ( "Print array for correlation eval by python script Corr.py\n" );
	printf ( "Reads from portfolio, watchlist, temp table or file\n" );
	printf ( "\n" );
	printf ( "USAGE: PortfolioCorr {P|W|T|F file} -member # [-d]\n" );
	printf ( " -d - debug\n" );
	exit ( 1 );
}

void getargs ( int argc, char *argv[] )
{
	int		xa, xs;

	if ( argc < 2 )
	{
		Usage ();
	}
	
	ReportMember = 0;
	ReportTable = '?';
	Debug = 0;

	switch ( argv[1][0] )
	{
		case 'P':
		case 'p':
		case 'W':
		case 'w':
		case 'T':
		case 't':
			ReportTable = toupper(argv[1][0] );
			xs = 2;
			break;
		case 'F':
		case 'f':
			if ( argc < 3 )
			{
				Usage ();
			}
			ReportTable = toupper(argv[1][0] );
			Filename = argv[2];
			xs = 3;
			break;
		default:
			Usage ();
			break;
	}

	for ( xa = xs; xa < argc; xa++ )
	{
		if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-member" ) == 0 )
		{
			xa++;
			ReportMember = nsAtol ( argv[xa] );
		}
		else if ( nsStrcmp ( argv[xa], "-d" ) == 0 )
		{
			Debug = 1;
		}
		else
		{
			printf ( "Unknown arg\n" );
			exit ( 1 );
		}
	}

	if ( ReportMember == 0 )
	{
		Usage ();
	}
}
