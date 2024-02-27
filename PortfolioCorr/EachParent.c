/*----------------------------------------------------------------------------
	Program : PortfolioCorr/EachParent.c
	Author  : Tom Stevelt
	Date    : 2019 - 2024
	Synopsis: Get stock record and then collect history closing prices
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

#include	"PortfolioCorr.h"

static int EachHistory ()
{
	int		ndx;

	if ( DataIndex == 0 )
	{
		if ( DataCount >= MAXDAYS )
		{
			return ( 1 );
		}
		nsStrcpy ( DataArray[DataCount].Date, xhistory.xhdate );
		DataArray[DataCount].Close[DataIndex] = xhistory.xhclose;
		DataCount++;
	}
	else
	{
		for ( ndx = 0; ndx < DataCount; ndx++ )
		{
			if ( nsStrcmp ( DataArray[ndx].Date, xhistory.xhdate ) == 0 )
			{
				DataArray[ndx].Close[DataIndex] = xhistory.xhclose;
				break;
			}
		}
	}
	
	return ( 0 );
}

int EachParent ()
{
	int		rv;
	char	*oneTicker;

	switch ( ReportTable )
	{
		case 'P':
			oneTicker = xportfolio.xpticker;
			break;
		case 'W':
			oneTicker = xwatchlist.xwticker;
			break;
		case 'T':
			oneTicker = xtemp.xtticker;
			break;
		case 'F':
			oneTicker = xstock.xsticker;
			break;
	}

	sprintf ( WhereClause, "Hticker = '%s'", oneTicker );
	sprintf ( OrderByClause, "Hdate desc" );
	if (( rv = LoadHistoryCB ( &MySql, WhereClause, OrderByClause, &xhistory, EachHistory, 0 )) < 1 )
	{
		if ( Debug )
		{
			printf ( "LoadHistory returned %d, not history for %s\n", rv, oneTicker );
		}
		return ( 0 );
	}
	else
	{
		nsStrcpy ( TickerArray[DataIndex],  oneTicker );
		DataIndex++;
		if ( Debug )
		{
			printf ( "LoadHistory returned %d for %s\n", rv, oneTicker );
		}
	}


	return ( 0 );
}
