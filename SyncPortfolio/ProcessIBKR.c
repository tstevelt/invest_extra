/*----------------------------------------------------------------------------
	Program : SyncPortfolio/ProcessIBKR.c
	Author  : Tom Stevelt
	Date    : 2019 - 2024
	Synopsis: Process Interactive Brokers .csv file
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

#define		TOK_ACCT_NO		x
#define		TOK_ACCT_NAME	x
#define		TOK_DESC		x
#define		TOK_CHANGE		x
#define		TOK_TODA_GAIN	x
#define		TOK_TODAY_PCNT	x
#define		TOK_TOTAL_PCNT	x
#define		TOK_PCNT_ACCT	x
#define		TOK_TYPE		x
#define		TOK_COST_SHARE	x

#define		TOK_SYMBOL		0
#define		TOK_QTY			1
#define		TOK_PRICE		x
#define		TOK_COST_BASIS	2
#define		TOK_CURR		3
#define		TOK_TOTAL_GAIN	4

#include	"SyncPortfolio.h"

void ProcessIBKR ()
{
	while ( fgets ( xbuffer, sizeof(xbuffer), fp ) != (char *)0 )
	{
		lineno++;

		if (( tokcnt = GetTokensStd ( xbuffer, ",\n\r", tokens, MAXTOKS, 0 )) < 5 )
		{
			printf ( "Syntax error: line %d, tokcnt %d\n", lineno, tokcnt );
			Errors++;
			continue;
		}

		/*----------------------------------------------------------
			skip headers
		----------------------------------------------------------*/
		if (( rv = nsStrcmp ( tokens[TOK_SYMBOL], "symbol" )) == 0 )
		{
			continue;
		}
		
/*---------------------------------------------------------------------------
symbol,shares,cost,market,gain
CASH,372.750,1.00,1.00,0.0
XLC,12.000,78.398,947.420,6.640
XLK,4.000,201.525,820.180,14.080
XLY,5.000,175.679,891.630,13.230
----------------------------------------------------------------------------*/
		if ( nsStrncmp ( tokens[TOK_SYMBOL], "CASH", 4 ) == 0 )
		{
			tokens[TOK_SYMBOL] = "**IBKRCASH";
		}

		FilePortfolio.xpshares = nsAtof ( tokens[TOK_QTY] );
		FilePortfolio.xpprice  = nsAtof ( tokens[TOK_COST_BASIS] );

		if ( Debug )
		{
			printf (  "%d: [%s] [%s] [%s] [%s]\n",
				lineno,
				tokens[TOK_SYMBOL],
				tokens[TOK_QTY],
				tokens[TOK_CURR],
				tokens[TOK_COST_BASIS] );

			printf ( "%.2f shares, buyat %.2f\n", FilePortfolio.xpshares, FilePortfolio.xpprice );
		}

		printf ( "\n" );

		sprintf ( WhereClause, "Sticker = '%s'", tokens[TOK_SYMBOL] );
		if ( LoadStock ( &MySql, WhereClause, &xstock, 0, 0 ) != 1 )
		{
			printf ( "%s not in stock table\n", tokens[TOK_SYMBOL] );

			printf ( "Do you want to add stock? [Y/N] " );
			ans = GetValidChar ( "YN" );
			if ( ans == 'N' )
			{
				continue;
			}

			printf ( "Type: S)tock A)dr R)eit P)refer E)tf B)ond C)ash " );
			xstock.xstype[0] = GetValidChar ( "SARPEBC" );

			sprintf ( Statement,
				"insert into stock (Sticker, Sname, Stype) values ( '%s', '%-78.78s', '%c' )",
						tokens[TOK_SYMBOL], tokens[TOK_SYMBOL], xstock.xstype[0] );

// printf ( "%s\n", Statement );

			rv = dbyInsert ( "invest", &MySql, Statement, 0, LogFileName );
			if ( rv != 1 )
			{
				printf ( "*** ERROR ADDING STOCK ***\n" );
				Errors++;
				continue;
			}

			if ( LoadStock ( &MySql, WhereClause, &xstock, 0, 0 ) != 1 )
			{
				printf ( "*** ERROR CANNOT RELOAD NEWLY ADDED STOCK ***\n" );
				Errors++;
				continue;
			}
		}

		if ( xstock.xstype[0] == STYPE_BOND )
		{
			FilePortfolio.xpshares /= 1000.0;
			FilePortfolio.xpprice  *= 1000.0;
		}

		sprintf ( WhereClause, "Pmember = %ld and Pbroker like '%s%%' and Pticker = '%s'", 
					xmember.xid, BrokerID, tokens[TOK_SYMBOL] );

		if ( LoadPortfolio ( &MySql, WhereClause, &xportfolio, 0 ) != 1 )
		{
			/*----------------------------------------------------------
				add to portfolio?
			----------------------------------------------------------*/
			printf ( "%s not in %s portfolio\n", tokens[TOK_SYMBOL], BrokerID  );

			printf ( "Do you want to add to portfolio? [Y/N] " );
			ans = GetValidChar ( "YN" );
			if ( ans == 'N' )
			{
				continue;
			}

			/*----------------------------------------------------------
				purchase date?
			----------------------------------------------------------*/
			printf ( "purchase date [yyyy-mm-dd] " );

			memset ( strPurchaseDate, 'x', sizeof(strPurchaseDate) );
			while ( StrToDatevalFmt ( strPurchaseDate, DATEFMT_YYYY_MM_DD, &dvPurchaseDate ) != 0 )
			{
				fgets ( strPurchaseDate, sizeof(strPurchaseDate), stdin );
				TrimRightAndLeft ( strPurchaseDate );
			}

			/*----------------------------------------------------------
				broker?
			----------------------------------------------------------*/
			printf ( "broker " );
			strBroker[0] = '\0';
			while ( strBroker[0] == '\0' )
			{
				fgets ( strBroker, sizeof(strBroker), stdin );
				TrimRightAndLeft ( strBroker );
			}


			if ( xstock.xstype[0] == STYPE_BOND )
			{
				printf ( "maturity date [yyyy-mm-dd] " );

				memset ( strMaturityDate, 'x', sizeof(strMaturityDate) );
				while ( StrToDatevalFmt ( strMaturityDate, DATEFMT_YYYY_MM_DD, &dvMaturityDate ) != 0 )
				{
					fgets ( strMaturityDate, sizeof(strMaturityDate), stdin );
					TrimRightAndLeft ( strMaturityDate );
				}

				sprintf ( Statement,
"insert into portfolio ( Pmember, Pticker, Pdate, Pshares, Pprice, Pbroker, Pbenchmark, Palerttype, Poptexp ) values ( %ld, '%s', '%s', %f, %f, '%s', 'SPY', 'N', '%s' )",
						xmember.xid,
						tokens[TOK_SYMBOL],
						strPurchaseDate,
						FilePortfolio.xpshares,
						FilePortfolio.xpprice,
						strBroker,
						strMaturityDate );
			}
			else
			{
				sprintf ( Statement,
"insert into portfolio ( Pmember, Pticker, Pdate, Pshares, Pprice, Pbroker, Pbenchmark, Palerttype ) values ( %ld, '%s', '%s', %f, %f, '%s', 'SPY', 'N' )",
						xmember.xid,
						tokens[TOK_SYMBOL],
						strPurchaseDate,
						FilePortfolio.xpshares,
						FilePortfolio.xpprice,
						strBroker );
			}

// printf ( "%s\n", Statement );

			rv = dbyInsert ( "invest", &MySql, Statement, 0, LogFileName );
			if ( rv != 1 )
			{
				printf ( "*** ERROR ADDING PORTFOLIO ***\n" );
				Errors++;
				continue;
			}

			if ( LoadPortfolio ( &MySql, WhereClause, &xportfolio, 0 ) != 1 )
			{
				printf ( "*** ERROR CANNOT RELOAD NEWLY ADDED PORTFOLO RECORD ***\n" );
				Errors++;
				continue;
			}
		}
		else 
		{
			RECORD	*ptr, Key;

			sprintf ( Key.Ticker, "%s", xportfolio.xpticker );
			if (( ptr = bsearch ( &Key, Array, Count, sizeof(RECORD), (int(*)()) cmprec )) == (RECORD *)0 )
			{
				printf ( "*** ERROR bsearch() FAILED ON TICKER %s ***\n", Key.Ticker );
				Errors++;
				exit ( 1 );
			}

			ptr->OnFile = 1;
		}

		if ( FilePortfolio.xpshares > 1.0 && dbldiff ( FilePortfolio.xpshares, xportfolio.xpshares, 0.0001 ))
		{
			printf ( "%s\n", tokens[TOK_SYMBOL] );
			printf ( "  SHARES PORTFOLIO %.4f != FILE %.4f\n", xportfolio.xpshares, FilePortfolio.xpshares );

			printf ( "  Do you want to fix share count? [Y/N] " );
			ans = GetValidChar ( "YN" );
			if ( ans == 'Y' )
			{
				sprintf ( Statement,
					"update portfolio set Pshares = %f where id = %ld", FilePortfolio.xpshares, xportfolio.xid );
// printf ( "%s\n", Statement );

			rv = dbyUpdate ( "invest", &MySql, Statement, 0, LogFileName );
			if ( rv != 1 )
			{
				printf ( "*** ERROR UPDATING PORTFOLIO SHARES***\n" );
				Errors++;
				continue;
			}
			}
		}

		if ( FilePortfolio.xpprice > 0.1 && dbldiff ( FilePortfolio.xpprice, xportfolio.xpprice, 0.0001 ))
		{
			printf ( "%s\n", tokens[TOK_SYMBOL] );
			printf ( "  COST/SHARE PORTFOLIO %.4f != FILE %.4f\n", xportfolio.xpprice, FilePortfolio.xpprice );

			printf ( "  Do you want to fix cost basis? [Y/N] " );
			ans = GetValidChar ( "YN" );
			if ( ans == 'Y' )
			{
				sprintf ( Statement,
					"update portfolio set Pprice = %f where id = %ld", FilePortfolio.xpprice, xportfolio.xid );
// printf ( "%s\n", Statement );

				rv = dbyUpdate ( "invest", &MySql, Statement, 0, LogFileName );
				if ( rv != 1 )
				{
					printf ( "*** ERROR UPDATING PORTFOLIO COST BASIS***\n" );
					Errors++;
					continue;
				}
			}
		}

		if ( xstock.xstype[0] == STYPE_BOND )
		{
			BondValueFile      += nsAtof(tokens[TOK_CURR]) ;
			BondValuePortfolio += xportfolio.xpshares * 1000.0;
		}
	}

	nsFclose ( fp );
}
