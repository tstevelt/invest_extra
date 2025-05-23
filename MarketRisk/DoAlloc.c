
#include	"MarketRisk.h"

static double GetPercent ( char *ticker )
{
	double	rv = 0.0;

	sprintf ( WhereClause, "Sticker = '%s'", ticker  );
	if ( LoadStock ( &MySql, WhereClause, &xstock, 0, 0 ) == 0 )
	{
		printf ( "GetPercent: unknown ticker %s\n", ticker );
		exit ( 1 );
	}
	
	sprintf ( WhereClause, "Aticker = '%s' and Adate = '%s'", ticker, xstock.xslast  );
	LoadAverage ( &MySql, WhereClause, &xaverage, 0 );
	
	rv = 100.0 * xaverage.xama5 / xaverage.xama200 - 100.0;

	printf ( "%-4.4s %-30.30s: ma200 %.2f ma5 %.2f percent %.2f\n",
		xstock.xsticker, xstock.xsname,  xaverage.xama200, xaverage.xama5, rv );

	return ( rv );
}

void DoAlloc ()
{
#ifdef STUFF
	double	DomesticPercent, DevelopedPercent, EmergingPercent, ExChinaPercent, GoldPercent;

	DomesticPercent = GetPercent ( "ITOT" );
	DevelopedPercent = GetPercent ( "IDEV" );
	EmergingPercent = GetPercent ( "EEM" );
	ExChinaPercent = GetPercent ( "EMXC" );
	GoldPercent = GetPercent ( "IAU" );
#endif

	for ( int ndx = 0; ndx < TickerCount; ndx++ )
	{
		GetPercent ( TickerArray[ndx] );
	}
}

#ifdef STUFF

#include	"MarketRisk.h"

void DoRisk ()
{
	int		rv, ndx, Start, Count;
	double	PrevAvg, CurrAvg;

	rv = LoadHistoryArray ( &MySql, xstock.xsticker, xstock.xslast, SORT_ASCEND, Days );
	printf ( "%s: Loaded %d history records, earlist date %s\n", xstock.xsticker, rv, HistoryArray[0].Date );
	if ( Method == 2 && rv < Days )
	{
		printf ( "Not enough history for current calculation\n" );
		exit ( 1 );
	}

	switch ( Method )
	{
		case 1:
			printf ( "Date,Previous,Current\n" );
			Days = rv;
			for ( Start = 0; Start < Days - 260; Start++ )
			{
				PrevAvg = CurrAvg = 0.0;
				for ( ndx = Start, Count = 0; Count < 255; ndx++, Count++ )
				{
					PrevAvg += HistoryArray[ndx].Close;
				}
				PrevAvg = PrevAvg / Count;
				for ( Count = 0; ndx < Days && Count < 5; ndx++, Count++ )
				{
					CurrAvg += HistoryArray[ndx].Close;
				}
				CurrAvg = CurrAvg / Count;
				printf ( "%s,%.2f,%.2f\n", HistoryArray[ndx-1].Date, PrevAvg, CurrAvg );
			}
			break;
		case 2:
			PrevAvg = CurrAvg = 0.0;
			for ( ndx = 0, Count = 0; ndx < Days - 5; ndx++, Count++ )
			{
				PrevAvg += HistoryArray[ndx].Close;
			}
			PrevAvg = PrevAvg / Count;
			for ( Count = 0; ndx < Days; ndx++, Count++ )
			{
				CurrAvg += HistoryArray[ndx].Close;
			}
			CurrAvg = CurrAvg / Count;
			printf ( "%s: Previous Average %.2f, Current Average %.2f\n", xstock.xsticker, PrevAvg, CurrAvg );
			break;
	}
}
#endif
