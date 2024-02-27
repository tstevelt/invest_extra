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

int EachStock ()
{
	int		rv;
	PORT_RECORD		Key, *Ptr;
	long	EntValue;
	double	Ratio, PctTrgt;
	int		ValueType;

	sprintf ( WhereClause, "Fticker = '%s'", xstock.xsticker );
	if (( rv = LoadFundamental ( &MySql, WhereClause, &xfundamental, 0 )) != 1 )
	{
		fprintf ( stderr, "LoadFundamental %s returned %d\n", xstock.xsticker, rv );
		return ( 0 );
	}
	
	sprintf ( Key.Ticker, "%s", xstock.xsticker );
	Ptr =  bsearch ( &Key, PortArray, PortCount, sizeof(PORT_RECORD), (int(*)()) cmpticker );

	if ( Ptr != NULL )
	{
		Ptr->Flag = 1;
	}

	if ( Ptr == NULL && xfundamental.xfgrow1 <= MINGROW1 )
	{
		return ( 0 );
	}

	if ( Ptr == NULL && xfundamental.xfgrow5 <= MINGROW5 )
	{
		return ( 0 );
	}

	if ( Ptr == NULL && xfundamental.xfyield <= MINYIELD )
	{
		return ( 0 );
	}

	ValueType = CalcEntValueAndRatio ( &EntValue, &Ratio );
	if ( EntValue < MinEntValue || EntValue > MaxEntValue )
	{
		if ( Ptr == NULL )
		{
			return ( 0 );
		}
	}

	if ( Ratio < MINRATIO || Ratio > MAXRATIO )
	{
		if ( Ptr == NULL )
		{
			return ( 0 );
		}
	}

//	printf ( "%-20.20s\n", xstock.xsticker );

	ReportCount++;
	if ( ReportCount == 1 )
	{
		sprintf ( OutFileName, "%s/prospect_%d.dat", TEMPDIR, getpid() );
		if (( fpOutput = fopen ( OutFileName, "w" )) == (FILE *) 0 )
		{
			fprintf ( stderr, "Cannot create %s for output\n", OutFileName );
			exit ( 1 );
		}
	}

	if ( xfundamental.xftarget > 0 )
	{
		PctTrgt = 100.0 * xstock.xsclose / (double) xfundamental.xftarget;
	}
	else
	{
		PctTrgt = 0.0;
	}

	if ( Ptr != NULL )
	{
		fprintf ( fpOutput, "X|" );
	}
	else
	{
		fprintf ( fpOutput, " |" );
	}

	fprintf ( fpOutput, "%s|", xstock.xsticker );
	fprintf ( fpOutput, "%s|", xstock.xsname );
	fprintf ( fpOutput, "%.3f|", xfundamental.xfgrow1 );
	fprintf ( fpOutput, "%.3f|", xfundamental.xfgrow5 );
	fprintf ( fpOutput, "%ld|", EntValue );
	fprintf ( fpOutput, "%.3f|", Ratio );
	if ( ValueType == 1 )
	{
		fprintf ( fpOutput, "PE|" );
	}
	else
	{
		fprintf ( fpOutput, "VAL/EARN|" );
	}
	fprintf ( fpOutput, "%.3f|", xfundamental.xfyield );
	fprintf ( fpOutput, "%.3f|", PctTrgt );

	fprintf ( fpOutput, "\n" );

	return ( 0 );
}
