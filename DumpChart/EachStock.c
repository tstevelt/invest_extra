/*----------------------------------------------------------------------------
	Program : DumpChart/EachStock.c
	Author  : Tom Stevelt
	Date    : 2019 - 2024
	Synopsis: print one line of data
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

#include	"DumpChart.h"

int EachStock ()
{
	int		ndx, xh;
	XSTOCK	xbenchmark;

	/*----------------------------------------------------------
		load history for S&P, in order to calculate RSI
		copy into BenchmarkArray.
	----------------------------------------------------------*/
	sprintf ( WhereClause, "Sticker = 'SPY'" );
	if ( LoadStock ( &MySql, WhereClause, &xbenchmark, 0, 0 ) != 1 )
	{
		printf ( "Cannot load stock for SPY\n" );
		exit ( 0 );
	}
	
	if ( nsStrcmp ( xbenchmark.xslast, xstock.xslast ) != 0 )
	{
		printf ( "Last Dates don't match\n" );
		return ( 1 );
	}

	LoadHistoryArray ( &MySql, xbenchmark.xsticker, xbenchmark.xslast, SORT_ASCEND, 252 );

	memcpy ( BenchmarkArray, HistoryArray, HistoryCount * sizeof(HISTORY_RECORD) );
	BenchmarkCount = HistoryCount;

	/*----------------------------------------------------------
		now load history for this stock
	----------------------------------------------------------*/
	HistoryCount = 0;
	memset ( HistoryArray, '\0', sizeof(HistoryArray) );
	LoadHistoryArray ( &MySql, xstock.xsticker, xstock.xslast, SORT_ASCEND, 252 );

	if ( HistoryCount != BenchmarkCount )
	{
		printf ( "History (%d) and Benchmark (%d) counts don't match\n", HistoryCount, BenchmarkCount );
		return ( 1 );
	}
	if ( nsStrcmp ( HistoryArray[0].Date, BenchmarkArray[0].Date ) != 0 )
	{
		printf ( "History (%s) and Benchmark (%s) dates don't match\n", HistoryArray[0].Date, BenchmarkArray[0].Date );
		return ( 1 );
	}

	printf ( "%s,%s\n", xstock.xsticker, xstock.xsname );

	printf ( "DATE," );
	printf ( "CLOSE," );
	printf ( "MA_5," );
	printf ( "MA_10," );
	printf ( "MA_20," );
	printf ( "MA_50," );
	printf ( "MA_100," );
	printf ( "MA_200," );
	printf ( "VOLUME," );
	printf ( "AVG-VOL," );
	printf ( "RSI," );
	printf ( "SPY" );

	printf ( "\n" );
	
	for ( ndx = 0; ndx < HistoryCount; ndx++ )
	{
		printf ( "%s,", HistoryArray[ndx].Date );
		printf ( "%.2f,", HistoryArray[ndx].Close );
		for ( xh = 0; xh < 6; xh++ )
		{
			printf ( "%.2f,", HistoryArray[ndx].Average[xh] );
		}
		printf ( "%ld,", HistoryArray[ndx].Volume );
		printf ( "%ld,", HistoryArray[ndx].AvgVol );
		printf ( "%.2f,", HistoryArray[ndx].RSI );

		printf ( "%.2f", BenchmarkArray[ndx].Close );

		printf ( "\n" );
	}

	return ( 0 );
}
