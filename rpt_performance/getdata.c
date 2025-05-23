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

extern	double	xstock_ttm_dividends;

#define		PRICEFILE		"/var/local/tmp/rpt_current.json"

int EachHistory ()
{
	return ( 0 );
}

static	double	BenchmarkCurrentPrice = 0.0;

int getdata ()
{
	int		rv;
	double	StockStartPrice, StockYesterdayClose, StockPercent; // , StockLatestPrice;
	double	HighSinceValue, HighSincePercent;
	DBY_QUERY	*qryHistory;
	int		NumberOfRows;
	double	ComparedToBenchmark;
	double	BenchmarkStartPrice, BenchmarkPercent;
	double	TotalDividends, TotalYield; // AnnualYield;

	StockStartPrice = xportfolio.xpprice;

	sprintf ( WhereClause, "Sticker = '%s'", xportfolio.xpticker );
	if (( rv = LoadStock ( &MySql, WhereClause, &xstock, 0, 1 )) != 1 )
	{
		fprintf ( stderr, "LoadStock %s returned %d\n", xportfolio.xpticker, rv );
		return ( 0 );
	}

	if ( xstock.xstype[0] != STYPE_STOCK ) // if ( xstock.xstype[0] != 'S' )
	{
		if ( Debug )
		{
			printf ( "Skipping %s, type %c\n", xstock.xsticker, xstock.xstype[0] );
		}
		return ( 0 );
	}

	if ( nsStrncmp ( xstock.xslast, "(null)", 6 ) == 0 )
	{
		printf ( "Stock %s does not have any history\n", xportfolio.xpticker );
		return ( 0 );
	}

	/*----------------------------------------------------------
		get stock current price
	----------------------------------------------------------*/
	sprintf ( WhereClause, "Hticker = '%s' and Hdate = '%s'", xportfolio.xpticker, xstock.xslast );
	if (( rv = LoadHistory ( &MySql, WhereClause, &xhistory, 0 )) != 1 )
	{
		fprintf ( stderr, 
			"LoadHistory returned %d, cannot find stock price for %s %s<br>\n", rv, xportfolio.xpticker, xstock.xslast );
		return ( 0 );
	}
	StockYesterdayClose = xhistory.xhclose;

	StockPercent = 100.0 * ( StockYesterdayClose - StockStartPrice ) / StockStartPrice;

	/*----------------------------------------------------------
		get benchmark prices (SPY default)
	----------------------------------------------------------*/
	BenchmarkStartPrice = 0.0;
	BenchmarkPercent = 0.0;
	ComparedToBenchmark = 0.0;

	/*----------------------------------------------------------
		benchmark start price
	----------------------------------------------------------*/
	sprintf ( WhereClause, "Hticker = '%s' and Hdate = '%s'", BenchmarkTicker, xportfolio.xpdate );
	if (( rv = LoadHistory ( &MySql, WhereClause, &xhistory, 0 )) != 1 )
	{
		fprintf ( stderr, 
			"LoadHistory returned %d, cannot find benchmark (%s) price for %s %s<br>\n",
					rv, xportfolio.xpbenchmark, xportfolio.xpticker, xportfolio.xpdate );
	}
	else
	{
		BenchmarkStartPrice = xhistory.xhclose;
	}

	/*----------------------------------------------------------
		benchmark current price -- this could live in getargs().
	----------------------------------------------------------*/
	if ( BenchmarkCurrentPrice == 0.0 )
	{
		sprintf ( Statement, "select Hclose from history where Hticker = '%s' order by Hdate desc limit 1", BenchmarkTicker );

		qryHistory = dbySelect ( "invest", &MySql, Statement, LogFileName );

		if ( qryHistory == (DBY_QUERY *) 0 )
		{
			fprintf ( stderr, "LoadHistoryCB select returned NULL. %s\n", Statement );
			exit ( 1 );
		}
		else if (( NumberOfRows = qryHistory->NumRows ) == 0 )
		{
			fprintf ( stderr, "LoadHistoryCB select returned zero rows. %s\n", Statement );
			exit ( 1 );
		}
		else 
		{
			if (( qryHistory->EachRow = mysql_fetch_row ( qryHistory->Result )) != NULL )
			{
				BenchmarkCurrentPrice = nsAtof( qryHistory->EachRow[0] );
				BenchmarkPercent = 100.0 * ( BenchmarkCurrentPrice - BenchmarkStartPrice ) / BenchmarkStartPrice;
				dbyFreeQuery ( qryHistory );
				qryHistory = (DBY_QUERY *) 0;
			}
			else
			{
				fprintf ( stderr, "fetch returned zero rows. %s\n", Statement );
				exit ( 1 );
			}
		}
	}
	BenchmarkPercent = 100.0 * ( BenchmarkCurrentPrice - BenchmarkStartPrice ) / BenchmarkStartPrice;

	/*----------------------------------------------------------
		get total dividends collected since purchase date
	----------------------------------------------------------*/
	TotalDividends = LoadDividendSum ( &MySql, xportfolio.xpticker, xportfolio.xpdate );
	TotalYield = 100.0 * ( StockYesterdayClose + TotalDividends - StockStartPrice ) / StockStartPrice;
	ComparedToBenchmark = TotalYield - BenchmarkPercent;

	/*----------------------------------------------------------
		get high since purchase date. this is not necessarily 
		the 52 week high
	----------------------------------------------------------*/
	HighSinceValue = 0.0;
	HighSincePercent = 0.0;

	sprintf ( Statement, "select max(Hhigh) from history where Hticker = '%s' and Hdate > '%s'",
		xportfolio.xpticker, xportfolio.xpdate );

	qryHistory = dbySelect ( "invest", &MySql, Statement, LogFileName );

	if ( qryHistory == (DBY_QUERY *) 0 )
	{
		fprintf ( stderr, "LoadHistoryCB select returned NULL. %s\n", WhereClause );
	}
	else if (( NumberOfRows = qryHistory->NumRows ) == 0 )
	{
		fprintf ( stderr, "LoadHistoryCB select returned zero rows. %s\n", WhereClause );
		dbyFreeQuery ( qryHistory );
		qryHistory = (DBY_QUERY *) 0;
	}
	else 
	{
		if (( qryHistory->EachRow = mysql_fetch_row ( qryHistory->Result )) != NULL )
		{
			HighSinceValue = nsAtof( qryHistory->EachRow[0] );
		}
		qryHistory = (DBY_QUERY *) 0;
	}

	HighSincePercent = 100.0 * ( StockYesterdayClose - HighSinceValue ) / HighSinceValue;

	/*------------------------------------------------------------------------------------
		ticker  name  shares  botdate  cost  current roi dividends roi SPYroi SPYdiff highsincebot belowhigh  
	------------------------------------------------------------------------------------*/
	ReportCount++;
	if ( ReportCount == 1 )
	{
		sprintf ( OutFileName, "%s/performance_%d.dat", TEMPDIR, getpid() );
		if (( fpOutput = fopen ( OutFileName, "w" )) == (FILE *) 0 )
		{
			fprintf ( stderr, "Cannot create %s for output\n", OutFileName );
			exit ( 1 );
		}
	}

	fprintf ( fpOutput, "%s|", xportfolio.xpticker );
	fprintf ( fpOutput, "%s|", xstock.xsname );
	fprintf ( fpOutput, "%.4f|", xportfolio.xpshares );
	fprintf ( fpOutput, "%s|", xportfolio.xpdate );
	fprintf ( fpOutput, "%.2f|", StockStartPrice );
	fprintf ( fpOutput, "%.2f|", StockYesterdayClose );
	fprintf ( fpOutput, "%.2f|", StockPercent );

	fprintf ( fpOutput, "%.2f|", TotalDividends );
	fprintf ( fpOutput, "%.2f|", TotalYield );
	if ( TotalYield < AlertTrigger )
	{
		fprintf ( fpOutput, "*|" );
	}
	else
	{
		fprintf ( fpOutput, " |" );
	}

	fprintf ( fpOutput, "%.2f|", BenchmarkPercent );
	fprintf ( fpOutput, "%.2f|", ComparedToBenchmark );
	if ( ComparedToBenchmark < AlertTrigger )
	{
		fprintf ( fpOutput, "*|" );
	}
	else
	{
		fprintf ( fpOutput, " |" );
	}

	fprintf ( fpOutput, "%.2f|", HighSinceValue );
	fprintf ( fpOutput, "%.2f|", HighSincePercent );
	if ( HighSincePercent < AlertTrigger )
	{
		fprintf ( fpOutput, "*|" );
	}
	else
	{
		fprintf ( fpOutput, " |" );
	}

	fprintf ( fpOutput, "\n" );

	return ( 0 );
}
