/*----------------------------------------------------------------------------
	Program : rpt_performance/getargs.c
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

#include	"rpt_performance.h"

static void Usage ()
{
	printf ( "USAGE: rpt_performance -member id [options]\n" );
	printf ( "options:\n" );
	printf ( " -benchmark ticker (SPY is default)\n" );
	printf ( " -limit #.#        (8 percent is default)\n" );
	printf ( " -sort {ticker*|gain|roi|compared|}\n" );
	printf ( " -fmt {csv*|txt|html|email|others not available\n" );
	printf ( " -d   = debug, prints alert or report, does not send email\n" );
	printf ( " * default value\n" );
	exit ( 1 );
}

void getargs ( int argc, char *argv[] )
{
	int		xa;

	ReportMember = 0;

	if ( argc < 3 )
	{
		Usage ();
	}
	else if ( nsStrcmp ( argv[1], "-member" ) == 0 )
	{
	 	ReportMember = nsAtol ( argv[2] );
		if ( ReportMember == DEMO_MEMBER )
		{
			srand ( time ( NULL ) );
			MungeData = 1;
		}
	}
	else
	{
		Usage ();
	}
	
	ReportOptions.Format = RPT_FORMAT_CSV;
	RunMode = MODE_REPORT;
	SortBy = SORTBY_TICKER;
	BenchmarkTicker = "SPY";
	AlertTrigger = -8.0;

	for ( xa = 3; xa < argc; xa++ )
	{
		if ( nsStrcmp ( argv[xa], "-d" ) == 0 )
		{
			Debug = 1;
		}
		else if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-fmt" ) == 0 )
		{
			xa++;
			ReportOptions.Format = ValidReportFormatUsage ( argv[xa], (int(*)()) Usage );
		}
		else if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-sort" ) == 0 )
		{
			xa++;
			if ( nsStrcmp ( argv[xa], "ticker" ) == 0 )
			{
				SortBy = SORTBY_TICKER;
			}
			else if ( nsStrcmp ( argv[xa], "gain" ) == 0 )
			{
				SortBy = SORTBY_GAIN;
			}
			else if ( nsStrcmp ( argv[xa], "roi" ) == 0 )
			{
				SortBy = SORTBY_RETURN;
			}
			else if ( nsStrcmp ( argv[xa], "compared" ) == 0 )
			{
				SortBy = SORTBY_COMPARED;
			}
			else
			{
				printf ( "Unknown -sort\n" );
				Usage ();
			}
		}
		else if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-benchmark" ) == 0 )
		{
			xa++;
			BenchmarkTicker = argv[xa];
		}
		else if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-limit" ) == 0 )
		{
			xa++;
			AlertTrigger = 0.0 - nsAtof ( argv[xa] );
		}
		else
		{
			printf ( "Unknown arg\n" );
			exit ( 1 );
		}
	}

	if ( ReportMember == 0 )
	{
		printf ( "-member is required\n" );
		Usage ();
	}

}
