/*---------------------------------------------------------------------------
    Market Risk

    Copyright (C)  2025 Tom Stevelt

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
---------------------------------------------------------------------------*/

#include	"MarketRisk.h"

static void CalcER ( char *ticker )
{
	double	CurrAvg, PrevAvg, Return;
	DATEVAL	dvCurr, dvPrev;

	sprintf ( WhereClause, "Sticker = '%s'", ticker  );
	if ( LoadStock ( &MySql, WhereClause, &xstock, 0, 0 ) == 0 )
	{
		printf ( "CaleER: unknown ticker %s\n", ticker );
		exit ( 1 );
	}
	
	sprintf ( WhereClause, "Aticker = '%s' and Adate = '%s'", ticker, xstock.xslast  );
	LoadAverage ( &MySql, WhereClause, &xaverage, 0 );
	CurrAvg = xaverage.xama5;

	
	/*----------------------------------------------------------
	int DateAdd ( DATEVAL *a , int NumberOfDays , DATEVAL *b );
		0123456789	
		2025-02-28
	----------------------------------------------------------*/

	dvCurr.year4 = atoi( xstock.xslast);
	dvCurr.month = atoi(&xstock.xslast[5]);
	dvCurr.day   = atoi(&xstock.xslast[8]);

	DateAdd ( &dvCurr, -260, &dvPrev );

	sprintf ( WhereClause, "Aticker = '%s' and Adate = '%04d-%02d-%02d'", ticker, dvPrev.year4, dvPrev.month, dvPrev.day );
	LoadAverage ( &MySql, WhereClause, &xaverage, 0 );
	PrevAvg = xaverage.xama5;
	
	Return = 100.0 * (CurrAvg - PrevAvg ) / PrevAvg;

	printf ( "%-4.4s %-30.30s: previous year return %.2f\n",
		xstock.xsticker, xstock.xsname, Return );
}

void DoExpected ()
{
	// double ER = 1.0 / CAPE - TIPS;
	double ER = 100.0 / CAPE - TIPS;
	printf ( "(1/CAPE-TIPS) Expected Return %.2f\n", ER );

	for ( int ndx = 0; ndx < TickerCount; ndx++ )
	{
		CalcER ( TickerArray[ndx] );
	}
}

