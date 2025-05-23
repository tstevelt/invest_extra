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

VERBOSE=N

if [ "$1" = '' ]
then
	DEST='cat'
else
	DEST=$1
fi

FILE=/var/local/tmp/GetVIX.txt
rm -f $FILE

#WORKDIR=/home/tms/src/invest_extra/getvix
WORKDIR=/Users/tms/src/invest_extra/getvix

SendFile ()
{
	if [ "$DEST" = 'cat' ]
	then
		cat $FILE
	else
		cat $FILE | mail -s 'VIX RISK' $DEST
	fi
	exit 0
}

echo "=== Getting VIX data `date` ===" >> $FILE

cd $WORKDIR

if [ ! -f VIX.csv.prev ]
then
	echo "Missing VIX.csv.prev" >> $FILE
	SendFile
fi

curl -s "https://cdn.cboe.com/api/global/us_indices/daily_prices/VIX_History.csv" > VIX.csv

# vi VIX.csv

diff -e VIX.csv.prev VIX.csv > /dev/null
if [ $? = 0 ]
then
	echo "VIX.csv.prev VIX.csv are the same" >> $FILE
	SendFile
fi

diff -e VIX.csv.prev VIX.csv | grep , | sed 's/^/VIX,/'  > x1.csv

rm -f script1 script2

if [ $VERBOSE = Y ]
then
	echo "select count(*) from history where Hticker = 'VIX';" >> script1
fi

echo "load data local infile 'x1.csv'" >> script1
echo "into table history" >> script1
echo "fields terminated by ',' lines terminated by '\n' " >> script1
echo "(Hticker, @Var_Hdate, Hopen, Hhigh, Hlow, Hclose )" >> script1
echo "set Hdate = str_to_date(@Var_Hdate,'%m/%d/%Y') ;" >> script1

if [ $VERBOSE = Y ]
then
	echo "show warnings ;" >> script1
fi

echo "update stock set Slast = (select max(Hdate) from history where Hticker = 'VIX') where Sticker = 'VIX';" >> script1

if [ $VERBOSE = Y ]
then
	echo "select count(*) from history where Hticker = 'VIX';" >> script1
fi

mysql -D invest < script1 >> $FILE

if [ $VERBOSE = Y ]
then
	UpdateAverage -ticker VIX Z >> $FILE
else
	UpdateAverage -ticker VIX Z > /dev/null
fi

echo "select Hdate, Hclose from history where Hticker = 'VIX' " > script2
echo " and Hdate = (select max(Hdate) from history where Hticker = 'VIX') into @DATE, @CLOSE;" >> script2
echo "select Ama10 from average where Aticker = 'VIX' and Adate = @DATE into @AMA10 ;" >> script2
if [ $VERBOSE = Y ]
then
	echo "select @DATE, @CLOSE, @AMA10;" >> script2
fi
echo "select 'Ama10', if ( @CLOSE < @AMA10, 'RISK ON', 'RISK OFF' );" >> script2

mysql -D invest < script2 >> $FILE

mv VIX.csv VIX.csv.prev

SendFile


