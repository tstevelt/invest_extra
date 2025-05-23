/*----------------------------------------------------------------------------
	Program : SyncPortfolio/ProcessFidelity.c
	Author  : Tom Stevelt
	Date    : 2019 - 2024
	Synopsis: Process Fidelity .csv file
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
Account Number,Account Name,Symbol,Description,Quantity,Last Price,Last Price Change,Current Value,Today's Gain/Loss Dollar,Today's Gain/Loss Percent,Total Gain/Loss Dollar,Total Gain/Loss Percent,Percent Of Account,Cost Basis,Cost Basis Per Share,Type

Account Number,Account Name,Symbol,Description,Quantity,Last Price,Last Price Change,Current Value,Today's Gain/Loss Dollar,Today's Gain/Loss Percent,Total Gain/Loss Dollar,Total Gain/Loss Percent,Percent Of Account,Cost Basis Total,Average Cost Basis,Type

----------------------------------------------------------------------------*/

#define		TOK_ACCT_NO		0
#define		TOK_ACCT_NAME	1
#define		TOK_SYMBOL		2
#define		TOK_DESC		3
#define		TOK_QTY			4
#define		TOK_PRICE		5
#define		TOK_CHANGE		6
#define		TOK_CURR		7
#define		TOK_TODA_GAIN	8
#define		TOK_TODAY_PCNT	9
#define		TOK_TOTAL_GAIN	10
#define		TOK_TOTAL_PCNT	11
#define		TOK_PCNT_ACCT	12
#define		TOK_COST_BASIS	13
#define		TOK_COST_SHARE	14
#define		TOK_TYPE		15

#include	"SyncPortfolio.h"

void ProcessFidelity ()
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
		
		/*----------------------------------------------------------
			why did I skip CORE?
		----------------------------------------------------------*/
//		if ( nsStrncmp ( tokens[TOK_SYMBOL], "CORE", 4 ) == 0 )
//		{
//			continue;
//		}

		if ( tokens[TOK_COST_BASIS][0] == '$' || tokens[TOK_COST_SHARE][0] == '$' )
		{
			printf ( "File contains dollar signs '$'\n" );
			exit ( 1 );
		}

		if ( Debug )
		{
			printf (  "%d: %s %s %s %s %s %s\n",
				lineno,
				tokens[TOK_SYMBOL],
				tokens[TOK_DESC],
				tokens[TOK_QTY],
				tokens[TOK_COST_BASIS],
				tokens[TOK_COST_SHARE],
				tokens[TOK_TYPE] );
		}

printf ( "\n" );

		FilePortfolio.xpshares = nsAtof ( tokens[TOK_QTY] );
		if ( FilePortfolio.xpshares == 0 )
		{
			FilePortfolio.xpshares = nsAtof ( tokens[TOK_CURR] );
		}
		FilePortfolio.xpprice  = nsAtof ( tokens[TOK_COST_BASIS] ) / FilePortfolio.xpshares;

		sprintf ( WhereClause, "Sticker = '%s'", tokens[TOK_SYMBOL] );
		if ( LoadStock ( &MySql, WhereClause, &xstock, 0, 0 ) != 1 )
		{
			printf ( "%s %s not in stock table\n", tokens[TOK_SYMBOL], tokens[TOK_DESC] );

			printf ( "Do you want to add stock? [Y/N] " );
			fflush ( stdout );
			ans = GetValidChar ( "YN" );
			if ( ans == 'N' )
			{
				continue;
			}

			printf ( "Type: S)tock A)dr R)eit P)refer E)tf B)ond C)ash " );
			fflush ( stdout );
			/*----------------------------------------------------------
				not using STYPE_FX, STYPE_OTHER, STYPE_CRYPTO or STYPE_INDEX
			----------------------------------------------------------*/
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

//if ( strcmp ( tokens[TOK_SYMBOL], "WBA" ) ==  0 )
//{
//	printf ( "kilroy was here\n" );
//}

		if (( rv = LoadPortfolio ( &MySql, WhereClause, &xportfolio, 0 )) != 1 )
		{
			if ( rv > 1 )
			{
				printf ( "ERROR: %s %s is in portfolio %s\n", tokens[TOK_SYMBOL], tokens[TOK_DESC], BrokerID  );
				continue;
			}

			printf ( "%s %s not in %s portfolio\n", tokens[TOK_SYMBOL], tokens[TOK_DESC], BrokerID  );

			printf ( "Do you want to add to portfolio? [Y/N] " );
			fflush ( stdout );
			ans = GetValidChar ( "YN" );
			if ( ans == 'N' )
			{
				continue;
			}

			printf ( "security type, bucket, (C|F|E) " );
			fflush ( stdout );
			ans = GetValidChar ( "CFE" );
			switch ( ans )
			{
				case 'C':
					xportfolio.xpbucket = 1;
					break;

				case 'F':
					xportfolio.xpbucket = 2;
					break;

				case 'E':
					xportfolio.xpbucket = 3;
					break;

			}


			printf ( "broker account " );
			fflush ( stdout );
			strBroker[0] = '\0';
			while ( strBroker[0] == '\0' )
			{
				fgets ( strBroker, sizeof(strBroker), stdin );
				TrimRightAndLeft ( strBroker );
			}

			sprintf ( WhereClause, "Pmember = %ld and Pbroker = '%s'", xmember.xid, strBroker );
			if ( dbySelectCount ( &MySql, "portfolio", WhereClause, LogFileName ) == 0 )
			{
				printf ( "WARNING: UNKNOWN BROKER ACCOUNT\n" );
				printf ( "WhereClause: [%s]\n", WhereClause );
			}

			printf ( "purchase date [yyyy-mm-dd] " );
			fflush ( stdout );

			memset ( strPurchaseDate, 'x', sizeof(strPurchaseDate) );
			while ( StrToDatevalFmt ( strPurchaseDate, DATEFMT_YYYY_MM_DD, &dvPurchaseDate ) != 0 )
			{
				fgets ( strPurchaseDate, sizeof(strPurchaseDate), stdin );
				TrimRightAndLeft ( strPurchaseDate );
			}

			if ( xstock.xstype[0] == STYPE_BOND )
			{
				printf ( "maturity date [yyyy-mm-dd] " );
				fflush ( stdout );

				memset ( strMaturityDate, 'x', sizeof(strMaturityDate) );
				while ( StrToDatevalFmt ( strMaturityDate, DATEFMT_YYYY_MM_DD, &dvMaturityDate ) != 0 )
				{
					fgets ( strMaturityDate, sizeof(strMaturityDate), stdin );
					TrimRightAndLeft ( strMaturityDate );
				}

				sprintf ( Statement,
"insert into portfolio ( Pmember, Pticker, Pdate, Pshares, Pprice, Pbenchmark, Palerttype, Poptexp, Pbroker, Pbucket ) \
 values ( %ld, '%s', '%s', %f, %f, 'SPY', 'N', '%s', '%s', %d )",
						xmember.xid,
						tokens[TOK_SYMBOL],
						strPurchaseDate,
						FilePortfolio.xpshares,
						FilePortfolio.xpprice,
						strMaturityDate,
						strBroker,
						xportfolio.xpbucket );
			}
			else
			{
				sprintf ( Statement,
"insert into portfolio ( Pmember, Pticker, Pdate, Pshares, Pprice, Pbenchmark, Palerttype, Pbroker, Pbucket ) \
 values ( %ld, '%s', '%s', %f, %f, 'SPY', 'N', '%s', %d )",
						xmember.xid,
						tokens[TOK_SYMBOL],
						strPurchaseDate,
						FilePortfolio.xpshares,
						FilePortfolio.xpprice,
						strBroker,
						xportfolio.xpbucket );
			}

// printf ( "%s\n", Statement );

			rv = dbyInsert ( "invest", &MySql, Statement, 0, LogFileName );
			if ( rv != 1 )
			{
				printf ( "*** ERROR ADDING PORTFOLIO ***\n" );
				Errors++;
				continue;
			}

			sprintf ( WhereClause, "Pmember = %ld and Pticker = '%s'", xmember.xid, tokens[TOK_SYMBOL] );

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
			fflush ( stdout );
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

#ifdef PROMPT_COST_BASIS
			printf ( "  Do you want to fix cost basis? [Y/N] " );
			fflush ( stdout );
			ans = GetValidChar ( "YN" );
#else
			ans = 'Y';
#endif
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

		if ( xstock.xstype[0] == STYPE_BOND ) // if ( xstock.xstype[0] == 'B' )
		{
			BondValueFile      += nsAtof(tokens[TOK_CURR]) ;
			BondValuePortfolio += xportfolio.xpshares * 1000.0;
		}
	}

	nsFclose ( fp );
}
