/*----------------------------------------------------------------------------
	Program : InsertStock.c
	Author  : Tom Stevelt 
	Date    : 07/19/2019
	Synopsis: Get all symbols from IEX Cloud, insert new companies.
			  curl -s "$BASEURL/ref-data/symbols/?format=csv&token=$TOKEN"
	Return  : 

	Who		Date		Modification
	---------------------------------------------------------------------
	tms		07/13/2021	Get private api keys with GetInvestCfg().

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
#include	"InsertStock.h"


int main ( int argc, char *argv[] )
{
	char	cmdline[1024];
	char	xbuffer[512];
#define		MAXTOKS		15
	char	*tokens[MAXTOKS];
	int		tokcnt, lineno, rv;
	FILE	*ifp;
	FILE	*pfp;
	char	pbuffer[128];
	double	LatestPrice;
	int		InsertCount = 0;
	int		ErrorCount = 0;
	int		PriceCount = 0;
	int		xi, xl;

	getargs ( argc, argv );

	GetInvestCfg ( 0 );

	if ( RunMode == MODE_CLOUD )
	{
		sprintf ( cmdline, "curl -s '%s/ref-data/symbols/?format=csv&token=%s' > %s",
			env_ApiURL,
			env_ApiKey,
			DATAFILE );

		system ( cmdline );

		RunMode = MODE_FILE;
	}

	StartMySQL ( &MySql, "invest" );

	if ( RunMode == MODE_FILE )
	{
/*---------------------------------------------------------------------------
0      1        2    3    4    5     6      7        8
symbol,exchange,name,date,type,iexId,region,currency,isEnabled
A,NYS,Agilent Technologies Inc.,2019-07-19,cs,IEX_46574843354B2D52,US,USD,true
AA,NYS,Alcoa Corp.,2019-07-19,cs,IEX_4238333734532D52,US,USD,true
AAAU,PSE,Perth Mint Physical Gold ETF,2019-07-19,et,IEX_474B433136332D52,US,USD,true
AABA,NAS,Altaba Inc.,2019-07-19,cef,IEX_4E5434354A302D52,US,USD,true
AAC,NYS,AAC Holdings Inc.,2019-07-19,cs,IEX_4843364642592D52,US,USD,true
AADR,PSE,AdvisorShares Dorsey Wright ADR ETF,2019-07-19,et,IEX_5253355435362D52,US,USD,true
AAL,NAS,American Airlines Group Inc.,2019-07-19,cs,IEX_4353464A535A2D52,US,USD,true
AAMC,ASE,Altisource Asset Management Corp.,2019-07-19,cs,IEX_5442323844432D52,US,USD,true
AAME,NAS,Atlantic American Corporation,2019-07-19,cs,IEX_5737584C53442D52,US,USD,true

March 7, 2021
0      1        2              3            4    5    6    7     8      9        10        11   12  13
symbol,exchange,exchangeSuffix,exchangeName,name,date,type,iexId,region,currency,isEnabled,figi,cik,lei
A,NYS,UN,"NEW YORK STOCK EXCHANGE, INC.",Agilent Technologies Inc.,2021-03-07,cs,IEX_46574843354B2D52,US,USD,true,BBG000C2V3D6,0001090872,QUIX8Y7A2WP0XRMW7G29
AA,NYS,UN,"NEW YORK STOCK EXCHANGE, INC.",Alcoa Corp,2021-03-07,cs,IEX_4238333734532D52,US,USD,true,BBG00B3T3HD3,0001675149,549300T12EZ1F6PWWU29
AAA,USPAC,UP,NYSE ARCA,Listed Funds Trust - AAF First Priority CLO Bond ETF,2021-03-07,et,IEX_5030314338392D52,US,USD,true,BBG00X5FSP48,0001683471,
AAAU,USPAC,UP,NYSE ARCA,Goldman Sachs Physical Gold ETF Shares - Goldman Sachs Physical Gold E,2021-03-07,et,IEX_474B433136332D52,US,USD,true,BBG00LPXX872,0001708646,
AAC=,NYS,UN,"NEW YORK STOCK EXCHANGE, INC.",Ares Acquisition Corporation - Units (1 Ord Share Class A & 1/5 War),2021-03-07,,,US,USD,true,BBG00YYCX683,0001829432,
---------------------------------------------------------------------------*/

#define		TOK_SYMBOL	0
#define		TOK_NAME	4
#define		TOK_TYPE	6

		if (( ifp = fopen ( DATAFILE, "r" )) == NULL )
		{
			printf ( "Cannot open %s for reading\n", DATAFILE );
			exit ( 1 );
		}

		lineno = 0;
		while ( fgets ( xbuffer, sizeof(xbuffer), ifp ) != NULL )
		{
			lineno++;

//			if (( tokcnt = GetTokensA ( xbuffer, ",\n\r", tokens, MAXTOKS )) < 9 )

// int GetTokensCSV ( char *Buffer , char *tokens [], int MaxToks , int KeepInternalCommas , int Verbose );
			if (( tokcnt = GetTokensCSV ( xbuffer, tokens, MAXTOKS, 0, 0 )) < 9 )
			{
				printf ( "tokcnt %d line %d\n", tokcnt, lineno );
				continue;
			}

			if ( nsStrcmp ( tokens[TOK_SYMBOL], "symbol" ) == 0 )
			{
				continue;
			}

			xstock.xstype[0] = ReadStockType ( tokens[TOK_TYPE] );
			if ( xstock.xstype[0] == STYPE_OTHER )
			{
				continue;
			}


			sprintf ( WhereClause, "Sticker = '%s'", tokens[TOK_SYMBOL] );
			
			if ( LoadStock ( &MySql, WhereClause, &xstock, 0, 0 ) == 1 )
			{
				continue;
			}

			// https://cloud.iexapis.com/stable/stock/aapl/quote/latestPrice

			sprintf ( cmdline, "curl -s '%s/stock/%s/quote/latestPrice?token=%s' > %s",
				env_ApiURL,
				tokens[TOK_SYMBOL],
				env_ApiKey,
				PRICEFILE );

			// printf ( "%s\n", cmdline );
			// LatestPrice = 0.0;

			IEX_RateLimit ( 0 );

			/*----------------------------------------------------------
				fixit - still using curl system() instead of library.
			----------------------------------------------------------*/

			system ( cmdline );

			if (( pfp = fopen ( PRICEFILE, "r" )) == NULL )
			{
				printf ( "Cannot open price file %s\n", PRICEFILE );
				continue;
			}

			fgets ( pbuffer, sizeof(pbuffer), pfp );
			nsFclose ( pfp );
			LatestPrice = nsAtof ( pbuffer );

#ifdef DEBUG
#define DEBUG
			printf ( "%s %.2f\n", tokens[TOK_SYMBOL], LatestPrice );
#endif

			if ( LatestPrice < MINIMUM_PRICE )
			{
				PriceCount++;
				continue;
			}

			for ( xi = 0, xl = nsStrlen(tokens[TOK_NAME]); xi < xl; xi++ )
			{
				if ( tokens[TOK_NAME][xi] == '\'' )
				{
					tokens[TOK_NAME][xi] = ' ';
				}
			}

			printf ( "Adding %s %s %s LATEST PRICE %.2f\n", tokens[TOK_TYPE], tokens[TOK_SYMBOL], tokens[TOK_NAME], LatestPrice );

			sprintf ( Statement, "insert into stock (Sticker, Sname, Stype, Scheck) values ('%s', '%s', '%c', 'N')",
				tokens[TOK_SYMBOL], tokens[TOK_NAME], xstock.xstype[0] );

			rv = dbyInsert ( "InsertStock", &MySql, Statement, 0, LogFileName );

			if ( rv == 1 )
			{
				InsertCount++;
			}
			else
			{
				printf ( "insert failed: %s\n", Statement );
				ErrorCount++;
			}

			fflush ( stdout );
		}

		printf ( "Inserted %d stocks, %d low price, %d errors\n", InsertCount, PriceCount, ErrorCount );

		fflush ( stdout );
	}
	

	return ( 0 );
/*----------+------------------+------+-----+---------+-------+
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
| Ssector   | char(2)          | YES  |     | NULL    |       |
| Sindustry | int(11) unsigned | YES  |     | NULL    |       |
+-----------+------------------+------+-----+---------+------*/
}
