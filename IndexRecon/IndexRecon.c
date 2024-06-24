/*----------------------------------------------------------------------------
	Program : IndexRecon.c
	Author  : Tom Stevelt
	Date    : Jun 2021
	Synopsis: Index Reconstitute
	Return  : 

	Russell 1000 and 2000 are reconstituted last Friday of June:
		https://www.ftserussell.com/resources/russell-reconstitution

	SP500
		https://github.com/datasets/s-and-p-500-companies/blob/master/data/constituents.csv
	
	NASDAQ
		/var/local/invest/GetNasdaq.sh

	Todo
	---------------
	check for previous price.  IEX symbols include several that at no longer active.
	check previous price is over MINIMUM ($5 ?)
	do we want ETF or not? jury still out as of June 2021

	Who		Date		Modification
	---------------------------------------------------------------------
	tms		07/13/2021	Get private api keys with GetInvestCfg().
	tms		07/21/2023	Added AddNewStock = 0.

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

#define		MAIN
#include	"IndexRecon.h"

int main ( int argc, char *argv[] )
{
	char	xbuffer[1024];
	char	cmdline[1024];
	char	fnJson[512];
	char				*ptrJsonBuffer;
	JSON_NAME_VALUE		*ptrNameValue;
	char	*tokens[24];
	int		tokcnt, lineno;
	int		OnFileCount, BadIndex, ErrorCount, SkipCount, InsertCount;
	int		xi, xo, xl, rv;
	int		FixIndex;
#define		CHEAP_STOCK		5.0
	double	dblValue;

	getargs ( argc, argv );

	// IEX dead GetInvestCfg ( 0 );

	StartMySQL ( &MySql, "invest" );

	/*----------------------------------------------------------
		fixit - still using curl system() instead of library.
	----------------------------------------------------------*/

	sprintf ( fnJson, "/var/local/tmp/IndexRecon.json" );
	lineno = OnFileCount = BadIndex = ErrorCount = InsertCount = SkipCount = 0;
	while ( fgets ( xbuffer, sizeof(xbuffer), ifp ) != NULL )
	{
		lineno++;
		
		// if (( tokcnt = GetTokensStd ( xbuffer, ",", tokens, 2, 0 )) < 2 )
		if (( tokcnt = GetTokensCSV ( xbuffer, tokens, 2, 0, 0 )) < 2 )
		{
			printf ( "tokcnt %d, lineno %d\n", tokcnt, lineno );
			ErrorCount++;
			continue;
		}

		TrimRightAndLeft ( tokens[0] );
		TrimRightAndLeft ( tokens[1] );

		/*----------------------------------------------------------
			if in the database, then check the index flag and 
			fix it if necessary.  this requires setting all of 
			that particular index flag to N before running the
			program.  also requires that the input file we are
			reading is the complete list for that index.
		----------------------------------------------------------*/
		sprintf ( WhereClause, "Sticker = '%s'", tokens[0] );
		if ( LoadStock ( &MySql, WhereClause, &xstock, 0, 0 ) == 1 )
		{
			FixIndex = 0;
			switch ( IndexType )
			{
				case 'D':
					if ( xstock.xsdj[0] != 'Y' )
					{
						if ( Debug )
						{
							printf ( "%-10.10s %c\n", xstock.xsticker, xstock.xsdj[0] );
							BadIndex++;
						}
						FixIndex = 1;
					}
					break;
				case 'S':
					if ( xstock.xssp500[0] != 'Y' )
					{
						if ( Debug )
						{
							printf ( "%-10.10s %c\n", xstock.xsticker, xstock.xssp500[0] );
							BadIndex++;
						}
						FixIndex = 1;
					}
					break;
				case 'N':
					if ( xstock.xsnasdaq[0] != 'Y' )
					{
						if ( Debug )
						{
							printf ( "%-10.10s %c\n", xstock.xsticker, xstock.xsnasdaq[0] );
							BadIndex++;
						}
						FixIndex = 1;
					}
					break;
				case '1':
					if ( xstock.xsrussell[0] != '1' )
					{
						if ( Debug )
						{
							printf ( "%-10.10s %c\n", xstock.xsticker, xstock.xsrussell[0] );
							BadIndex++;
						}
						FixIndex = 1;
					}
					break;
				case '2':
					if ( xstock.xsrussell[0] != '2' )
					{
						if ( Debug )
						{
							printf ( "%-10.10s %c\n", xstock.xsticker, xstock.xsrussell[0] );
							BadIndex++;
						}
						FixIndex = 1;
					}
					break;
			}

			if ( FixIndex )
			{
				sprintf ( Statement, "update stock set %s = '%c' where Sticker = '%s'",
						  FieldName, SetValue, xstock.xsticker );

				rv = dbyUpdate ( "IndexRecon", &MySql, Statement, 1, LogFileName );

				if ( rv != 1 )
				{
					printf ( "update failed: %s\n",  Statement );
					ErrorCount++;
				}

			}

			OnFileCount++;
			continue;
		}

		if ( AddNewStock == 0 )
		{
			printf ( "Not adding stock for %s %s\n", tokens[0], tokens[1] );
			continue;
		}

		/*----------------------------------------------------------
			see about adding this to the datatbase
		----------------------------------------------------------*/
		if ( Debug )
		{
			printf ( "TICKER %s\n", tokens[0] );
		}

		/*----------------------------------------------------------
			check last historical date and price.  reject if
			date < test data or price < limit price.
		----------------------------------------------------------*/
		sprintf ( cmdline, "curl -s '%s/stock/%s/previous?filter=date,open,high,low,close,volume&token=%s' -o %s",
				env_ApiURL, tokens[0],  env_ApiKey, fnJson );

// printf ( "%s\n", cmdline );

		IEX_RateLimit ( 0 );
		system ( cmdline );

// {"date":"2021-06-25","open":3.37,"high":3.45,"low":3.31,"close":3.37,"volume":10786115}

		if (( ptrJsonBuffer = JsonOpenFileAndRead ( fnJson )) == NULL )
		{
			printf ( "JsonOpenFileAndRead returned null on %s\n", fnJson );
			exit ( 1 );
		}

		if ( nsStrncmp ( ptrJsonBuffer, "Unknown symbol", 14 ) == 0 )
		{
			printf ( "%s previous not found, lineno %d\n", tokens[0], lineno );
			free ( ptrJsonBuffer );
			ErrorCount++;
			continue;
		}

		if (( ptrNameValue = JsonScan ( ptrJsonBuffer, "date", JSON_FIRST )) == NULL )
		{
			printf ( "%s date not found, lineno %d\n", tokens[0], lineno );
			free ( ptrJsonBuffer );
			ErrorCount++;
			continue;
		}

		if ( nsStrncmp ( ptrNameValue->Value, TestDate, 10 ) != 0 )
		{
			SkipCount++;
			if ( Debug )
			{
				printf ( "Skipping %s previous date %10.10s\n", tokens[1], ptrNameValue->Value );
			}
			continue;
		}

		if (( ptrNameValue = JsonScan ( ptrJsonBuffer, "close", JSON_FIRST )) == NULL )
		{
			printf ( "%s close not found, lineno %d\n", tokens[0], lineno );
			free ( ptrJsonBuffer );
			ErrorCount++;
			continue;
		}

		dblValue = nsAtof ( ptrNameValue->Value );
		if ( dblValue < CHEAP_STOCK )
		{
			SkipCount++;
			if ( Debug )
			{
				printf ( "Skipping %s cheap stock %.2f\n", tokens[1], dblValue );
			}
			continue;
		}

		/*----------------------------------------------------------
			get company information
		----------------------------------------------------------*/
		sprintf ( cmdline, "curl -s '%s/stock/%s/company?format=json&token=%s' -o %s", 
					env_ApiURL, tokens[0], env_ApiKey, fnJson );

		IEX_RateLimit ( 0 );
		system ( cmdline );

		if (( ptrJsonBuffer = JsonOpenFileAndRead ( fnJson )) == NULL )
		{
			printf ( "JsonOpenFileAndRead returned null on %s\n", fnJson );
			exit ( 1 );
		}

		if ( nsStrncmp ( ptrJsonBuffer, "Unknown symbol", 14 ) == 0 )
		{
			printf ( "%s not on IEX cloud, lineno %d\n", tokens[0], lineno );
			free ( ptrJsonBuffer );
			ErrorCount++;
			continue;
		}


		/*----------------------------------------------------------
			these are the fields we are interested in
			"symbol":"ABUS"
			"companyName":"Arbutus Biopharma Corp"
			"exchange":"NASDAQ/NGS (GLOBAL SELECT MARKET)"
			"industry":"Pharmaceutical Preparation Manufacturing "
			"issueType":"cs"
			"sector":"Manufacturing"
		----------------------------------------------------------*/

		sprintf ( xstock.xsticker, "%s", tokens[0] );
		if (( ptrNameValue = JsonScan ( ptrJsonBuffer, "companyName", JSON_FIRST )) == NULL )
		{
			printf ( "Cannot find companyName in ptrJsonBuffer for %s\n", tokens[0] );
			free ( ptrJsonBuffer );
			ErrorCount++;
			continue;
		}

		xl = sizeof(xstock.xsname) - 1;
		for ( xi = 0, xo = 0; xo < xl; xi++, xo++ )
		{
			if ( ptrNameValue->Value[xi] == '"' )
			{
				xstock.xsname[xo] = '\0';
				break;
			}
			xstock.xsname[xo] = ptrNameValue->Value[xi];
		}

		if (( ptrNameValue = JsonScan ( ptrJsonBuffer, "issueType", JSON_FIRST )) == NULL )
		{
			printf ( "Cannot find issueType in ptrJsonBuffer for %s\n", tokens[0] );
			free ( ptrJsonBuffer );
			ErrorCount++;
			continue;
		}

		/*----------------------------------------------------------
			only interested in certain types:
				type 	refers to the common issue type
				A ad - ADR
				R re - REIT
				S cs - Common Stock
				E et - ETF

				ce - Closed end fund
				si - Secondary Issue
				lp - Limited Partnerships
				wt - Warrant
				oef - Open Ended Fund
				cef - Closed Ended Fund
				ps - Preferred Stock
				ut - Unit
				struct - Structured Product
		----------------------------------------------------------*/

		if ( nsStrncmp ( ptrNameValue->Value, "cs", 2 ) == 0 )
		{
			xstock.xstype[0] = STYPE_STOCK;
		}
		else if ( nsStrncmp ( ptrNameValue->Value, "ad", 2 ) == 0 )
		{
			xstock.xstype[0] = STYPE_ADR;
		}
		else if ( nsStrncmp ( ptrNameValue->Value, "et", 2 ) == 0 )
		{
			xstock.xstype[0] = STYPE_ETF;
		}
		else if ( nsStrncmp ( ptrNameValue->Value, "re", 2 ) == 0 )
		{
			xstock.xstype[0] = STYPE_REIT;
		}
		else
		{
			SkipCount++;
			if ( Debug )
			{
				printf ( "Skipping %s %s %2.2s\n", xstock.xsticker, xstock.xsname, ptrNameValue->Value );
			}
			free ( ptrJsonBuffer );
			continue;
		}

/*---------------------------------------------------------------------------
+-----------+------------------+------+-----+---------+-------+
| Field     | Type             | Null | Key | Default | Extra |
+-----------+------------------+------+-----+---------+-------+
| Sticker   | varchar(20)      | NO   | PRI | NULL    |       |
| Sname     | varchar(80)      | NO   |     | NULL    |       |
| Slast     | date             | YES  |     | NULL    |       |
| Sclose    | double           | YES  |     | 0       |       |
| Stype     | char(1)          | YES  |     | S       |       |
| Ssp500    | char(1)          | YES  |     | N       |       |
| Srussell  | char(1)          | YES  |     | N       |       |
| Sdj       | char(1)          | YES  |     | N       |       |
| Snasdaq   | char(1)          | YES  |     | N       |       |
| Scheck    | char(1)          | NO   |     | N       |       |
| Shigh52   | double           | YES  |     | 0       |       |
| Sdate52   | date             | YES  |     | NULL    |       |
| Sindustry | int(11) unsigned | YES  |     | NULL    |       |
| Sexchange | varchar(6)       | YES  |     | NULL    |       |
| Ssector   | int(11) unsigned | YES  |     | NULL    |       |
| Snews     | int(11)          | YES  |     | 0       |       |
| Sdomfor   | char(1)          | NO   |     | D       |       |
+-----------+------------------+------+-----+---------+-------+
---------------------------------------------------------------------------*/
//	xstock.xsindustry =
//	xstock.xssector   =
//Sindustry | int(11) unsigned | YES  |     | NULL    |       |
//Ssector   | int(11) unsigned | YES  |     | NULL    |       |

		sprintf ( Statement, 
			"insert into stock (Sticker, Sname, Stype, Scheck, %s) values ( '%s', '%s', '%c', 'N', '%c' )",
				FieldName, xstock.xsticker, xstock.xsname, xstock.xstype[0], SetValue );
			
		rv = dbyInsert ( "IndexRecon", &MySql, Statement, 0, LogFileName );

		if ( rv == 1 )
		{
			printf ( "Added %s\n", xstock.xsticker );
			InsertCount++;
		}
		else
		{
			printf ( "insert failed: %s\n", Statement );
			ErrorCount++;
		}



		free ( ptrJsonBuffer );
	}


	nsFclose ( ifp );

	printf ( "%d lines, %d on file, %d bad index, %d errors, %d skipped, %d new\n", 
			lineno, OnFileCount, BadIndex, ErrorCount, SkipCount, InsertCount );

	return ( 0 );
}
