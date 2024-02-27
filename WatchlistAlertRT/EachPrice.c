/*----------------------------------------------------------------------------
	Program : WatchlistAlertRT/EachPrice.c
	Author  : Tom Stevelt
	Date    : 2019 - 2024
	Synopsis: see if alert triggered.
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

#include	"WatchlistAlertRT.h"

static	char	PreviousTicker[21];
static	int		FirstPass = 1;
static	double	LatestPrice = 0.0;
static	double	LowPrice = 0.0;
static	double	HighPrice = 0.0;

int EachPrice ()
{
	double	AlertPrice;
	int		xh, xf, KeepIt;
	char	cmdline[1024];
	FILE	*fp;
	char	xbuffer[1024];
	char	*tokens[100];
	char	*Tags[100];
	int		tokcnt;

	if ( FirstPass == 1 )
	{
		FirstPass = 0;
		snprintf ( PreviousTicker, sizeof(PreviousTicker), "zzzzzzzzzzzzzzzzzzzz" );
	}

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
		default:
			break;
	}

	if ( nsStrcmp ( PreviousTicker, xstock.xsticker ) != 0 )
	{
		printf ( "Getting Quote for %s\n", xstock.xsticker );

		snprintf ( PreviousTicker, sizeof(PreviousTicker), "%s", xstock.xsticker );
		LatestPrice = 0.0;
		LowPrice = 0.0;
		HighPrice = 0.0;

		sprintf ( cmdline, 
			"curl -s '%s/stock/%s/quote?format=json&token=%s' > %s",
					env_ApiURL, xstock.xsticker, env_ApiKey, TempFileName );

		IEX_RateLimit ( 0 );

		/*----------------------------------------------------------
			fixit - still using curl system() instead of library.
		----------------------------------------------------------*/

		system ( cmdline );

		if (( fp = fopen ( TempFileName, "r" )) == (FILE *)0 )
		{
			printf ( "Cannot open %s\n", TempFileName );
			return ( 0 );
		}
		else
		{
			if ( fgets ( xbuffer, sizeof(xbuffer), fp ) == (char *)0 )
			{
				printf ( "Empty file %s\n", TempFileName );
				nsFclose ( fp );
				return ( 0 );
			}
			else if (( tokcnt = JsonTokens ( xbuffer, Tags, tokens, 100 )) < 15 )
			{
				printf ( "Token count %d\n", tokcnt );
				nsFclose ( fp );
				return ( 0 );
			}
			else
			{
				for ( xh = 0, xf = 0; xh < tokcnt && xf < 3; xh++ )
				{
					if ( Debug )
					{
						printf ( "%s = %s\n", Tags[xh], tokens[xh] );
					}

					if ( nsStrcmp ( Tags[xh], "latestPrice" ) == 0 )
					{
						LatestPrice = nsAtof(tokens[xh]);
						xf++;
						printf ( " Latest %.2f\n", LatestPrice );
					}
					else if ( nsStrcmp ( Tags[xh], "low" ) == 0 )
					{
						LowPrice = nsAtof(tokens[xh]);
						xf++;
						printf ( " Low %.2f\n", LowPrice );
					}
					else if ( nsStrcmp ( Tags[xh], "high" ) == 0 )
					{
						HighPrice = nsAtof(tokens[xh]);
						xf++;
						printf ( " High %.2f\n", HighPrice );
					}
				}

				if ( LowPrice == 0 || LatestPrice < LowPrice )
				{
					LowPrice = LatestPrice;
				}
				if ( HighPrice == 0 || LatestPrice > HighPrice )
				{
					HighPrice = LatestPrice;
				}
			}
			nsFclose ( fp );
		}
	}

	if ( LatestPrice < 0.01 )
	{
		printf ( "  No latest price for %s\n", xstock.xsticker );
		return ( 0 );
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
			if ( HighPrice > AlertPrice )
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
			if ( LowPrice < AlertPrice )
			{
				KeepIt = 2;
			}
			break;
	}

	if ( KeepIt )
	{
		sprintf ( cmdline, "echo '%s %s %s %.2f %s: %.2f' | mail -s 'ALERT ON %s' %s",
				xwatchlist.xwticker, xstock.xsname,  
				AlertDescription ( xwatchlist.xwalerttype[0] ),
				AlertPrice,
				KeepIt == 1 ? "High" : "Low", 
				KeepIt == 1 ? HighPrice : LowPrice, 
				xwatchlist.xwticker,
				xmember.xmpager );
		
		ReportCount++;

		if ( Debug )
		{
			printf ( "%s\n", cmdline );
		}
		else
		{
			system ( cmdline );
		}
			
		MemberCount++;
	}

	return ( 0 );
}
