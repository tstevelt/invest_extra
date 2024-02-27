#!/bin/sh
#     Invest extras
# 
#     Copyright (C)  2019 - 2024 Tom Stevelt
# 
#     This program is free software: you can redistribute it and/or modify
#     it under the terms of the GNU Affero General Public License as
#     published by the Free Software Foundation, either version 3 of the
#     License, or (at your option) any later version.
# 
#     This program is distributed in the hope that it will be useful,
#     but WITHOUT ANY WARRANTY; without even the implied warranty of
#     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#     GNU Affero General Public License for more details.
# 
#     You should have received a copy of the GNU Affero General Public License
#     along with this program.  If not, see <https://www.gnu.org/licenses/>.

if [ "$1" = '' ]
then
	echo "DoIgnore.sh {all|file}"
	exit 1
fi

case "$1" in
	'all' ) OPTIONS=A
			;;
	* )		if [ ! -f $1 ]
			then
				echo "File $1 not found"
				exit 1
			fi
			OPTIONS="-file $1"
			;;
esac

##  USAGE: getfundSEC index          [options]
##  USAGE: getfundSEC -file filename [options]
##  USAGE: getfundSEC -ticker ticker [options]
##  USAGE: getfundSEC -old [-delete]
##   S = S&P 500
##   D = Dow Jones Industrials
##   N = Nasdaq 100
##   M = Midcap (Russell 1000 ex S&P 500)
##   1 = Russell 1000
##   2 = Russell 2000
##   3 = Russell 3000
##   A = All stocks
##   E = Only ETF
##  Options:
##   -ignore = passes ancient Fframe CY197001
##   -v      = verbose
##   -nodb   = no db update, statements to stdout
##   -d [#]  = debug, no db update

getfundSEC $OPTIONS -ignore


##  USAGE: getfundIEX index          [options]
##  USAGE: getfundIEX -file filename [options]
##  USAGE: getfundIEX -ticker ticker [options]
##   S = S&P 500
##   D = Dow Jones Industrials
##   N = Nasdaq 100
##   M = Midcap (Russell 1000 ex S&P 500)
##   1 = Russell 1000
##   2 = Russell 2000
##   3 = Russell 3000
##   A = All stocks
##   E = Only ETF
##  Options:
##   -v      = verbose
##   -nodb   = no db update, statements to stdout
##   -d [#]  = debug, no db update

getfundIEX $OPTIONS 


##  USAGE: UpdateFundamental -all           [options]
##  USAGE: UpdateFundamental -file filename [options]
##  USAGE: UpdateFundamental -ticker ticker [options]
##  Options:
##   -nodb   = no db update, statements to stdout
##   -v      = verbose
##   -d [#]  = debug, no db update

UpdateFundamental $OPTIONS


