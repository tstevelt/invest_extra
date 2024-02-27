/*----------------------------------------------------------------------------
	Program : PortfolioAlert/EachPortfolio.c
	Author  : Tom Stevelt
	Date    : 2019 - 2024
	Synopsis: For each portfolio record with an alert, see if alert triggers
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

#include	"PortfolioAlert.h"

extern	double		xstock_ttm_dividends;

int EachPortfolio ()
{
	int		rv;
	int		KeepIt, Count, Index, ndx;
	double	StockStartPrice, StockCurrentPrice, StockPercent;
	double	BenchmarkStartPrice, BenchmarkCurrentPrice, BenchmarkPercent;
	DATEVAL	BoughtDateval, CurrentDateval;
	int		OwnedDays;
	double	OwnedYears;
	double	AnnualYield, TotalDividends, TotalYield;
	double	Direction, CompareTo, Difference;

	/*----------------------------------------------------------
		using portfolio price instead of one historical price,
		allows for multiple purchase date at various prices.
	----------------------------------------------------------*/
	StockStartPrice = xportfolio.xpprice;

	sprintf ( WhereClause, "Sticker = '%s' and Stype != '%c' and Stype != '%c'", 
			xportfolio.xpticker, STYPE_CRYPTO, STYPE_BOND );

	if (( rv = LoadStock ( &MySql, WhereClause, &xstock, 0, 1 )) != 1 )
	{
		if ( Debug )
		{
			printf ( "LoadStock %s returned %d\n", xportfolio.xpticker, rv );
		}
		return ( 0 );
	}

	if ( xstock.xslast == NULL || nsStrncmp ( xstock.xslast, "(null)", 6 ) == 0 )
	{
		printf ( "Stock %s does not have any history\n", xportfolio.xpticker );
		return ( 0 );
	}

	StrToDatevalFmt ( xportfolio.xpdate, DATEFMT_YYYY_MM_DD, &BoughtDateval );
	StrToDatevalFmt ( xstock.xslast,     DATEFMT_YYYY_MM_DD, &CurrentDateval );
	OwnedDays = (int) DateDiff ( &BoughtDateval, &CurrentDateval );
	OwnedYears = (double) OwnedDays / 365.0;
	AnnualYield = 100.0 * xstock_ttm_dividends / xportfolio.xpprice;	

	TotalDividends = LoadDividendSum ( &MySql, xportfolio.xpticker, xportfolio.xpdate );
	TotalYield = 100.0 * TotalDividends / xportfolio.xpprice;

	if ( Debug )
	{
		printf ( "%s: owned %d days %.2f years, annual %.2f total %.2f\n", 
			xportfolio.xpticker, OwnedDays, OwnedYears, AnnualYield, TotalYield );
	}

	/*----------------------------------------------------------
		get stock current price
	----------------------------------------------------------*/
	sprintf ( WhereClause, "Hticker = '%s' and Hdate = '%s'", xportfolio.xpticker, xstock.xslast );
	if (( rv = LoadHistory ( &MySql, WhereClause, &xhistory, 0 )) != 1 )
	{
		if ( Debug )
		{
			printf ( "LoadHistory returned %d, cannot find stock price for %s %s\n", rv, xportfolio.xpticker, xstock.xslast );
		}
		return ( 0 );
	}
	StockCurrentPrice = xhistory.xhclose;

	StockPercent = 100.0 * ( StockCurrentPrice - StockStartPrice ) / StockStartPrice;

	KeepIt = 0;
	switch ( xportfolio.xpalerttype[0] )
	{
		case ALERT_ABOVE_MA5:
		case ALERT_BELOW_MA5:
		case ALERT_ABOVE_MA10:
		case ALERT_BELOW_MA10:
		case ALERT_ABOVE_MA20:
		case ALERT_BELOW_MA20:
		case ALERT_ABOVE_MA50:
		case ALERT_BELOW_MA50:
		case ALERT_BELOW_MA100:
		case ALERT_ABOVE_MA100:
		case ALERT_ABOVE_MA200:
		case ALERT_BELOW_MA200:
			/*----------------------------------------------------------
				compare current stock price to 50 day moving average
				when custom is implemented, the can use other # of days.
			----------------------------------------------------------*/
			sprintf ( WhereClause, "Aticker = '%s' and Adate = '%s'", xportfolio.xpticker, xstock.xslast );
			LoadAverage ( &MySql, WhereClause, &xaverage, 0 );
			break;

		case ALERT_MA5_DECREASE:
		case ALERT_MA10_DECREASE:
		case ALERT_MA20_DECREASE:
		case ALERT_MA50_DECREASE:
		case ALERT_MA5_INCREASE:
		case ALERT_MA10_INCREASE:
		case ALERT_MA20_INCREASE:
		case ALERT_MA50_INCREASE:
			LoadHistoryArray ( &MySql, xstock.xsticker, xstock.xslast, SORT_DESCEND, 10 );
			switch ( xportfolio.xpalerttype[0] )
			{
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
			}
			switch ( xportfolio.xpalerttype[0] )
			{
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
							return ( 0 );
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
							return ( 0 );
							break;
						}
					}
					break;
			}
			break;

		case ALERT_BENCHMARK:
			/*----------------------------------------------------------
				get benchmark start price
			----------------------------------------------------------*/
			sprintf ( WhereClause, "Hticker = '%s' and Hdate = '%s'", xportfolio.xpbenchmark, xportfolio.xpdate );
			if (( rv = LoadHistory ( &MySql, WhereClause, &xhistory, 0 )) != 1 )
			{
				BenchmarkStartPrice = xportfolio.xpprice;
			}
			else
			{
				BenchmarkStartPrice = xhistory.xhclose;
			}

			/*----------------------------------------------------------
				get benchmark current price
			----------------------------------------------------------*/
			sprintf ( WhereClause, "Hticker = '%s' and Hdate = '%s'", xportfolio.xpbenchmark, xstock.xslast );
			if (( rv = LoadHistory ( &MySql, WhereClause, &xhistory, 0 )) != 1 )
			{
				if ( Debug )
				{
					printf ( "LoadHistory returned %d, cannot find benchmark price for %s %s\n", 
							rv, xportfolio.xpticker, xstock.xslast );
				}
				return ( 0 );
			}
			BenchmarkCurrentPrice = xhistory.xhclose;

			BenchmarkPercent = 100.0 * ( BenchmarkCurrentPrice - BenchmarkStartPrice ) / BenchmarkStartPrice;
			
			break;
	}

	CompareTo = 0.0;
	Direction = 1;
	switch ( xportfolio.xpalerttype[0] )
	{
		case ALERT_ABOVE_AMT:
			Direction = -1;
		case ALERT_BELOW_AMT:
			CompareTo = xportfolio.xpalertprice;
			break;

		case ALERT_TRAIL_STOP:
			CompareTo = xportfolio.xptrailvalue;
			break;

		case ALERT_BENCHMARK:
			CompareTo = BenchmarkPercent - xportfolio.xpalert; 
			StockCurrentPrice = (StockPercent + TotalYield);
			if ( Debug )
			{
				printf ( "%s Benchmark ROI %.2f - %.2f = %.2f Stock ROI %.2f + %.2f = %.2f\n", 
						xportfolio.xpticker, 
						BenchmarkPercent, xportfolio.xpalert, CompareTo, 
						StockPercent, TotalYield, StockCurrentPrice );
			}
			break;

		case ALERT_ABOVE_MA5:
			Direction = -1;
		case ALERT_BELOW_MA5:
			CompareTo = xaverage.xama5;
			break;

		case ALERT_ABOVE_MA10:
			Direction = -1;
		case ALERT_BELOW_MA10:
			CompareTo = xaverage.xama10;
			break;

		case ALERT_ABOVE_MA20:
			Direction = -1;
		case ALERT_BELOW_MA20:
			CompareTo = xaverage.xama20;
			break;

		case ALERT_ABOVE_MA50:
			Direction = -1;
		case ALERT_BELOW_MA50:
			CompareTo = xaverage.xama50;
			break;

		case ALERT_BELOW_MA100:
			Direction = -1;
		case ALERT_ABOVE_MA100:
			CompareTo = xaverage.xama100;
			break;

		case ALERT_ABOVE_MA200:
			Direction = -1;
		case ALERT_BELOW_MA200:
			CompareTo = xaverage.xama200;
			break;
	}

	if ( KeepIt == 0 && CompareTo == 0.0 )
	{
		return ( 0 );
	}

	Difference = Direction * ( StockCurrentPrice - CompareTo );
	if ( KeepIt == 0 && Difference >= 0.0 )
	{
		return ( 0 );
	}

	MemberCount++;
	if ( MemberCount == 1 )
	{
		sprintf ( OutFileName, "%s/portfolio_%d.dat", TEMPDIR, getpid() );
		if (( fpOutput = fopen ( OutFileName, "w" )) == (FILE *) 0 )
		{
			fprintf ( stderr, "Cannot create %s for output\n", OutFileName );
			exit ( 1 );
		}

		fprintf ( fpOutput, "<html>\n" );
		fprintf ( fpOutput, "<head>\n" );
		fprintf ( fpOutput, "<style>\n" );
		fprintf ( fpOutput, ".heading { color: white; background-color: black; }\n" );
		fprintf ( fpOutput, ".odd { background-color: powderblue; }\n" );
		fprintf ( fpOutput, ".even { background-color: linen; }\n" );
		fprintf ( fpOutput, ".bench { background-color: lightyellow; }\n" );
		fprintf ( fpOutput, "</style>\n" );
		fprintf ( fpOutput, "</head>\n" );
		fprintf ( fpOutput, "<body>\n" );
		fprintf ( fpOutput, "<table border='1' cellpadding='3' cellspacing='0'>" );

		fprintf ( fpOutput, "<tr class='heading'>" );
		fprintf ( fpOutput, "<td>TICKER</td>" );
		fprintf ( fpOutput, "<td>COMPANY</td>" );
		fprintf ( fpOutput, "<td align='right'>CURRENT</td>" );
		fprintf ( fpOutput, "<td align='left'>ALERT</td>" );
		fprintf ( fpOutput, "<td align='right'>VALUE</td>" );
#ifdef SHOW_OPTION_EXPIRATION
		fprintf ( fpOutput, "<td align='right'>OPTION</td>" );
#endif
		fprintf ( fpOutput, "</tr>\n" );
	}

	fprintf ( fpOutput, "<tr class='%s'>\n", MemberCount % 2 ? "odd" : "even" );
	fprintf ( fpOutput, "<td>%s</td>", xportfolio.xpticker );
	fprintf ( fpOutput, "<td>%s</td>", xstock.xsname );
	fprintf ( fpOutput, "<td class='%s'>%.2f</td>", 
			xportfolio.xpalerttype[0] == ALERT_BENCHMARK ? "bench" : MemberCount % 2 ? "odd" : "even", StockCurrentPrice );

	if ( xportfolio.xpalerttype[0] == ALERT_BENCHMARK )
	{
		fprintf ( fpOutput, "<td>%s (%s)</td>", AlertDescription(xportfolio.xpalerttype[0]), xportfolio.xpbenchmark );
		SawBenchmark++;
	}
	else
	{
		fprintf ( fpOutput, "<td>%s</td>", AlertDescription (  xportfolio.xpalerttype[0] ) );
	}

// tms 06/29/2021	if ( CompareTo > 0.0 )
	{
		fprintf ( fpOutput, "<td class='%s'>%.2f</td>", 
			xportfolio.xpalerttype[0] == ALERT_BENCHMARK ? "bench" : MemberCount % 2 ? "odd" : "even", CompareTo );
	}
// tms 06/29/2021	else
// tms 06/29/2021	{
// tms 06/29/2021		fprintf ( fpOutput, "<td class='%s'>&nbsp;</td>", 
// tms 06/29/2021			xportfolio.xpalerttype[0] == ALERT_BENCHMARK ? "bench" : MemberCount % 2 ? "odd" : "even" );
// tms 06/29/2021	}

#ifdef SHOW_OPTION_EXPIRATION
	if ( xportfolio.xpoptexp[0] > '0' )
	{
		fprintf ( fpOutput, "<td>%s</td>", xportfolio.xpoptexp );
	}
	else
	{
		fprintf ( fpOutput, "<td>&nbsp;</td>" );
	}
#endif

	fprintf ( fpOutput, "</tr>\n" );

	return ( 0 );
}
