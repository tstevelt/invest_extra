/*----------------------------------------------------------------------------
	Program : getfundIEX/EachStock.c
	Author  : Tom Stevelt
	Date    : 2019 - 2024
	Synopsis: for each stock, get data from IEX and update database.
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

#include	"getfundIEX.h"

int EachStock ( XSTOCK *ptr )
{
	char	*TempFileName = "/var/local/tmp/getfundIEX.json";
	FILE	*tfp;
	char	xbuffer[4096];
	char	zbuffer[4096];
	int		xq, xi, xo, xl;
#define		MAXTOKS		200
	char	*tokens[MAXTOKS];
	int		tokcnt, xt;
	char	Fragment[128];
	int		FragCount;
	double	NewEPS, CheckEPS, Percent;
	double	NewPE;
	double	NewShares;
	double	NewMktCap;
	double	NewEntVal;
	double	NewValue;
	int		numread;
	int		GotShares, GotEPS, GotPE;

	StockCount++;
	
	sprintf ( WhereClause, "Fticker = '%s'", ptr->xsticker );

	if (LoadFundamental ( &MySql, WhereClause, &xfundamental, 0 ) < 1 )
	{
		printf ( "%s no fundamental\n", ptr->xsticker );
		ErrorCount++;
		return ( 0 );
	}

	sprintf ( cmdline, "%s/stock/%s/stats?token=%s" , env_ApiURL, ptr->xsticker, env_ApiKey );

	if ( Debug || Verbose )
	{
		printf ( "%s\n", cmdline );
	}

	IEX_RateLimit ( 0 );

	if (( tfp = fopen ( TempFileName, "w" )) == (FILE *)0 )
	{
		ErrorCount++;
		return ( -1 );
	}

	curl_easy_setopt ( curl, CURLOPT_URL, cmdline );
	curl_easy_setopt ( curl, CURLOPT_WRITEDATA, tfp );

	if (( curlRV = curl_easy_perform ( curl )) != CURLE_OK )
	{
		fprintf ( stderr, "perform failed: %s\n", curl_easy_strerror(curlRV) );
		exit ( 1 );
	}

	nsFclose ( tfp );

	if (( tfp = fopen ( TempFileName, "r" )) == (FILE *)0 )
	{
		if ( Debug )
		{
			printf ( "fopen failed\n" );
		}
		ErrorCount++;
		return ( -1 );
	}

/*---------------------------------------------------------------------------
{"companyName"	"Berkshire Hathaway Inc."
"marketcap"	770706830784
"week52high"	373.34
"week52low"	292.42
"week52highSplitAdjustOnly"	373.34
"week52lowSplitAdjustOnly"	292.42
"week52change"	0.1523874800091387
"sharesOutstanding"	1308414093
"float"	0
"avg10Volume"	3544674
"avg30Volume"	3099180
"day200MovingAvg"	348.49
"day50MovingAvg"	351.03
"employees"	383000
"ttmEPS"	52827.09
"ttmDividendRate"	0
"dividendYield"	0
"nextDividendDate"	""
"exDividendDate"	""
"nextEarningsDate"	"2024-02-24"
"peRatio"	10.033546805668834
"beta"	0.7366899279919928
"maxChangePercent"	3.8156028368794326
"year5ChangePercent"	0.7233502538071066
"year2ChangePercent"	0.24446637529959103
"year1ChangePercent"	0.15389391810189856
"ytdChangePercent"	0.1430236322434446
"month6ChangePercent"	0.05098973061467471
"month3ChangePercent"	-0.02772958832438388
"month1ChangePercent"	0.019578400231013537
"day30ChangePercent"	0.019578400231013537
"day5ChangePercent"	-0.011174279553028854}
---------------------------------------------------------------------------*/

	// fixit - should all be in one line, but don't assume.  read entire file at once.
	GotShares = GotEPS = 0;
	// size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
	numread = fread ( xbuffer, 1, sizeof(xbuffer), tfp );
	if ( numread == 0 )
	{
		printf ( "fread failed, numread %d, error %s\n", numread, strerror(errno) );
		nsFclose ( tfp );
		return ( 0 );
	}

	xl = strlen ( xbuffer );
	for ( xq = 0, xi = 0, xo = 0; xi < xl; xi++ )
	{
		switch ( xbuffer[xi] )
		{
			case '"':
				if ( xq == 0 ) 
				{
					xq = 1;
				}
				else
				{
					xq = 0;
				}
				continue;
			case ',':
				if ( xq )
				{
					xbuffer[xi] = ' ';
				}
				break;
		}
		zbuffer[xo++] = xbuffer[xi];
		zbuffer[xo] = '\0';
	}
	// printf ("%d: %s\n", lineno, xbuffer );
	// printf ("%d: %s\n", lineno, zbuffer );

	sprintf ( Statement, "update fundamental set" );
	FragCount = 0;

	tokcnt = GetTokensD ( zbuffer, ",\n\r", tokens, MAXTOKS );
	for ( xt = 0; xt < tokcnt; xt++ )
	{
		// printf ( "%s\n", tokens[xt] );

		if ( strncmp ( tokens[xt], "sharesOutstanding:", 18 ) == 0 )
		{
			/*----------------------------------------------------------
			-->> sharesOutstanding:183662681
			done -->> Fshares = 38100000000,
			done -->> Fmktcap = 9081897000000,
			----------------------------------------------------------*/
			NewShares = atol ( &tokens[xt][18] );
			GotShares = 1;

			// printf ( "%.0f %ld\n", NewShares, xfundamental.xfshares );

			if ( NewShares != xfundamental.xfshares ) 
			{
				NewMktCap = NewShares * ptr->xsclose;
				
				sprintf ( Fragment, "%sFshares = %.2f, Fmktcap = %.2f",
						FragCount ? ", " : " ",
						NewShares, NewMktCap );

				strcat ( Statement, Fragment );
				FragCount++;
	
			}
			else
			{
				NewMktCap = xfundamental.xfshares * ptr->xsclose;
			}
		}
		else if ( strncmp ( tokens[xt], "ttmEPS:", 7 ) == 0 )
		{
			/*----------------------------------------------------------
			-->> ttmEPS:7.92
			-->> Fepsttm = 0.00,
			----------------------------------------------------------*/
			NewEPS = atof( &tokens[xt][7] );
			GotEPS = 1;
		}
		else if ( strncmp ( tokens[xt], "peRatio:", 8 ) == 0 )
		{
			NewPE = atof ( &tokens[xt][8] );
			GotPE = 1;
		}
		else if ( strncmp ( tokens[xt], "ttmDividendRate:", 16 ) == 0 )
		{
			/*----------------------------------------------------------
			-->> ttmDividendRate:1.5732263851411514
			----------------------------------------------------------*/
			NewValue = atof( &tokens[xt][16] );
			if ( fabs(NewValue-xfundamental.xfdividend) > 0.005 )
			{
				sprintf ( Fragment, "%sFdividend = %.2f, Fyield = %.2f", 
					FragCount ? ", " : " ",
					NewValue,
					100.0 * NewValue / ptr->xsclose );

				strcat ( Statement, Fragment );
				FragCount++;
			}
		}
	}

	if ( GotEPS && GotPE )
	{
		CheckEPS = ptr->xsclose / NewPE;
		Percent = 100.0 * (NewEPS - CheckEPS) / CheckEPS;

		if ( fabs ( Percent ) > 25.0 )
		{
			sprintf ( WhereClause, "Pticker = '%s'",  ptr->xsticker );
			if ( dbySelectCount (  &MySql, "portfolio", WhereClause, LogFileName ) > 0 )
			{
				printf ( "%s Close %.2f Fpettm %.2f -> NewPE %.2f, CheckEPS %.2f -> NewEPS %.2f = %.2f%%\n", 
					ptr->xsticker, ptr->xsclose, xfundamental.xfpettm, NewPE, CheckEPS, NewEPS, Percent );
			}
		}

		if ( NewEPS > ptr->xsclose )
		{
			ErrorCount++;
			printf ( "Fixing crazy EPS %.2f on %s\n", NewEPS, ptr->xsticker );
			NewEPS = ptr->xsclose * NewPE / 100.0;
		}
		xfundamental.xfpettm = NewPE;

		if ( dbldiff(NewEPS,xfundamental.xfepsttm,0.01) )
		{
			sprintf ( Fragment, "%sFepsttm = %.2f, Fpettm = %.2f", 
				FragCount ? ", " : " ",
				NewEPS,
				xfundamental.xfpettm );

			strcat ( Statement, Fragment );
			FragCount++;
		}
	}

	if ( GotShares && GotEPS && xfundamental.xfreturna == 0 && xfundamental.xftotasst > 0 )
	{
		NewValue = 100.0 * NewEPS*NewShares/xfundamental.xftotasst;

		if ( fabs ( NewValue - xfundamental.xfreturna ) > 0.005 )
		{
			sprintf ( Fragment, "%sFreturnA = %.2f", 
				FragCount ? ", " : " ",
				NewValue );

			strcat ( Statement, Fragment );
			FragCount++;
		}
	}

	if ( GotShares )
	{
		double	NewBVPS, NewPB;

		if ( xfundamental.xfequity > 0 && NewShares > 0 )
		{
			// Add Fequity to calc Fbvps to calc Fpb
			NewBVPS = xfundamental.xfequity / NewShares;
			NewPB = ptr->xsclose / NewBVPS;;
		}
		else
		{
			NewBVPS = 0.0;
			NewPB = 0.0;
		}

		if (( fabs ( xfundamental.xfbvps - NewBVPS ) > 1 ) ||
			( fabs ( xfundamental.xfpb - NewPB ) > 0.01 ))
		{
			sprintf ( Fragment, "%sFbvps = %.2f, Fpb = %.2f",
				FragCount ? ", " : " ",
				NewBVPS, 
				NewPB );

			strcat ( Statement, Fragment );
			FragCount++;
		}

		if ( ptr->xstype[0] != STYPE_ETF ) // if ( ptr->xstype[0] != 'E' )
		{
			// EnterpriseValue = MarketCap + MinorityInterest + LongTermDebtNoncurrent + OperatingLeaseLiabilityCurrent + 
			//                   AccountsPayableCurrent - CashEquiv

			NewEntVal = NewMktCap + xfundamental.xfminority + xfundamental.xfdebtnc + xfundamental.xfliab + xfundamental.xfpayables - xfundamental.xfcash;
			if ( xfundamental.xfebitda > 0 )
			{
				NewValue = NewEntVal / xfundamental.xfebitda;
			}
			else
			{
				NewValue = 0.0;
			}

			if ( Debug )
			{
				printf ( "-- enterprise value calc --\n" );
				printf ( "mkt cap %16.0f\n", NewMktCap );
				printf ( "min int %16.0f\n", xfundamental.xfminority );
				printf ( "debt nc %16.0f\n", xfundamental.xfdebtnc );
				printf ( "liab    %16.0f\n", xfundamental.xfliab );
				printf ( "payble  %16.0f\n", xfundamental.xfpayables );
				printf ( "cash    %16.0f\n", xfundamental.xfcash );
				printf ( "ent val %16.0f\n", NewEntVal );
				printf ( "ebitda  %16.0f\n", (double) xfundamental.xfebitda );
				printf ( "value   %19.2f\n\n", NewValue );
			}
		}
		else
		{
			NewEntVal = NewValue = 0.0;
		}

		if (( fabs ( NewEntVal - xfundamental.xfentval ) > 1   ) ||
			( fabs ( NewValue  - xfundamental.xfvalue  ) > 0.1 ))
		{
			sprintf ( Fragment, "%sFentval = %.0f, Fvalue = %.2f",
				FragCount ? ", " : " ",
				NewEntVal,
				NewValue );

			strcat ( Statement, Fragment );
			FragCount++;
		}


		if ( xfundamental.xfliab > 1 )
		{
			double		MarketableSecurities;

			// QuickRatio = ( AssetsCurrent + CashEquiv + MarketableSecurities) / (Liabilities )

			/*----------------------------------------------------------
				don't dilute the market by more than five percent.
			----------------------------------------------------------*/
			MarketableSecurities = ptr->xsclose * dmin ( xfundamental.xftreasury, NewShares / 20.0 );

			NewValue = (xfundamental.xfcurasst + xfundamental.xfcash + MarketableSecurities) / xfundamental.xfliab;

			if ( Debug )
			{
				printf ( "-- quick ratio calc --\n" );
				printf ( "treasury   %16.0f\n", xfundamental.xftreasury );
				printf ( "outstand   %16.0f\n", NewShares );
				printf ( "price      %16.2f\n", ptr->xsclose );
				printf ( "marketable %16.0f\n", MarketableSecurities );
				printf ( "curr asst  %16.0f\n", xfundamental.xfcurasst );
				printf ( "cash equiv %16.0f\n", xfundamental.xfcash );
				printf ( "liab       %16.0f\n", xfundamental.xfliab );
				printf ( "quick      %19.2f\n\n", NewValue );
			}

		}
		else
		{
			NewValue  = 0;
		}

		if ( fabs ( NewValue - xfundamental.xfquick  ) > 0.01 )
		{
			sprintf ( Fragment, "%sFquick = %.2f",
				FragCount ? ", " : " ",
				NewValue );

			strcat ( Statement, Fragment );
			FragCount++;
		}

	}

	if ( FragCount == 0 )
	{
		NoChangeCount++;
		return ( 0 );
	}

	sprintf ( Fragment, ", Fupdated = %ld where Fticker = '%s'",  StartTime, xfundamental.xfticker );
	strcat ( Statement, Fragment );

	if ( Verbose || Debug )
	{
		printf ( "%s\n", Statement );
	}

	if ( UpdateDB )
	{
		if ( dbyUpdate ( "invest", &MySql, Statement, 0, LogFileName ) != 1 )
		{
			printf ( "dbyUpdate failed: %s\n", Statement );	
			ErrorCount++;
		}
		else
		{
			UpdateCount++;
		}
	}

	nsFclose ( tfp );
	return ( 0 );
}
