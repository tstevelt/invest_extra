/*----------------------------------------------------------------------------
	Program : UpdatePorthist/EachMember.c
	Author  : Tom Stevelt
	Date    : 2019 - 2024
	Synopsis: Update/Insert members porthist record for given date.
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

#include	"UpdatePorthist.h"

int EachMember ()
{
	long	rv;
	DBY_QUERY	*Query = NULL;
	double	ThisInvest, TotalInvest, TotalValue;
	double	EquityValue, BondValue, CashValue;

	if ( Debug )
	{
		printf ( "Processing %s\n", xmember.xmname );
	}

	VisitCount++;

	/*----------------------------------------------------------
		warn if any purchases made after history date
	----------------------------------------------------------*/
	sprintf ( WhereClause, "Pmember = %ld and Pdate > '%s'", xmember.xid, ascHistDate );
	rv = dbySelectCount ( &MySql, "portfolio", WhereClause, LogFileName );

	if ( rv > 0 )
	{
		printf ( "WARNING: %s has %ld holdings purchased after %s\n",
			xmember.xmname, rv, ascHistDate );
		WarnCount++;
	}

	TotalInvest = TotalValue = 0.0;
	
	/*----------------------------------------------------------
		get value of equities
	----------------------------------------------------------*/
	sprintf ( Statement, "select sum(Pshares * Pprice), sum(Pshares * Hclose) \
  from portfolio, history \
 where Pticker = Hticker \
   and Pmember = %ld \
   and Hdate = '%s'",
   		xmember.xid,
		ascHistDate );

	Query = dbySelect ( "invest", &MySql, Statement, LogFileName );

	if ( Query != NULL )
	{
		if ( Query->NumRows > 0 )
		{
			if (( Query->EachRow = mysql_fetch_row ( Query->Result )) != NULL )
			{
				ThisInvest = nsAtof ( Query->EachRow[0] );
				TotalInvest += ThisInvest;
				EquityValue = nsAtof ( Query->EachRow[1] );
				TotalValue += EquityValue;
			}
		}
	}

	/*----------------------------------------------------------
		get value of bonds
	----------------------------------------------------------*/
	sprintf ( Statement, "select sum(Pshares * Pprice), sum(Pshares * 1000.0) \
  from portfolio, stock \
 where Pticker = Sticker \
   and Pmember = %ld \
   and Stype = 'B'",
   		xmember.xid );

	Query = dbySelect ( "invest", &MySql, Statement, LogFileName );

	if ( Query != NULL )
	{
		if ( Query->NumRows > 0 )
		{
			if (( Query->EachRow = mysql_fetch_row ( Query->Result )) != NULL )
			{
				ThisInvest = nsAtof ( Query->EachRow[0] );
				TotalInvest += ThisInvest;
				BondValue = nsAtof ( Query->EachRow[1] );
				TotalValue += BondValue;
			}
		}
	}

	/*----------------------------------------------------------
		get value of cash
	----------------------------------------------------------*/
	sprintf ( Statement, "select sum(Pshares * Pprice), sum(Pshares * Pprice) \
  from portfolio, stock \
 where Pticker = Sticker \
   and Pmember = %ld \
   and Stype = 'C'",
   		xmember.xid );

	Query = dbySelect ( "invest", &MySql, Statement, LogFileName );

	if ( Query != NULL )
	{
		if ( Query->NumRows > 0 )
		{
			if (( Query->EachRow = mysql_fetch_row ( Query->Result )) != NULL )
			{
				ThisInvest = nsAtof ( Query->EachRow[0] );
				TotalInvest += ThisInvest;
				CashValue = nsAtof ( Query->EachRow[1] );
				TotalValue += CashValue;
			}
		}
	}

	if ( Debug )
	{
		printf ( "Invest %.2f  Values %.2f  %.2f  %.2f Total %.2f\n", TotalInvest, EquityValue, BondValue, CashValue, TotalValue );
		// return ( 0 );
	}

	if ( TotalValue == 0.0 )
	{
		return ( 0 );
	}

	sprintf ( WhereClause, "PHmember = %ld and PHdate = '%s'", xmember.xid, ascHistDate );
	rv = dbySelectCount ( &MySql, "porthist", WhereClause, LogFileName );
	if ( rv == 0 )
	{
		sprintf ( Statement, 
"insert into porthist (PHmember, PHdate, PHinvest, PHequity, PHbonds, PHcash) values ( %ld, '%s', %.2f, %.2f, %.2f, %.2f)",
			xmember.xid, ascHistDate, TotalInvest, EquityValue, BondValue, CashValue  );
		
		if ( Debug )
		{
			printf ( "%s\n", Statement );
		}

		rv = dbyInsert ( "invest", &MySql, Statement, 0, LogFileName );
		if ( rv == 0 )
		{
			printf ( "ERROR: insert failed on:\n%s\n", Statement );
			ErrorCount++;
		}
		else
		{
			InsertCount++;
		}
	}
	else
	{
		sprintf ( Statement, 
"update porthist set PHinvest = %.2f, PHequity = %.2f, PHbonds = %.2f, PHcash = %.2f where PHmember = %ld and PHdate = '%s'",
			TotalInvest, EquityValue, BondValue, CashValue, xmember.xid, ascHistDate );

		if ( Debug )
		{
			printf ( "%s\n", Statement );
		}

		rv = dbyUpdate ( "invest", &MySql, Statement, 0, LogFileName );
		if ( rv == 0 )
		{
			printf ( "WARNING: no update on:\n%s\n", Statement );
			WarnCount++;
		}
		else
		{
			UpdateCount++;
		}
	}

	return ( 0 );
}
