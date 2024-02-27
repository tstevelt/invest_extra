/*----------------------------------------------------------------------------
	Program : getnews/EachStock.c
	Author  : Tom Stevelt
	Date    : 2019 - 2024
	Synopsis: for each stock, get some news articles from IEX
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

#include	"getnews.h"

static int FindClosingQuote ( char *String )
{
	int		xl, xi;
	char	*cp, *prev;

	for ( cp = String, xl = 0, xi = 0; *cp; cp++, xl++, xi++ )
	{
		if ( *cp == '"' )
		{
			if ( xi > 0 && *prev != '\\' )
			{
				break;
			}
		}
		prev = cp;
	}
	
	return ( xl );
}

int EachStock ( char Ticker[], long StockDateTime )
{
	char	cmdline[1024];
	char	JsonFileName[256];
	char	NewsFileName[256];
	FILE	*fpNews;
	char	*buffer;
	JSON_NAME_VALUE		*ptrNameValue;
	int		xl;
	long	longDateTime;
	char	strDateTime[11];

	StockCount++;

	xl = lastchr ( Ticker, nsStrlen(Ticker) );

	Ticker[xl] = '\0';

	sprintf ( JsonFileName, "%s/%s_news.json", TEMPDIR, Ticker );
	sprintf ( NewsFileName, "%s/%s_news.txt",  WEBSTATEDIR, Ticker );
	
	if ( Debug )
	{
		printf ( "%s\n", JsonFileName );
	}


	sprintf ( cmdline, "curl -s '%s/stock/%s/news/last/1?token=%s' > %s",
			env_ApiURL, Ticker, env_ApiKey, JsonFileName );

	/*---------------------------------------------------------------------------
		this shit doesn't even work right even if it did what I want.
		/var/local/invest/TestCloud.sh
		curl -s "$BASEURL/time-series/news/$2?from=$4&first=1&sort=DESC&token=$TOKEN" 

xxxx	sprintf ( strDateTime, "%s", DateYYYY_MM_DD( StockDateTime ) );
xxx	sprintf ( cmdline, 
xxx		"curl -s '%s/time-series/news/%s?from=%s&first=1&sort=DESC&token=%s' > %s",
xxx				env_ApiURL, Ticker, strDateTime, env_ApiKey, JsonFileName );
	---------------------------------------------------------------------------*/

	if ( Debug )
	{
		printf ( "%s\n", cmdline );
	}

	IEX_RateLimit ( 0 );

	/*----------------------------------------------------------
		fixit - still using curl system() instead of library.
	----------------------------------------------------------*/

	system ( cmdline );
	RetrieveCount++;

	if (( buffer = JsonOpenFileAndRead ( JsonFileName )) == NULL )
	{
		if ( Debug )
		{
			printf ( "JsonOpenFileAndRead NULL on %s\n", JsonFileName );
		}
		ErrorCount++;
		return ( 0 );
	}

	if ( nsStrncmp ( buffer, "[]", 2 ) == 0 )
	{
		if ( Debug )
		{
			printf ( "No news on file for %s\n", Ticker );
		}
		ErrorCount++;
	}
	else
	{
/*---------------------------------------------------------------------------
             1234567890
[{"datetime":1626956695000
"headline":"International Business Machines Corp. stock rises Thursday
outperforms market"
"source":"MarketWatch"
"url":"https://cloud.iexapis.com/v1/news/article/1daXoRtkAFdLFnkJQpys9RwKu6z2w47svFtbJmCU46h1"
"summary":"International Business Machines Corp. stock rises Thursday
outperforms market MarketWatch"
"related":"IBM
IBM-LN"
"image":"https://cloud.iexapis.com/v1/news/image/1daXoRtkAFdLFnkJQpys9RwKu6z2w47svFtbJmCU46h1"
"lang":"en"
"hasPaywall":false}]
---------------------------------------------------------------------------*/
		if (( fpNews = fopen ( NewsFileName, "w" )) == NULL )
		{
			fprintf ( stderr, "Cannot create %s\n", NewsFileName );
			exit ( 1 );
		}

		strDateTime[0] = '\0';

		if ( Debug )
		{
			printf ( "ticker %s\n", Ticker );
		}
		fprintf ( fpNews, "ticker %s\n", Ticker );
		if (( ptrNameValue = JsonScan ( buffer, "datetime", JSON_FIRST )) != NULL )
		{
			if ( Debug )
			{
				printf ( "datetime %-10.10s\n", ptrNameValue->Value );
			}
			fprintf ( fpNews, "datetime %-10.10s\n", ptrNameValue->Value );
			sprintf ( strDateTime, "%-10.10s", ptrNameValue->Value );
		}
		if (( ptrNameValue = JsonScan ( buffer, "summary", JSON_FIRST )) != NULL )
		{
			xl = FindClosingQuote ( ptrNameValue->Value );
			if ( Debug )
			{
				printf ( "summary %*.*s\n", xl, xl, ptrNameValue->Value );
			}
			fprintf ( fpNews, "summary %*.*s\n", xl, xl, ptrNameValue->Value );
		}
		if (( ptrNameValue = JsonScan ( buffer, "url", JSON_FIRST )) != NULL )
		{
			xl = FindClosingQuote ( ptrNameValue->Value );
			if ( Debug )
			{
				printf ( "url %*.*s\n", xl, xl, ptrNameValue->Value );
			}
			fprintf ( fpNews, "url %*.*s\n", xl, xl, ptrNameValue->Value );
		}
		if (( ptrNameValue = JsonScan ( buffer, "source", JSON_FIRST )) != NULL )
		{
			xl = FindClosingQuote ( ptrNameValue->Value );
			if ( Debug )
			{
				printf ( "source %*.*s\n", xl, xl, ptrNameValue->Value );
			}
			fprintf ( fpNews, "source %*.*s\n", xl, xl, ptrNameValue->Value );
		}
		if ( Debug )
		{
			printf ( "\n" );
		}
		fprintf ( fpNews, "\n" );

		nsFclose ( fpNews );

		longDateTime = nsAtol ( strDateTime );
		if ( longDateTime > StockDateTime )
		{
			sprintf ( Statement, 
				"update stock set Snews = %ld where Sticker = '%s'", longDateTime, Ticker );

			dbyUpdate ( "getnews", &MySql, Statement, 0, LogFileName );
			UpdateCount++;
		}
	}

	free ( buffer );

	return ( 0 );
}
