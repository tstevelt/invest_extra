/*----------------------------------------------------------------------------
	Program : CheckExpire/EachPortfolio.c
	Author  : Tom Stevelt
	Date    : 2019 - 2024
	Synopsis: For each stock held in portfolio, check trailing stop, if set.
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

#include	"CheckExpire.h"

int EachPortfolio ()
{
	int		rv;
	char	SlashDate[12];

	sprintf ( WhereClause, "Sticker = '%s'", xportfolio.xpticker );
	if (( rv = LoadStock ( &MySql, WhereClause, &xstock, 0, 0 )) != 1 )
	{
		printf ( "LoadStock %s returned %d\n", xportfolio.xpticker, rv );
		return ( 0 );
	}

/*----------------------------------------------------------
Pmember = 501 and Poptexp like '20%'
0123456789
2024-08-07 BANK OF AMERICA NA CD 5.00000% 08/07/2024                                     
2027-02-28 BANK OF AMERICA NA CD 4.85000% 02/08/2027                                     
2025-06-13 FEDERAL HOME LOAN BA SER YK-2025 3.12500% 06/13/2025                          
2028-11-27 FEDERAL FARM CR BKS BOND 2.65000% 11/27/2028                                  
2028-01-21 FEDERAL FARM CR BKS BOND 2.03000% 01/21/2028                                  
2024-07-02 FEDL NATL MTG ASSN SER BENCHMARK 1.75000% 07/02/2024
2024-05-06 FIRST NATL BANCORP INC PA CD 5.05000% 05/06/2024                              
2024-05-14 GOLDMAN SACHS BK USA CD 5.00000% 05/14/2024                                   
2024-11-15 JPMORGAN CHASE BK N A CD 5.15000% 11/15/2024                                  
2025-05-19 MORGAN STANLEY BK N A CD 4.90000% 05/19/2025                                  
2026-05-26 SOUTHERN FIRST BK N A GREENVIL CD 4.65000% 05/26/2026                         
2025-11-01 TENNESSEE VALLEY AUTH FED BE ZERO CPN 0.00000% 11/01/2025                     
2024-03-31 UNITED STATES TREAS SER AZ-2024 2.25000% 03/31/2024 NTS NOTE
2024-04-30 UNITED STATES TREAS SER BA-2024 2.50000% 04/30/2024 NTS NOTE                  
2024-12-31 UNITED STATES TREAS SER BM-2024 4.25000% 12/31/2024 NTS NOTE                  
2026-04-15 UNITED STATES TREAS SER AM-2026 3.75000% 04/15/2026 NTS NOTE                  
CheckExpire: Updated 0, Errors 0
----------------------------------------------------------*/

	if ( xstock.xstype[0] != STYPE_BOND )
	{
		return ( 0 );
	}

	sprintf ( SlashDate, "%2.2s/%2.2s/%4.4s", 
			&xportfolio.xpoptexp[5],
			&xportfolio.xpoptexp[8],
			&xportfolio.xpoptexp[0] );

	UpdateCount++;

	if ( strstr ( xstock.xsname, SlashDate ) == NULL )
	{
		printf ( "%ld  %s  %s  %s\n", xportfolio.xpmember, xportfolio.xpoptexp, xstock.xsticker, xstock.xsname );
		ErrorCount++;
	}

	return ( 0 );
}
