/*----------------------------------------------------------------------------
	Program : SyncPortfolio.c
	Author  : Tom Stevelt
	Date    : Apr 2022
	Synopsis: Broker account position files are downloaded (as .csv) and 
				pre-processed on local computer.  Then uploaded to 
				server and sync'ed with this program.
	Return  : 

	Who		Date		Modification
	---------------------------------------------------------------------
	tms		01/05/2023	Added Bond File vs Portfolio analysis. Still off
						if there are duplicate bonds between IRA and TOD.
	tms		01/04/2024	Added BROKER ID for Schwab
	tms		02/08/024	Added IBKR

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
#include	"SyncPortfolio.h"

char GetValidChar ( char *ValidChars )
{
	char		answer;

	answer = ' ';

	while ( strchr ( ValidChars, answer ) == (char *)0 )
	{
		answer = toupper(getc(stdin));
	}

	return ( answer );
}

// fixit char GetAnswer ( char *Question, *ValidChars ) { }

int main ( int argc, char *argv[] )
{
	getargs ( argc, argv );

	if (( fp = fopen ( Filename, "r" )) == (FILE *)0 )
	{
		printf ( "Cannot open file %s for input\n", Filename );
		exit ( 1 );
	}

	printf ( "Processing %s\n", Filename );

	sprintf ( Today, "%s", fmtGetTodayDate ( DATEFMT_YYYY_MM_DD ) );
	
	StartMySQL ( &MySql, "invest" );

	sprintf ( WhereClause, "Pmember = %ld and Pbroker like '%s%%'", xmember.xid, BrokerID );
	rv = LoadPortfolioCB ( &MySql, WhereClause, NULL, &xportfolio, EachPortfolio, 0 );
	if ( Debug )
	{
		printf ( "Loaded %d portfolio records\n", rv );
	}

	qsort ( Array, Count, sizeof(RECORD), (int(*)()) cmprec );

	Errors = NotInDB = NoChange = TotalSold = TotalBought = lineno = 0;
//	Profit = TotalProfit = 0.0;

	if ( strcmp ( BrokerID, "FID" ) == 0 )
	{
		ProcessFidelity ();
	}
	else if ( strcmp ( BrokerID, "SCHW" ) == 0 )
	{
		ProcessSchwab ();
	}
	else if ( strcmp ( BrokerID, "IBKR" ) == 0 )
	{
		ProcessIBKR ();
	}
	else
	{
		printf ( "Bad Broker ID %s\n", BrokerID );
		exit ( 1 );
	}

	for ( ndx = 0; ndx < Count; ndx++ )
	{
		if ( Array[ndx].OnFile == 0 )
		{
			printf ( "Remove %s from portfolio? [Y/N] ", Array[ndx].Ticker );
			ans = GetValidChar ( "YN" );
			if ( ans == 'Y' )
			{
				sprintf ( Statement,
					"delete from portfolio where Pmember = %ld and Pticker = '%s'", xmember.xid, Array[ndx].Ticker );

				rv = dbyDelete ( "invest", &MySql, Statement, 0, LogFileName );
				if ( rv != 1 )
				{
					printf ( "*** ERROR DELETING %s FROM PORTFOLIO ***\n", Array[ndx].Ticker );
					printf ( "%s\n", Statement );
					Errors++;
					continue;
				}
			}
		}
	}

	printf ( "Read      %d\n", lineno );
//	printf ( "No in DB  %d\n", NotInDB );
//	printf ( "No change %d\n", NoChange );
//	printf ( "Sold      %d\n", TotalSold );
//	printf ( "Bought    %d\n", TotalBought );
	printf ( "Errors    %d\n", Errors );

	printf ( "BondValueFile %.2f, BondValuePortfolio %.2f, Diff %.2f\n",
		BondValueFile, BondValuePortfolio, BondValuePortfolio - BondValueFile);

	return ( 0 );
}
