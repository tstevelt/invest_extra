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

if [ "$2" = '' ]
then
	echo "USAGE: getYAHOO.sh ticker date"
	echo "  enter date as mm-dd-yyyy"
	exit 1
fi

TICKER=$1
DATE=$2

FILE=$TICKER.csv

#USAGE: YAHOO_FIN.py ticker [startdate]
#   startdate format mm-dd-yyyy. default 01-01-2014

YAHOO_FIN.py $TICKER $DATE

head $FILE

#open,high,low,close,adjclose,volume,ticker
#2014-01-02,71.04,71.08,70.38,70.60,59.63,1288800,RSP
#2014-01-03,70.74,70.85,70.48,70.54,59.58,538200,RSP
#2014-01-06,70.68,70.89,70.26,70.32,59.39,593200,RSP

echo "delete from history where Hticker = '$TICKER';"  > x1.sql
echo "delete from average where Aticker = '$TICKER';" >> x1.sql
mysql < x1.sql

getYAHOO $FILE > x1.sql
mysql < x1.sql

rm -f $FILE x1.sql

UpdateAverage -ticker $TICKER A


