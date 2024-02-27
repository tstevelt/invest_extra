/*----------------------------------------------------------------------------
	Program : SyncPortfolio/ProcessSchwab.c
	Author  : Tom Stevelt
	Date    : 2019 - 2024
	Synopsis: Process Schwab .csv file.
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

/*---------------------------------------------------------------------------
0      1           2        3     4             5             6            7           8           9          10         11 
Symbol,Description,Quantity,Price,Price Change ,Price Change ,Market Value,Day Change ,Day Change ,Cost Basis,Gain/Loss ,Gain/Loss ,

12     13                   14             15          16
Ratings,Reinvest Dividends?,Capital Gains?, Of Account,Security Type
----------------------------------------------------------------------------*/

#define		TOK_ACCT_NO		x
#define		TOK_ACCT_NAME	x

#define		TOK_SYMBOL		0
#define		TOK_DESC		1
#define		TOK_QTY			2
#define		TOK_PRICE		3
#define		TOK_CHANGE		4

#define		TOK_CURR		6
#define		TOK_TODA_GAIN	7
#define		TOK_TODAY_PCNT	8
#define		TOK_COST_BASIS	9
#define		TOK_TOTAL_GAIN	10
#define		TOK_TOTAL_PCNT	11
#define		TOK_PCNT_ACCT	15
#define		TOK_TYPE		16

#define		TOK_COST_SHARE	x

#include	"SyncPortfolio.h"

void ProcessSchwab ()
{
	while ( fgets ( xbuffer, sizeof(xbuffer), fp ) != (char *)0 )
	{
		lineno++;

		if (( tokcnt = GetTokensStd ( xbuffer, ",\n\r", tokens, MAXTOKS, 0 )) < 16 )
		{
			printf ( "Syntax error: line %d, tokcnt %d\n", lineno, tokcnt );
			Errors++;
			continue;
		}

		/*----------------------------------------------------------
			skip headers
		----------------------------------------------------------*/
		if (( rv = nsStrcmp ( tokens[TOK_SYMBOL], "Symbol" )) == 0 )
		{
			continue;
		}
		
		if ( nsStrncmp ( tokens[TOK_SYMBOL], "Cash", 4 ) == 0 )
		{
			tokens[TOK_SYMBOL] = "SCHWAB**";
		}

		if ( tokens[TOK_COST_BASIS][0] == '$' /* || tokens[TOK_COST_SHARE][0] == '$' */ )
		{
			printf ( "File contains dollar signs '$'\n" );
			exit ( 1 );
		}

		FilePortfolio.xpshares = nsAtof ( tokens[TOK_QTY] );
		if ( FilePortfolio.xpshares == 0 )
		{
			FilePortfolio.xpshares = nsAtof ( tokens[TOK_CURR] );
			FilePortfolio.xpprice  = 1.0;
		}
		else
		{
			FilePortfolio.xpprice  = nsAtof ( tokens[TOK_COST_BASIS] ) / FilePortfolio.xpshares;
		}

		if ( Debug )
		{
			printf (  "%d: [%s] [%s] [%s] [%s] [%s] [%s]\n",
				lineno,
				tokens[TOK_SYMBOL],
				tokens[TOK_DESC],
				tokens[TOK_QTY],
				tokens[TOK_CURR],
				tokens[TOK_COST_BASIS],
				/* tokens[TOK_COST_SHARE], */
				tokens[TOK_TYPE] );

			printf ( "%.2f shares, buyat %.2f\n", FilePortfolio.xpshares, FilePortfolio.xpprice );
		}

printf ( "\n" );

		sprintf ( WhereClause, "Sticker = '%s'", tokens[TOK_SYMBOL] );
		if ( LoadStock ( &MySql, WhereClause, &xstock, 0, 0 ) != 1 )
		{
			printf ( "%s %s not in stock table\n", tokens[TOK_SYMBOL], tokens[TOK_DESC] );

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
						tokens[TOK_SYMBOL], tokens[TOK_DESC], xstock.xstype[0] );

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
			printf ( "%s %s not in %s portfolio\n", tokens[TOK_SYMBOL], tokens[TOK_DESC], BrokerID  );

			printf ( "Do you want to add to portfolio? [Y/N] " );
			ans = GetValidChar ( "YN" );
			if ( ans == 'N' )
			{
				continue;
			}

			/*----------------------------------------------------------
				purchase date?
			----------------------------------------------------------*/
			printf ( "purchase date, yyyy-mm-dd " );

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
			printf ( "%s %s\n", tokens[TOK_SYMBOL], tokens[TOK_DESC] );
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
			printf ( "%s %s\n", tokens[TOK_SYMBOL], tokens[TOK_DESC] );
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

		if ( xstock.xstype[0] == 'B' )
		{
			BondValueFile      += nsAtof(tokens[TOK_CURR]) ;
			BondValuePortfolio += xportfolio.xpshares * 1000.0;
		}
	}

	nsFclose ( fp );
}
