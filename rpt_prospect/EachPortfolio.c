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

#include	"rpt_prospect.h"

long CalcEntValueAndRatio ( long *EntValue, double *Ratio )
{
	int		rv = 0;

	if ( xfundamental.xfentval > 0 )
	{
		*EntValue = xfundamental.xfentval;
		*Ratio = xfundamental.xfvalue;
	}
	else
	{
		*EntValue = xfundamental.xfshares *  xstock.xsclose;
		*Ratio = xfundamental.xfpefwd;
		rv = 1;
	}
	return ( rv );
}

int EachPortfolio ()
{
	int		rv;
//	double	StockStartPrice, StockYesterdayClose, StockPercent; // , StockLatestPrice;
//	double	HighSinceValue, HighSincePercent;
//	DBY_QUERY	*qryHistory;
//	int		NumberOfRows;
//	double	ComparedToBenchmark;
//	double	BenchmarkStartPrice, BenchmarkPercent;
//	double	TotalDividends, TotalYield; // AnnualYield;

	sprintf ( WhereClause, "Sticker = '%s'", xportfolio.xpticker );
	if (( rv = LoadStock ( &MySql, WhereClause, &xstock, 0, 1 )) != 1 )
	{
		fprintf ( stderr, "LoadStock %s returned %d\n", xportfolio.xpticker, rv );
		return ( 0 );
	}

	switch ( xstock.xstype[0] )
	{
		case 'S':
		case 'A':
			break;
		default:
			if ( Debug )
			{
				printf ( "Skipping %s, type %c\n", xstock.xsticker, xstock.xstype[0] );
			}
			return ( 0 );
	}

	sprintf ( WhereClause, "Fticker = '%s'", xportfolio.xpticker );
	if (( rv = LoadFundamental ( &MySql, WhereClause, &xfundamental, 0 )) != 1 )
	{
		fprintf ( stderr, "LoadFundamental %s returned %d\n", xportfolio.xpticker, rv );
		return ( 0 );
	}
	
	if ( PortCount >= MAXPORT )
	{
		fprintf ( stderr, "Exceeds MAXPORT\n" );
		exit ( 0 );
	}

	sprintf ( PortArray[PortCount].Ticker, "%s", xportfolio.xpticker );
	PortArray[PortCount].Grow5  = xfundamental.xfgrow5;
	CalcEntValueAndRatio ( &PortArray[PortCount].EntVal, &PortArray[PortCount].Ratio );

	if ( PortCount == 0 )
	{
		MinEntValue = PortArray[PortCount].EntVal;
		MaxEntValue = PortArray[PortCount].EntVal;
	}
	else
	{
		if ( MinEntValue > PortArray[PortCount].EntVal )
		{
			MinEntValue = PortArray[PortCount].EntVal;
		}
		if ( MaxEntValue < PortArray[PortCount].EntVal )
		{
			MaxEntValue = PortArray[PortCount].EntVal;
		}
	}

	PortCount++;

	return ( 0 );
}
