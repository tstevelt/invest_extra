/*----------------------------------------------------------------------------
	Program : rpt_prospect.c
	Author  : Tom Stevelt
	Date    : 03-01-2023
	Synopsis: Compare portfolio holding to range of other stocks to find
			  better prospects.
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
#include	"rpt_prospect.h"

static int cmpgrow5 ( PORT_RECORD *a, PORT_RECORD *b )
{
	if ( a->Grow5 < b->Grow5 )
	{
		return ( -1 );
	}
	if ( a->Grow5 > b->Grow5 )
	{
		return ( 1 );
	}
	return ( 0 );
}

int cmpticker ( PORT_RECORD *a, PORT_RECORD *b )
{
	return ( strcmp(a->Ticker,b->Ticker) );
}

int main ( int argc, char *argv[] )
{
	char	xbuffer[1024];
	char	*AttachArray[1];
	int		rv;

	//printf ( "sizeof long %ld\n", sizeof(long) );
	//printf ( "sizeof int  %ld\n", sizeof(int) );
	//exit ( 1 );

	getargs ( argc, argv );

	StartMySQL ( &MySql, "invest" );

	sprintf ( WhereClause, "member.id = %ld", ReportMember );
	if ( LoadMember ( &MySql, WhereClause, &xmember, 0 ) < 1 )
	{
		printf ( "Unknown member id %ld\n", ReportMember );
		exit ( 1 );
	}

	sprintf ( WhereClause, "Pmember = %ld", ReportMember );
	LoadPortfolioCB ( &MySql, WhereClause, "Pticker", &xportfolio, (int(*)()) EachPortfolio, 1 );

	if ( Debug )
	{
		qsort ( PortArray, PortCount, sizeof(PORT_RECORD), (int(*)()) cmpgrow5 );

		for ( rv = 0; rv < PortCount; rv++ )
		{
			printf ( "%-20.20s %7.3f %16ld %6.2f\n", 
				PortArray[rv].Ticker, 
				PortArray[rv].Grow5,
				PortArray[rv].EntVal,
				PortArray[rv].Ratio );
		}

		printf ( "Min Value %16ld\n", MinEntValue );
		printf ( "Max Value %16ld\n", MaxEntValue );
	}

	qsort ( PortArray, PortCount, sizeof(PORT_RECORD), (int(*)()) cmpticker );

	sprintf ( WhereClause, "(Stype = 'S' or Stype = 'A')" );
	switch ( StockIndex )
	{
		case 'D':
			nsStrcat ( WhereClause, " and Sdj = 'Y'" );
			break;
		case 'S':
			nsStrcat ( WhereClause, " and Ssp500 = 'Y'" );
			break;
		case 'N':
			nsStrcat ( WhereClause, " and Snasdaq = 'Y'" );
			break;
		case '1':
			nsStrcat ( WhereClause, " and Srussell = '1'" );
			break;
		case '2':
			nsStrcat ( WhereClause, " and Srussell = '2'" );
			break;
		case '3':
			nsStrcat ( WhereClause, " and (Srussell = '1' or Srussell = '2')" );
			break;
	}
	LoadStockCB ( &MySql, WhereClause, NULL, &xstock, (int(*)()) EachStock, 0 );

	for ( int xp = 0; xp < PortCount; xp++ )
	{
		if ( PortArray[xp].Flag == 1 )
		{
			continue;
		}

		sprintf ( WhereClause, "Sticker = '%s'", PortArray[xp].Ticker );
		LoadStockCB ( &MySql, WhereClause, NULL, &xstock, (int(*)()) EachStock, 0 );
	}

	if ( ReportCount == 0 )
	{
		return ( 0 );
	}

	if ( Debug )
	{
		nsFclose ( fpOutput );
		sprintf ( xbuffer, "cat %s", OutFileName );
		system ( xbuffer );
		return ( 0 );
	}
	nsFclose ( fpOutput );

	sortdata ( OutFileName );

	switch ( ReportOptions.Format )
	{
		case RPT_FORMAT_APPEND_W:
		case RPT_FORMAT_REPLACE_W:
			rv = ToWatchlist ( &MySql, OutFileName, "|", 1, ReportOptions.Format, ReportMember, 1 );
			if ( rv < 0 )
			{
				printf ( "ToWatchlist failed, rv %d\n", rv );
			}
			else if ( rv == 0 )
			{
				printf ( "Nothing added to watchlist\n" );
			}
			else
			{
				printf ( "Added %d stocks to watchlist\n", rv );
			}
			break;
		case RPT_FORMAT_TEMP:
			rv = ToTemp ( &MySql, OutFileName, "|", 1, ReportOptions.Format, ReportMember, 1 );
			if ( rv < 0 )
			{
				printf ( "ToTemp failed, rv %d\n", rv );
			}
			else if ( rv == 0 )
			{
				printf ( "Nothing added to temp file\n" );
			}
			else
			{
				printf ( "Added %d stocks to temp file\n", rv );
			}
			break;
		default:
			dorpt_base ( OutFileName );
			break;
	}

	switch ( ReportOptions.Format )
	{
		case RPT_FORMAT_CSV:
		case RPT_FORMAT_EXCEL:
		case RPT_FORMAT_TEXT:
		case RPT_FORMAT_PDF_VIEW:
			snprintf ( xbuffer, sizeof(xbuffer), "Prospect Report - %d stocks", ReportCount);
			AttachArray[0] = ReportOptions.OutputFilename;
			SendAttached ( REPORT_ADDRESS, xmember.xmemail, "", "", xbuffer, 0, NULL, 1, AttachArray );
			printf ( "Sending %s to %s\n", ReportOptions.OutputFilename, xmember.xmemail );
			break;
		case RPT_FORMAT_EMAIL:
			snprintf ( xbuffer, sizeof(xbuffer), "Prospect Report - %d stocks", ReportCount);
			SendAttached ( REPORT_ADDRESS, xmember.xmemail, "", "", xbuffer, 1, ReportOptions.OutputFilename, 0, AttachArray );
			printf ( "Sending %s to %s\n", ReportOptions.OutputFilename, xmember.xmemail );
			break;
		case RPT_FORMAT_HTML:
			snprintf ( xbuffer, sizeof(xbuffer), "cat %s", ReportOptions.OutputFilename );
			system ( xbuffer );
			break;
	}
		
//	unlink ( OutFileName );
//	unlink ( ReportOptions.OutputFilename );

	return ( 0 );
}
