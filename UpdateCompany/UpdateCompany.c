/*----------------------------------------------------------------------------
	Program : UpdateCompany.c
	Author  : Tom Stevelt
	Date    : July 2017
	Synopsis: Read company info file from IEX Cloud.  Update Sector and Industry
	Return  : 

	Who		Date		Modification
	---------------------------------------------------------------------
	tms		07/01/2019	getargs and filename
	tms		07/31/2019	added exchange
	tms		03/07/2021	new file format from IEX
	tms		03/07/2021	reconfigured sector table
	tms		03/07/2021	Set sector and/or industry to unknown if not given.
	tms		06/26/2021	Add sector and/or industry if not in db. AUTOADD
	tms		04/15/2022	Added issueType to update Stype
	tme		04/22/2022	Added "country:" to set Sdomfor

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
/*----------------------------------------------------------
select * from sector where SECname = 'Unknown' ;
select * from industry where INDname = 'Unknown' ;
+----+---------+
| id | SECname |
+----+---------+
| 64 | Unknown |
+----+---------+
+-----+---------+
| id  | INDname |
+-----+---------+
| 257 | Unknown |
+-----+---------+
----------------------------------------------------------*/
/*----------------------------------------------------------
country:Bermuda
country:Canada
country:China (Mainland)
country:Netherlands
country:null			<--- domestic
country:Russia
country:Singapore
country:United Kingdom
country:United States	<--- domestic
country:US				<--- domestic
----------------------------------------------------------*/

#define		MAIN
#include	"UpdateCompany.h"

#define	UNKNOWN_SECTOR		64
#define	UNKNOWN_INDUSTRY	257
#undef		AUTOADD
#define		AUTOADD

typedef struct
{
	char	*Iex;
	char	*Invest;
	int		Length;
} EXCHANGE_RECORD;

static EXCHANGE_RECORD ExchangeArray[] = 
{
	{ "Cboe BZX US Equities Exchange",	"CBOE" },
	{ "IEX",							"IEX" },
	{ "NASDAQ",							"NASDAQ" },
	{ "New York Stock Exchange",		"NYSE" },
	{ "NYSE American",					"AMEX" },
	{ "NYSE Arca",						"ARCA" },
	{ "NYSE ARCA",						"ARCA" },
	{ "US OTC",							"OTC" },
	{ "NASDAQ",							"NASDAQ" },
	{ "NASDAQ CAPITAL MARKET",			"NASDAQ" },
	{ "NASDAQ/NGS",						"NASDAQ" },
	{ "NASDAQ/NMS",						"NASDAQ" },
	{ "NEW YORK STOCK EXCHANGE",		"NYSE" },
	{ "NYSE MKT LLC",					"NYSE" },
	{ "OTCQB MARKETPLACE",				"OTC" },
	{ "CBOE BZX U.S. EQUITIES EXCHANGE", "CBOE" },
	{ "OTC PINK CURRENT", 				"OTC" },
	{ "OTCQX MARKETPLACE",				"OTC" },
};

static int ExchangeCount = sizeof(ExchangeArray) / sizeof(EXCHANGE_RECORD);

typedef struct
{
	char	*CountryDescription;
	int		Length;
} MY_COUNTRY_RECORD;

static	MY_COUNTRY_RECORD	MyCountryArray [] =
{
	{ "null" },
	{ "United States" },
	{ "US" },
};

static	int				MyCountryCount = sizeof(MyCountryArray) / sizeof(MY_COUNTRY_RECORD);

int main ( int argc, char *argv[] )
{
	char	xbuffer[256];
	char	PrevTicker[48];
	#define		MAXTOKS		20
	char	*tokens[MAXTOKS];
	int		tokcnt, xt, xl, lineno, ndx;
	int		Okay[7], Error[7], HaveFund;
	char	*ptrExchange;
	char	StockNasdaq, StockType, DomesticForeign, CountryAlpha2[3];
	COUNTRY_RECORD	*ptrCountry;

	getargs ( argc, argv );

	for ( ndx = 0; ndx < ExchangeCount; ndx++ )
	{
		ExchangeArray[ndx].Length = nsStrlen ( ExchangeArray[ndx].Iex );
	}

	for ( ndx = 0; ndx < MyCountryCount; ndx++ )
	{
		MyCountryArray[ndx].Length = nsStrlen ( MyCountryArray[ndx].CountryDescription );
	}

	printf ( "Processing %s\n", InputFilename );

	StartMySQL ( &MySql, "invest" );

	memset ( Okay, '\0', sizeof(Okay) );
	memset ( Error, '\0', sizeof(Error) );

	lineno = 0;
	xt = 0;
	HaveFund = 0;
	sprintf ( PrevTicker, "zzzzzzzzzz" );
	while ( fgets ( xbuffer, sizeof(xbuffer), fp ) != (char *)0 )
	{
		lineno++;

		tokcnt = GetTokensStd ( xbuffer, ":\n\r", tokens, MAXTOKS, 0 );
		if ( tokcnt < 2 )
		{
			if ( Debug )
			{
				printf ( "token count %d\n", tokcnt );
			}

			continue;
		}

// 'symbol': 'ADPT'
		if ( nsStrcmp ( tokens[0], "symbol" ) == 0 )
		{
			if ( strcmp ( PrevTicker, tokens[1] ) != 0 && xt != 6 )
			{
				printf ( "%s only has %d required fields\n", PrevTicker, xt );
			}

			sprintf ( PrevTicker, "%s", tokens[1]  );

			// xt++;
			xt = 1;

			sprintf ( WhereClause, "Sticker = '%s'", tokens[1] );
			if ( LoadStock ( &MySql, WhereClause, &xstock, 0, 0 ) != 1 )
			{
				printf ( "Cannot find stock %s, line %d\n", tokens[1], lineno );
				fflush ( stdout );
				Error[0]++;
				continue;
			}
			Okay[0]++;

			sprintf ( WhereClause, "Fticker = '%s'", tokens[1] );
			if ( LoadFundamental ( &MySql, WhereClause, &xfundamental, 0 ) == 1 )
			{
				HaveFund = 1;
			}
		}
	
// 'exchange': 'NASDAQ'
		else if ( nsStrcmp ( tokens[0], "exchange" ) == 0 )
		{
			xt++;
				
			for ( ndx = 0; ndx < ExchangeCount; ndx++ )
			{
				if ( nsStrncmp ( tokens[1], ExchangeArray[ndx].Iex, ExchangeArray[ndx].Length ) == 0 )
				{
					ptrExchange = ExchangeArray[ndx].Invest;
					break;
				}
			}

			if ( ndx >= ExchangeCount  )
			{
				printf ( "Cannot find exchange %s, line %d\n", tokens[1], lineno );
				fflush ( stdout );
				Error[1]++;
				continue;
			}

			Okay[1]++;
		}


// 'industry': 'Miscellaneous Commercial Services'
		else if ( nsStrcmp ( tokens[0], "industry" ) == 0 )
		{
			xt++;
			if ( nsStrlen ( tokens[1] ) == 0 )
			{
				xindustry.xid = UNKNOWN_INDUSTRY;
			}
			else
			{
				xl = imin ( nsStrlen(tokens[1]), sizeof(xindustry.xindname)-1 );

				sprintf ( WhereClause, "INDname = '%-*.*s'", xl, xl, tokens[1] );
				if ( LoadIndustry ( &MySql, WhereClause, &xindustry, 0 ) != 1 )
				{
#ifndef AUTOADD
					printf ( "Cannot find industry %s, line %d\n", tokens[1], lineno );
					fflush ( stdout );
					// xt = 0;
					Error[2]++;
					continue;
#else
					sprintf ( Statement, "insert into industry (INDname) value ( '%-*.*s' )", xl, xl, tokens[1] );
					if ( dbyInsert ( "invest", &MySql, Statement, 0, LogFileName ) != 1 )
					{
						printf ( "Cannot insert industry %s, line %d\n", tokens[1], lineno );
						fflush ( stdout );
						// xt = 0;
						Error[2]++;
						continue;
					}
					LoadIndustry ( &MySql, WhereClause, &xindustry, 0 );
#endif
				}
			}
			Okay[2]++;
		}

// 'sector': 'Commercial Services'
		else if ( nsStrcmp ( tokens[0], "sector" ) == 0 )
		{
			xt++;
			if ( nsStrlen ( tokens[1] ) == 0 )
			{
				xsector.xid = UNKNOWN_SECTOR;
			}
			else
			{
				xl = imin ( nsStrlen(tokens[1]), sizeof(xsector.xsecname)-1 );

				sprintf ( WhereClause, "SECname = '%-*.*s'", xl, xl, tokens[1] );
				if ( LoadSector ( &MySql, WhereClause, &xsector, 0 ) != 1 )
				{
#ifndef AUTOADD
					printf ( "Cannot find sector %s, line %d\n", tokens[1], lineno );
					fflush ( stdout );
					Error[3]++;
					continue;
#else
					sprintf ( Statement, "insert into sector (SECname) value ( '%-*.*s' )", xl, xl, tokens[1] );
					if ( dbyInsert ( "invest", &MySql, Statement, 0, LogFileName ) != 1 )
					{
						printf ( "Cannot insert sector %s, line %d\n", tokens[1], lineno );
						fflush ( stdout );
						// xt = 0;
						Error[2]++;
						continue;
					}
					LoadSector ( &MySql, WhereClause, &xsector, 0 );
#endif
				}
			}
			Okay[3]++;
		}

// issueType:cs
		else if ( nsStrcmp ( tokens[0], "issueType" ) == 0 )
		{
			xt++;
            StockType = ReadStockType ( tokens[1] );
            if ( StockType == STYPE_OTHER )
            {
				Error[4]++;
                continue;
			}
			Okay[4]++;
		}

	
// 'country': 'none'
		else if ( nsStrcmp ( tokens[0], "country" ) == 0 )
		{
			xt++;
				
			DomesticForeign = 'F';
			for ( ndx = 0; ndx < MyCountryCount; ndx++ )
			{
				if ( nsStrncmp ( tokens[1], MyCountryArray[ndx].CountryDescription, MyCountryArray[ndx].Length ) == 0 )
				{
					DomesticForeign = 'D';
					break;
				}
			}

			if (( nsStrcmp ( tokens[1], "null" ) == 0 ) ||
				( nsStrcmp ( tokens[1], "US"   ) == 0 ))
			{
				sprintf ( CountryAlpha2, "US" );
// printf ( "%s -> %s\n", tokens[1], CountryAlpha2 );
			}
			else
			{
				if (( ptrCountry = LookupCountry(COUNTRY_BY_NAME,tokens[1])) == NULL )
				{
					HaveFund = 0;
				}
				else
				{
					snprintf ( CountryAlpha2, sizeof(CountryAlpha2), "%s", ptrCountry->Alpha2 );
// printf ( "%s -> %s\n", tokens[1], CountryAlpha2 );
				}
			}

			Okay[5]++;
		}


		if ( StockType == 'A' && DomesticForeign == 'D' )
		{
			DomesticForeign = 'F';

			printf ( "Override DomesticForeign to F for %s\n", xstock.xsticker );
		}

		/*----------------------------------------------------------
			update database
		----------------------------------------------------------*/
		if ( xt == 6 )
		{
			if ( nsStrcmp ( ptrExchange, "NASDAQ" ) == 0 )
			{
				StockNasdaq = 'Y';
			}
			else
			{
				StockNasdaq = 'N';
			}

			if (( xindustry.xid   != xstock.xsindustry              ) ||
				( xsector.xid     != xstock.xssector                ) ||
				( StockType       != xstock.xstype[0]               ) ||
				( StockNasdaq     != xstock.xsnasdaq[0]             ) ||
				( DomesticForeign != xstock.xsdomfor[0]             ) ||
				( nsStrcmp ( ptrExchange, xstock.xsexchange ) != 0 ))
			{
				snprintf ( Statement, sizeof(Statement),
					"update stock set Sindustry = %ld, Ssector = %ld, Sexchange = '%s', Snasdaq = '%c', Stype = '%c', Sdomfor = '%c' where Sticker = '%s'",
							xindustry.xid, xsector.xid, ptrExchange, StockNasdaq, StockType, DomesticForeign, xstock.xsticker );
#ifdef DEBUG
#define DEBUG
				printf ( "%s\n", Statement );
#else
				
				if ( dbyUpdate ( "invest", &MySql, Statement, 0, LogFileName ) != 1 )
				{
					printf ( "update failed: %s\n", Statement );
					fflush ( stdout );
					Error[6]++;
				}
				else
				{
					Okay[6]++;
				}
#endif
			}
			else
			{
				printf ( "No update needed for %s\n", xstock.xsticker );
			}

			if ( HaveFund == 1 && nsStrncmp ( xfundamental.xfcountry, CountryAlpha2, 2 ) != 0 )
			{
				snprintf ( Statement, sizeof(Statement),
					"update fundamental set Fcountry = '%s' where Fticker = '%s'", CountryAlpha2, xstock.xsticker );
#ifdef DEBUG
#define DEBUG
				printf ( "%s\n", Statement );
#else
				if ( dbyUpdate ( "invest", &MySql, Statement, 0, LogFileName ) != 1 )
				{
					printf ( "update failed: %s\n", Statement );
					fflush ( stdout );
				}
#endif
			}

			// xt = 0;
		}

	}

	nsFclose ( fp );

	printf ( "Stock    okay %4d, error %4d\n", Okay[0], Error[0] );
	printf ( "Exchange okay %4d, error %4d\n", Okay[1], Error[1] );
	printf ( "Industry okay %4d, error %4d\n", Okay[2], Error[2] );
	printf ( "Sector   okay %4d, error %4d\n", Okay[3], Error[3] );
	printf ( "Stype    okay %4d, skip  %4d\n", Okay[4], Error[4] );
	printf ( "Sdomfor  okay %4d, error %4d\n", Okay[5], Error[5] );
	printf ( "Update   okay %4d, error %4d\n", Okay[6], Error[6] );

	return ( 0 );
}
