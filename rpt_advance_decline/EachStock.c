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

#include	"rpt_advance_decline.h"


static int cmprec ( DAY_RECORD *a, DAY_RECORD *b )
{
	return ( nsStrcmp ( a->Date, b->Date ));
}

static void SaveDay ( char Date[], char AD )
{
	DAY_RECORD		*ptrDay, Key;
	int				NeedSort = 0;

	if ( DailyCount > 0 )
	{
		nsStrcpy ( Key.Date, Date );
		ptrDay = bsearch ( &Key, DailyArray, DailyCount, sizeof(DAY_RECORD), (int(*)()) cmprec );
	}
	else
	{
		ptrDay = NULL;
	}

	if ( ptrDay == NULL )
	{
		if ( DailyCount >= MAXDAYS )
		{
			fprintf ( stderr, "Exceeds MAX DAYS\n" );
			exit ( 1 );
		}

		nsStrcpy ( DailyArray[DailyCount].Date, Date );
		DailyArray[DailyCount].DayAdvanceCount = 0;
		DailyArray[DailyCount].DayDeclineCount = 0;
		ptrDay = &DailyArray[DailyCount];
		DailyCount++;
		NeedSort = 1;
	}

	if ( AD == 'A' )
	{
		ptrDay->DayAdvanceCount++;
	}
	else if ( AD == 'D' )
	{
		ptrDay->DayDeclineCount++;	
	}
	
	if ( NeedSort )
	{
		qsort ( DailyArray, DailyCount, sizeof(DAY_RECORD), (int(*)()) cmprec );
	}

}
/*---------------------------------------------------------------------------
ar: creating rpt_advance_decline_64.a
a - rpt_advance_decline.o
a - getargs.o
EachStock.c: In function ‘EachStock’:
EachStock.c:66:17: warning: unused variable ‘Avg50’ [-Wunused-variable]
  double Avg200, Avg50;
                 ^~~~~
EachStock.c:66:9: warning: unused variable ‘Avg200’ [-Wunused-variable]
  double Avg200, Avg50;
         ^~~~~~
EachStock.c:65:21: warning: unused variable ‘CurrentPercent’ [-Wunused-variable]
  double DipPercent, CurrentPercent;
                     ^~~~~~~~~~~~~~
EachStock.c:65:9: warning: unused variable ‘DipPercent’ [-Wunused-variable]
  double DipPercent, CurrentPercent;
         ^~~~~~~~~~
EachStock.c:64:9: warning: unused variable ‘LowSince’ [-Wunused-variable]
  double LowSince;
         ^~~~~~~~
EachStock.c:63:7: warning: unused variable ‘HighIndex’ [-Wunused-variable]
  int  HighIndex;
       ^~~~~~~~~
EachStock.c:62:7: warning: unused variable ‘HighDate’ [-Wunused-variable]
  char HighDate[12];
       ^~~~~~~~
EachStock.c:61:9: warning: unused variable ‘High52’ [-Wunused-variable]
  double High52;
         ^~~~~~
a - EachStock.o
a - dorpt.o
using gcc to load /usr/local/bin/rpt_advance_decline
---------------------------------------------------------------------------*/

int EachStock ()
{
	int		Days, ndx, rv;
	int		StockAdvanceCount, StockDeclineCount;
#ifdef STUFF
	double	High52;
	char	HighDate[12];
	int		HighIndex;
	double	LowSince;
	double	DipPercent, CurrentPercent;
	double	Avg200, Avg50;
#endif
#ifdef CURVE_FITTING
	int		LowIndex;
	double	qa, qb, qc, sum, diff;
	double	lm, lb, lr, dblArray[300];
	int		xi, xo;
	int		b1x, tx, b2x;
	double	b1y, ty, b2y, PeakPercent;
	double	cx, cy, r;
#endif


	if ( Debug )
	{
		printf ( "%s %s", xstock.xsticker, xstock.xsname );
	}

	/*----------------------------------------------------------
		load 52 weeks -- this actually loads more than
		12 month, due to market holidays, so use CutoffDate
		to make sure we stay with 52 weeks.
	----------------------------------------------------------*/
	switch ( DateRange )
	{
		case DATE_RANGE_ONE_MONTH:
			Days = AVERAGE_TRADING_DAYS_PER_MONTH;
			break;
		case DATE_RANGE_SIX_MONTHS:
			Days = AVERAGE_TRADING_DAYS_PER_MONTH * 6;
			break;
		case DATE_RANGE_ONE_YEAR:
			Days = AVERAGE_TRADING_DAYS_PER_MONTH * 12;
			break;
	}
	rv = LoadHistoryArray ( &MySql, xstock.xsticker, xstock.xslast, SORT_ASCEND, Days );
	if ( rv < Days )
	{
		if ( Debug )
		{
			printf ( "%s only %d history records\n", xstock.xsticker, rv );
		}
		return ( 0 );
	}

	/*----------------------------------------------------------
		find 52 week high and where it occurred
	----------------------------------------------------------*/
	StockAdvanceCount = StockDeclineCount = 0;
	for ( ndx = 1; ndx < HistoryCount; ndx++ )
	{
		if ( HistoryArray[ndx].Close > HistoryArray[ndx-1].Close )
		{
			StockAdvanceCount++;
			TotalAdvanceCount++;
			SaveDay ( HistoryArray[ndx].Date, 'A' );
		}
		else if ( HistoryArray[ndx].Close < HistoryArray[ndx-1].Close )
		{
			StockDeclineCount++;
			TotalDeclineCount++;
			SaveDay ( HistoryArray[ndx].Date, 'D' );
		}
	}
	
	if ( Debug )
	{
		printf ( " advance %d, decline %d\n", StockAdvanceCount, StockDeclineCount );
	}

#ifdef STIFF

#ifdef NO_WORKY_BUT_NOT_NEEDED
	/*----------------------------------------------------------
		If 52 week high less than 3 months ago, bail.
		This should never happen!
	----------------------------------------------------------*/
	if ( HighIndex > HistoryCount - (Months * AVERAGE_TRADING_DAYS_PER_MONTH) )
	{
#ifdef NEW_FIELDS_IN_STOCK
		if ( Debug )
		{
			printf ( "%s high less than %d months, %d\n", xstock.xsticker, Months, HistoryCount - HighIndex );
		}
#else
		printf ( "%s: less than %d months, select stock should have eliminated this stock\n", xstock.xsticker, Months );
#endif
		return ( 0 );
	}
#endif
	
	/*----------------------------------------------------------
		if current price is not back within 5 percent, bail
	----------------------------------------------------------*/
	CurrentPercent = 100.0 * ( High52 - HistoryArray[HistoryCount-1].Close ) / High52;

	if ( CurrentPercent > Within )
	{
#ifdef NEW_FIELDS_IN_STOCK
		if ( Debug )
		{
			printf ( "%s current percent %.2f\n", xstock.xsticker, CurrentPercent );
		}
#else
		if ( Debug )
		{
			printf ( "%s: more than 5 percent, high %f %s close %f percent %f<br>\n", 
				xstock.xsticker, High52, HistoryArray[HighIndex].Date, HistoryArray[HistoryCount-1].Close , CurrentPercent  );
		}
#endif
		return ( 0 );
	}

	/*----------------------------------------------------------
		find low since 52 week high
	----------------------------------------------------------*/
	LowSince = High52;
	for ( ndx = HighIndex + 1; ndx < HistoryCount; ndx++ )
	{
		if ( LowSince > HistoryArray[ndx].Low )
		{
			LowSince = HistoryArray[ndx].Low;
#ifdef CURVE_FITTING
			LowIndex = ndx;
#endif
		}
	}

	/*----------------------------------------------------------
		if low since less than $10.00, bail
	----------------------------------------------------------*/
	if ( LowSince < 10.0 )
	{
		if ( Debug )
		{
			printf ( "%s cheap stock, %.2f\n", xstock.xsticker, LowSince );
		}
		return ( 0 );
	}

	/*----------------------------------------------------------
		if low since 52 week high is less than 25%, bail
	----------------------------------------------------------*/
	DipPercent = 100.0 * ( High52 - LowSince ) / High52;
	if ( DipPercent < MinimumDip )
	{
		if ( Debug )
		{
			printf ( "%s no dip, %.2f\n", xstock.xsticker, DipPercent );
		}
		return ( 0 );
	}
	
#ifdef CURVE_FITTING
/*----------------------------------------------------------
	analyze curve shapes between 52 week high and now
----------------------------------------------------------*/
printf ( "High ndx %d, date %s, value %.4f\n", HighIndex, HistoryArray[HighIndex].Date, High52 );

/*----------------------------------------------------------
	use linear regression to find best fit line
----------------------------------------------------------*/
for ( xi = HighIndex, xo = 0; xi < HistoryCount; xi++, xo++ )
{
	dblArray[xo] = HistoryArray[xi].Close;
}
dataproc_linreg (  xo, dblArray, &lm, &lb, &lr );
for ( xi = 0, sum = 0.0; xi < xo; xi++ )
{
	diff = lm * xi + lb;
	diff = diff - dblArray[xi];
	sum += diff * diff;
}
sum = sqrt ( sum );
printf ( "linear regression: m %f, b %f, r %f, fit %f\n", lm, lb, lr, sum );

/*----------------------------------------------------------
	look for double bottom with second low less than first.
	\
	 \          /
	  \  /\    /
	   \/  \  /
	        \/
	s  b1 t  b2 c
----------------------------------------------------------*/
for ( xi = HighIndex, xo = 0; xi < HistoryCount; xi++, xo++ )
{
	dblArray[xo] = HistoryArray[xi].Average[AVG_5];
}
rv = dataproc_double_bottom ( xo, dblArray, &b1x, &b1y, &tx, &ty, &b2x, &b2y );

if ( rv == 0 )
{
	double	m, b;

	sum = 0.0;
	/*----------------------------
		first leg
	----------------------------*/
	dataproc_line_slope_intercept ( 0.0, dblArray[0], b1x, b1y, &m, &b );
	for ( xi = 0; xi < b1x; xi++ )
	{
		diff = dataproc_line_y ( (double) xi, m, b );
		diff = diff - dblArray[xi];
		sum += diff * diff;
	}

	/*----------------------------
		second leg
	----------------------------*/
	dataproc_line_slope_intercept ( b1x, b1y, tx, ty, &m, &b );
	for ( xi = b1x; xi < tx; xi++ )
	{
		diff = dataproc_line_y ( (double) xi, m, b );
		diff = diff - dblArray[xi];
		sum += diff * diff;
	}

	/*----------------------------
		third leg
	----------------------------*/
	dataproc_line_slope_intercept ( tx, ty, b2x, b2y, &m, &b );
	for ( xi = tx; xi < b2x; xi++ )
	{
		diff = dataproc_line_y ( (double) xi, m, b );
		diff = diff - dblArray[xi];
		sum += diff * diff;
	}

	/*----------------------------
		fourth leg
	----------------------------*/
	dataproc_line_slope_intercept ( b2x, b2y, xo-1, dblArray[xo-1], &m, &b );
	for ( xi = b2x; xi < xo; xi++ )
	{
		diff = dataproc_line_y ( (double) xi, m, b );
		diff = diff - dblArray[xi];
		sum += diff * diff;
	}

	sum = sqrt ( sum );

	PeakPercent = 100.0 * ( ty - b1y ) / b1y;

	printf ( "double bottom: %d,%f,%d,%f,%d,%f,fit %f, pct %.2f%%\n",  b1x, b1y, tx, ty, b2x, b2y, sum, PeakPercent );
}
else
{
	printf ( "Not an IBD double bottom: " );
	switch ( rv )
	{
		case -1:
			printf ( "reversed W\n" );
			break;
		case -2:
			printf ( "not broad enough\n" );
			break;
		case -3:
			printf ( "less than 5%%\n" );
			break;
	}
}

/*----------------------------------------------------------
	find quadratic equation based end points and low 
----------------------------------------------------------*/
dataproc_quadratic ( HighIndex, High52, LowIndex, LowSince, HistoryCount-1, HistoryArray[HistoryCount-1].Close, &qa, &qb, &qc );
for ( xi = HighIndex, sum = 0.0; xi < HistoryCount; xi++ )
{
	diff = qa * xi * xi + qb * xi + qc;
	diff = diff - HistoryArray[xi].Close;
	sum += diff * diff;
}
sum = sqrt ( sum );
printf ( "quadratic: a %f, b %f, c %f, fit %f\n", qa, qb, qc, sum );


/*----------------------------------------------------------
	find circle equation based on same three points
----------------------------------------------------------*/
if ( dataproc_circle (  HighIndex, High52, LowIndex, LowSince, HistoryCount-1, HistoryArray[HistoryCount-1].Close, &cx, &cy, &r ) == 0 )
{
	double		high, low;

	for ( xi = HighIndex, sum = 0.0; xi < HistoryCount; xi++ )
	{
		dataproc_circle_y ( (double)xi, cx, cy, r, &high, &low );
		diff = low - HistoryArray[xi].Close;
		sum += diff * diff;
		printf ( "%d,%.4f\n", xi, low );
	}
	sum = sqrt ( sum );
	printf ( "circle: cx %f, cy%f, r %f, fit %f\n", cx, cy, r, sum );
}
#endif


	Avg200 = HistoryArray[HistoryCount-1].Average[AVG_200];
	Avg50  = HistoryArray[HistoryCount-1].Average[AVG_50];

	fprintf ( fpOutput, "%-10.10s|%-30.30s|%.2f|%s|%.2f|%.2f|%.2f|%.2f|%.2f\n", 
			xstock.xsticker, xstock.xsname, High52, HighDate, LowSince, DipPercent, CurrentPercent, Avg50, Avg200 );
#endif

	return ( 0 );
}
