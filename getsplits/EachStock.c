/*----------------------------------------------------------------------------
	Program : getsplits/EachStock.c
	Author  : Tom Stevelt
	Date    : 2019 - 2024
	Synopsis: For each stock, get splits information from IEX
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

#include	"getsplits.h"

int EachStock ()
{
	char	cmdline[1024];
	char	FileName[256];
	char	*cpPeriod;
	char	*buffer;
	JSON_NAME_VALUE		*ptrNameValue;
	DATEVAL	dvCurrent;
	DATEVAL	dvPastDate;
	int		nr, xl;
	double	PercentDiff, CurrentAverage, PastAverage;
	int		CheckAnyway;

	switch ( xstock.xstype[0] )
	{
		case STYPE_STOCK:
		case STYPE_ETF:
		case STYPE_ADR:
		case STYPE_REIT:
		case STYPE_PREFER:
			break;
		case STYPE_BOND:
		case STYPE_CRYPTO:
		case STYPE_INDEX:
		case STYPE_OTHER:
		default:
			return ( 0 );
	}

	xl = lastchr ( xstock.xsticker, nsStrlen(xstock.xsticker) );

	xstock.xsticker[xl] = '\0';

	if ( RunMode != MODE_ONE )
	{
		CheckAnyway = 0;

		sprintf ( WhereClause, "Aticker = '%s' and Adate = '%s'", xstock.xsticker, xstock.xslast );
		nr = LoadAverage ( &MySql, WhereClause, &xaverage, 0 );
		if ( nr == 0 )
		{
			if ( Debug )
			{
				printf ( "No average for %s\n", WhereClause );
			}
			CurrentAverage = xstock.xsclose;
			CheckAnyway = 1;
		}
		else
		{
			CurrentAverage = xaverage.xama5;
		}

		StrToDatevalFmt ( xstock.xslast, DATEFMT_YYYY_MM_DD, &dvCurrent );
		DateAdd ( &dvCurrent, 0 - Days, &dvPastDate );

		sprintf ( WhereClause, "Aticker = '%s' and Adate = '%04d-%02d-%02d'", 
				xstock.xsticker, dvPastDate.year4, dvPastDate.month, dvPastDate.day );
		nr = LoadAverage ( &MySql, WhereClause, &xaverage, 0 );
		if ( nr == 0 )
		{
			if ( Debug )
			{
				printf ( "No average for %s\n", WhereClause );
			}
			PastAverage = xstock.xshigh52;
			CheckAnyway = 1;
		}
		else
		{
			PastAverage = xaverage.xama5;
		}

		PercentDiff = 100.0 * CurrentAverage / PastAverage;
		if ( PercentDiff > 200.0  )
		{
			printf ( "%s reverse split? past %.2f current %.2f\n",
				xstock.xsticker, PastAverage, CurrentAverage );
		}
		else if ( PercentDiff < 50.0  )
		{
			printf ( "%s split? past %.2f current %.2f\n",
				xstock.xsticker, PastAverage, CurrentAverage );
		}
		else if ( CheckAnyway == 0 )
		{
			return ( 0 );
		}
		else
		{
			printf ( "%s check anyway\n", xstock.xsticker );
		}
	}

	sprintf ( FileName, "%s/%s_split.json", TEMPDIR, xstock.xsticker );
	
	if ( Debug >= 2 )
	{
		printf ( "%s\n", FileName );
	}

	if ( Days < (30+20) )
	{
		cpPeriod = "1m";
	}
	else if ( Days < (30*3+20) )
	{
		cpPeriod = "3m";
	}
	else if ( Days < (30*6+20) )
	{
		cpPeriod = "6m";
	}
	else if ( Days < (365+20) )
	{
		cpPeriod = "1y";
	}
	else if ( Days < (365*2+20) )
	{
		cpPeriod = "2y";
	}
	else
	{
		cpPeriod = "5y";
	}

	sprintf ( cmdline, 
		"curl -s '%s/stock/%s/splits/%s?&token=%s' > %s",
				env_ApiURL, xstock.xsticker, cpPeriod, env_ApiKey, FileName );

	if ( Debug )
	{
		printf ( "%s\n", cmdline );
	}

	IEX_RateLimit ( 0 );

	/*----------------------------------------------------------
		fixit - still using curl system() instead of library.
	----------------------------------------------------------*/

	system ( cmdline );

	if (( buffer = JsonOpenFileAndRead ( FileName )) == NULL )
	{
		if ( Debug )
		{
			printf ( "JsonOpenFileAndRead NULL on %s\n", FileName );
		}
		return ( 0 );
	}

	if ( nsStrncmp ( buffer, "[]", 2 ) == 0 )
	{
		printf ( "No split on file for %s\n", xstock.xsticker );
	}
	else
	{
		printf ( "%s", xstock.xsticker );
		if (( ptrNameValue = JsonScan ( buffer, "exDate", JSON_FIRST )) != NULL )
		{
			printf ( " exDate %-10.10s", ptrNameValue->Value );
		}
		if (( ptrNameValue = JsonScan ( buffer, "fromFactor", JSON_FIRST )) != NULL )
		{
			printf ( " fromFactor %d", nsAtoi(ptrNameValue->Value) );
		}
		if (( ptrNameValue = JsonScan ( buffer, "toFactor", JSON_FIRST )) != NULL )
		{
			printf ( " toFactor %d", nsAtoi(ptrNameValue->Value) );
		}
		printf ( "\n" );
	}

	free ( buffer );

	return ( 0 );
}
