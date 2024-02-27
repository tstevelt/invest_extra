/*----------------------------------------------------------------------------
	Program : SyncPortfolio/EachPortfolio.c
	Author  : Tom Stevelt
	Date    : 2019 - 2024
	Synopsis: load Array with portfolio ticker
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


#include	"SyncPortfolio.h"

int EachPortfolio ()
{
	if ( Count >= MAXPORTRECS )
	{
		printf ( "Exceeds MAXPORTRECS\n" );
		exit ( 1 );
	}

	sprintf ( Array[Count].Ticker, "%s", xportfolio.xpticker );
	Array[Count].OnFile = 0;
	Count++;

	return ( 0 );
}

int cmprec ( RECORD *a, RECORD *b )
{
	return ( nsStrcmp ( a->Ticker, b->Ticker ) );
}
