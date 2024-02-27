/*----------------------------------------------------------------------------
	Program : WatchlistAlert/EachPrice.c
	Author  : Tom Stevelt
	Date    : 2019 - 2024
	Synopsis: for a stock in the watchlist, see if alert triggered.
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

#include	"WatchlistAlert.h"

int EachPrice ()
{
	double	AverageVolume, VolumePercent, AlertPrice;
	int		ndx, KeepIt, Index, Count;

	sprintf ( WhereClause, "Sticker = '%s'", xwatchlist.xwticker );
	if ( LoadStock ( &MySql, WhereClause, &xstock, 0, 0 ) != 1 )
	{
		return ( 0 );
	}

	LoadHistoryArray ( &MySql, xstock.xsticker, xstock.xslast, SORT_DESCEND, 10 );
	if ( HistoryCount == 0 )
	{
		return ( 0 );
	}

	if ( Debug )
	{
		printf ( "%s %s %d\n", xstock.xsticker, xstock.xslast, HistoryCount );
	}

	Count = 0;
	AlertPrice = 0.0;
	switch ( xwatchlist.xwalerttype[0] )
	{
		case ALERT_NONE:
			return ( 0 );
		case ALERT_ABOVE_AMT:
		case ALERT_BELOW_AMT:
			if ( xwatchlist.xwalertprice < 0.01 )
			{
				return ( 0 );
			}
			AlertPrice = xwatchlist.xwalertprice;
			break;
		case ALERT_ABOVE_MA5:
		case ALERT_BELOW_MA5:
			AlertPrice = HistoryArray[0].Average[AVG_5];
			break;
		case ALERT_ABOVE_MA10:
		case ALERT_BELOW_MA10:
			AlertPrice = HistoryArray[0].Average[AVG_10];
			break;
		case ALERT_ABOVE_MA20:
		case ALERT_BELOW_MA20:
			AlertPrice = HistoryArray[0].Average[AVG_20];
			break;
		case ALERT_ABOVE_MA50:
		case ALERT_BELOW_MA50:
			AlertPrice = HistoryArray[0].Average[AVG_50];
			break;
		case ALERT_ABOVE_MA100:
		case ALERT_BELOW_MA100:
			AlertPrice = HistoryArray[0].Average[AVG_100];
			break;
		case ALERT_ABOVE_MA200:
		case ALERT_BELOW_MA200:
			AlertPrice = HistoryArray[0].Average[AVG_200];
			break;
		case ALERT_MA5_DECREASE:
		case ALERT_MA5_INCREASE:
			Count = 2;
			Index = AVG_5;
			break;
		case ALERT_MA10_DECREASE:
		case ALERT_MA10_INCREASE:
			Count = 3;
			Index = AVG_10;
			break;
		case ALERT_MA20_DECREASE:
		case ALERT_MA20_INCREASE:
			Count = 6;
			Index = AVG_20;
			break;
		case ALERT_MA50_DECREASE:
		case ALERT_MA50_INCREASE:
			Count = 9;
			Index = AVG_50;
			break;
		default:
			break;
	}

	KeepIt = 0;
	switch ( xwatchlist.xwalerttype[0] )
	{
		case ALERT_ABOVE_AMT:
		case ALERT_ABOVE_MA5:
		case ALERT_ABOVE_MA10:
		case ALERT_ABOVE_MA20:
		case ALERT_ABOVE_MA50:
		case ALERT_ABOVE_MA100:
		case ALERT_ABOVE_MA200:
			if ( HistoryArray[0].Close > AlertPrice )
			{
				KeepIt = 1;
			}
			break;

		case ALERT_BELOW_AMT:
		case ALERT_BELOW_MA5:
		case ALERT_BELOW_MA10:
		case ALERT_BELOW_MA20:
		case ALERT_BELOW_MA50:
		case ALERT_BELOW_MA100:
		case ALERT_BELOW_MA200:
			if ( HistoryArray[0].Close < AlertPrice )
			{
				KeepIt = 1;
			}
			break;

		case ALERT_MA5_DECREASE:
		case ALERT_MA10_DECREASE:
		case ALERT_MA20_DECREASE:
		case ALERT_MA50_DECREASE:
			KeepIt = 1;
			for ( ndx = 0; ndx < Count; ndx++ )
			{
#ifdef DEBUG
printf ( "%s %.2f %.2f\n", xwatchlist.xwticker,  HistoryArray[ndx].Average[Index], HistoryArray[ndx+1].Average[Index] );
#endif

				if ( HistoryArray[ndx].Average[Index] > HistoryArray[ndx+1].Average[Index] )
				{
					KeepIt = 0;
					break;
				}
			}
			break;

		case ALERT_MA5_INCREASE:
		case ALERT_MA10_INCREASE:
		case ALERT_MA20_INCREASE:
		case ALERT_MA50_INCREASE:
			KeepIt = 1;
			for ( ndx = 0; ndx < Count; ndx++ )
			{
				if ( HistoryArray[ndx].Average[Index] < HistoryArray[ndx+1].Average[Index] )
				{
					KeepIt = 0;
					break;
				}
			}
			break;
	}

	if ( KeepIt )
	{
		for ( ndx = 0, AverageVolume = 0; ndx < HistoryCount; ndx++ )
		{
			AverageVolume += HistoryArray[ndx].Volume;
		}
		AverageVolume /= (double) HistoryCount;
		VolumePercent = 100.0 * HistoryArray[0].Volume / AverageVolume;

		if ( MemberCount == 0 )
		{
			sprintf ( OutFileName, "%s/WatchlistAlert_%ld.html", TEMPDIR, xwatchlist.xwmember );
			if (( fpOutput = fopen ( OutFileName, "w" )) == (FILE *)0 )
			{
				printf ( "Cannot create %s for output\n", OutFileName );
				exit ( 1 );
			}

			fprintf ( fpOutput, "<html>\n" );
			fprintf ( fpOutput, "<head>\n" );
			fprintf ( fpOutput, "<style>\n" );
			fprintf ( fpOutput, ".title { color: black; background-color: lightgray; }\n" );
			fprintf ( fpOutput, ".heading { color: white; background-color: black; }\n" );
			fprintf ( fpOutput, ".odd { background-color: powderblue; }\n" );
			fprintf ( fpOutput, ".even { background-color: linen; }\n" );
			fprintf ( fpOutput, "</style>\n" );
			fprintf ( fpOutput, "</head>\n" );
			fprintf ( fpOutput, "<body>\n" );
			fprintf ( fpOutput, "<table>\n" );
			fprintf ( fpOutput, "<tr class='title'><td colspan='5'>%s</td></tr>\n", xmember.xmname );
			fprintf ( fpOutput, "<tr class='title'><td colspan='5'>Alert Triggered on Watchlist Stocks </td></tr>\n" );
			fprintf ( fpOutput, "<tr class='heading'><td>TICKER</td><td>COMPANY</td><td>CLOSE</td><td>ALERT</td><td>VOL-PCT</td></tr>\n" );
		}

		fprintf ( fpOutput, "<tr class='%s'><td>%-10.10s</td><td>%-20.20s</td>",
				MemberCount % 2 ? "odd" : "even",
				xwatchlist.xwticker, 
				xstock.xsname );

		fprintf ( fpOutput, "<td align='right'>%8.2f</td>",
				HistoryArray[0].Close );

		if ( AlertPrice > 0.0 )
		{
			fprintf ( fpOutput, "<td>%s %8.2f</td>",
				AlertDescription ( xwatchlist.xwalerttype[0] ),
				AlertPrice );
		}
		else
		{
			fprintf ( fpOutput, "<td>%s</td>",
				AlertDescription ( xwatchlist.xwalerttype[0] ) );
		}

		fprintf ( fpOutput, "<td align='right'>%.2f</td></tr>\n", 
				VolumePercent );

		MemberCount++;
	}

	return ( 0 );
}
