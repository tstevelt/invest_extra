#!/bin/sh

if [ "$1" = '' ]
then
	echo "USAGE: Retry.sh yyyy-mm-dd"
	exit 1
fi

DATE=$1

/usr/bin/vim VIX.csv.prev

echo "delete from history where Hdate = '$DATE' and Hticker = 'VIX';"  > script
echo "delete from average where Adate = '$DATE' and Aticker = 'VIX';" >> script

mysql -D invest < script

CheckTicker.sh VIX




