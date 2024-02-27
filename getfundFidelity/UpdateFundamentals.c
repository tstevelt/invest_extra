/*----------------------------------------------------------------------------
	Program : getfundFidelity/UpdateFundamentals.c
	Author  : Tom Stevelt
	Date    : 2019 - 2024
	Synopsis: Update fundamental record if information changed.
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


#include	"getfundFidelity.h"

static	FILE	*cfp;

static void DoubleChange ( char *Ticker, char *Field, double Previous, double Current )
{
	if ( ChangesCount == 0 )
	{
		if (( cfp = fopen ( ChangesFilename, "w" )) == NULL )
		{
			cfp = stdout;
		}
	}

	fprintf ( cfp, "%s %s %.4f -> %.4f\n", Ticker, Field, Previous, Current );

	ChangesCount++;
}

static void CharChange ( char *Ticker, char *Field, char Previous, char Current )
{
	if ( ChangesCount == 0 )
	{
		if (( cfp = fopen ( ChangesFilename, "w" )) == NULL )
		{
			cfp = stdout;
		}
	}

	if ( Previous == '\0' )
	{
		fprintf ( cfp, "%s %s new %c\n", Ticker, Field, Current );
	}
	else
	{
		fprintf ( cfp, "%s %s %c -> %c\n", Ticker, Field, Previous, Current );
	}

	ChangesCount++;
}

void UpdateFundamentals ()
{
	long	SystemTime;
	char	Fragment[128];
	int		FragCount;

	time ( &SystemTime );

	for ( int ndx = 0; ndx < Count; ndx++ )
	{
		if ( Array[ndx].HaveStock == 0 )
		{
			continue;
		}

		sprintf ( WhereClause, "Fticker = '%s'", Array[ndx].Ticker );
		if (LoadFundamental ( &MySql, WhereClause, &xfundamental, 0 ) != 1 )
		{
			sprintf ( Statement, "insert into fundamental (Fticker) values ('%s');", Array[ndx].Ticker );
			printf ( "FIXIT %s\n", Statement );
			continue;
		}
		else
		{
			Array[ndx].HaveFundamental = 1;
		}

		sprintf ( Statement, "update fundamental set " );
		FragCount = 0;

		if ( dbldiff(xfundamental.xftotasst,Array[ndx].Assets,0.01) )
		{
			sprintf ( Fragment, "%sFtotasst = %.0f", FragCount ? ", " : "", Array[ndx].Assets );
			strcat ( Statement, Fragment );
			FragCount++;
		}

		if ( dbldiff(xfundamental.xfexpratio,Array[ndx].ExpenseRatio,0.00001) )
		{
			sprintf ( Fragment, "%sFexpratio = %.4f", FragCount ? ", " : "", Array[ndx].ExpenseRatio );
			strcat ( Statement, Fragment );
			FragCount++;
		}

		if ( dbldiff(xfundamental.xfyield,Array[ndx].Yield,0.01) )
		{
			sprintf ( Fragment, "%sFyield = %.2f", FragCount ? ", " : "", Array[ndx].Yield );
			strcat ( Statement, Fragment );
			FragCount++;
		}

		if ( dbldiff(xfundamental.xfavgret3,Array[ndx].AvgRet3,0.01) )
		{
			DoubleChange ( Array[ndx].Ticker, "Favgret3", xfundamental.xfavgret3, Array[ndx].AvgRet3 );
			
			sprintf ( Fragment, "%sFavgret3 = %.2f", FragCount ? ", " : "", Array[ndx].AvgRet3 );
			strcat ( Statement, Fragment );
			FragCount++;
		}

		if ( dbldiff(xfundamental.xfavgret5,Array[ndx].AvgRet5,0.01) )
		{
			DoubleChange ( Array[ndx].Ticker, "Favgret5", xfundamental.xfavgret5, Array[ndx].AvgRet5 );
			
			sprintf ( Fragment, "%sFavgret5 = %.2f", FragCount ? ", " : "", Array[ndx].AvgRet5 );
			strcat ( Statement, Fragment );
			FragCount++;
		}

		if ( strchr ( "12345", Array[ndx].Morningstar ) != NULL && xfundamental.xfmstar[0] != Array[ndx].Morningstar )
		{
			CharChange ( Array[ndx].Ticker, "Fmstar", xfundamental.xfmstar[0], Array[ndx].Morningstar );
			
			sprintf ( Fragment, "%sFmstar = '%c'", FragCount ? ", " : "", Array[ndx].Morningstar );
			strcat ( Statement, Fragment );
			FragCount++;
		}

		if ( strchr ( "ABCDEF", Array[ndx].Factset ) != NULL && xfundamental.xffactset[0] != Array[ndx].Factset )
		{
			CharChange ( Array[ndx].Ticker, "Ffactset", xfundamental.xffactset[0], Array[ndx].Factset );
			
			sprintf ( Fragment, "%sFfactset = '%c'", FragCount ? ", " : "", Array[ndx].Factset );
			strcat ( Statement, Fragment );
			FragCount++;
		}

		if ( nsStrcmp ( xfundamental.xfinception,  Array[ndx].Inception ) != 0 )
		{
			sprintf ( Fragment, "%sFinception = '%s'", FragCount ? ", " : "", Array[ndx].Inception );
			strcat ( Statement, Fragment );
			FragCount++;
		}

		if ( FragCount )
		{
			sprintf ( Fragment, ", Fupdated = %ld where Fticker = '%s';", SystemTime, Array[ndx].Ticker );
			strcat ( Statement, Fragment );

			if ( Verbose )
			{
				printf ( "%s\n", Statement );
			}

			if ( dbyUpdate ( "invest", &MySql, Statement, 0, LogFileName ) != 1 )
			{
				printf ( "Update failed on %s\n", Array[ndx].Ticker );
				ErrorCount++;
			}
			else
			{
				UpdateCount++;
			}
		}
		else
		{
			NoChangeCount++;
		}

	}
}
