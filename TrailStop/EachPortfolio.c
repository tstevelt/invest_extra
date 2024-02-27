/*----------------------------------------------------------------------------
	Program : TrailStop/EachPortfolio.c
	Author  : Tom Stevelt
	Date    : 2019 - 2024
	Synopsis: For each stock held in portfolio, check trailing stop, if set.
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

#include	"TrailStop.h"

int EachPortfolio ()
{
	int		rv;
	double	NewTrailValue, Diff;

	sprintf ( WhereClause, "Sticker = '%s'", xportfolio.xpticker );
	if (( rv = LoadStock ( &MySql, WhereClause, &xstock, 0, 0 )) != 1 )
	{
		printf ( "LoadStock %s returned %d\n", xportfolio.xpticker, rv );
		return ( 0 );
	}

	/*----------------------------------------------------------
		get stock current price
	----------------------------------------------------------*/
	sprintf ( WhereClause, "Hticker = '%s' and Hdate = '%s'", xportfolio.xpticker, xstock.xslast );
	if (( rv = LoadHistory ( &MySql, WhereClause, &xhistory, 0 )) != 1 )
	{
		printf ( "LoadHistory returned %d, cannot find stock price for %s %s\n", rv, xportfolio.xpticker, xstock.xslast );
		return ( 0 );
	}
	
	NewTrailValue = xhistory.xhhigh * ( 1.0 - xportfolio.xptrailpct/100.0 );

	if ( NewTrailValue > xportfolio.xptrailvalue )
	{
		Diff = NewTrailValue - xportfolio.xptrailvalue;
	}
	else
	{
		Diff = 0.0;
	}

	if ( Diff > 0.008 )
	{
		if ( Debug  )
		{
			printf ( "%ld %ld %s close %.2f percent %.2f%% update %.2f -> %.2f\n",
				xportfolio.xid, xportfolio.xpmember, xportfolio.xpticker, 
				xhistory.xhclose, xportfolio.xptrailpct, xportfolio.xptrailvalue, NewTrailValue );
		}
		else
		{
			sprintf ( Statement,
				"update portfolio set Ptrailvalue = %.2f where portfolio.id = %ld",
						NewTrailValue, xportfolio.xid );

			if ( dbyUpdate ( "invest", &MySql, Statement, 0, LogFileName ) != 1 )
			{			
				printf ( "update id %ld failed\n", xportfolio.xid );
				ErrorCount++;
			}
			else
			{
				printf ( "TrailStop: %ld %s close %.2f percent %.2f%% update %.2f -> %.2f\n",
					xportfolio.xpmember, xportfolio.xpticker, 
					xhistory.xhclose, xportfolio.xptrailpct, xportfolio.xptrailvalue, NewTrailValue );
				UpdateCount++;
			}

		}
	}

	return ( 0 );
}
