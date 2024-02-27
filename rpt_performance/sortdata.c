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

#include	"rpt_performance.h"

void sortdata ( char *OutFileName )
{
	char	cmdline[1024];
	int		FldNo = 1;

	switch ( SortBy )
	{
		case SORTBY_TICKER:
			/*----------------------------------------------------------
				Ticker is column one for all reports.
			----------------------------------------------------------*/
			break;
		case SORTBY_GAIN:
					FldNo = 7;
			break;
		case SORTBY_RETURN:
					FldNo = 8;
			break;
		case SORTBY_COMPARED:
					FldNo = 11;
			break;
	}

	snprintf ( cmdline, sizeof(cmdline), "sort -n -t '|' -k %d,%d -o %s %s", FldNo, FldNo, OutFileName, OutFileName );

	system ( cmdline );
}
