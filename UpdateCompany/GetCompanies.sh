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


case "$1" in
	'-generate' )
		rm -f tickers.txt
		echo "select Sticker from stock where Stype != 'E' and (Ssector is NULL or Sindustry is null)" | mysql -D invest | sed 's/ //g' | awk -F '|' '{print $2}' > tickers.txt
		echo "select Sticker from stock where Stype != 'E' and (Ssector = 64 or Sindustry = 257)" | mysql -D invest | sed 's/ //g' | awk -F '|' '{print $2}' >> tickers.txt
		echo "select Fticker from fundamental,stock where Sticker = Fticker and Stype != 'E' and Fcountry is null" | mysql -D invest | sed 's/ //g' | awk -F '|' '{print $2}' >> tickers.txt

		echo "select Sticker from stock where Stype = '?'" | mysql -D invest | sed 's/ //g' | awk -F '|' '{print $2}' >> tickers.txt

		sort -u -o tickers.txt tickers.txt
		vi tickers.txt
		echo "press enter to get company data from IEX"
		read x
		;;

	'-portfolio' )
		echo "select Pticker from portfolio" | mysql -D invest | sed 's/ //g' | awk -F '|' '{print $2}' > tickers.txt
		vi tickers.txt
		;;

	'-file' )
		if [ ! -f tickers.txt ]
		then
			echo "tickers.txt does not exist"
			exit 1
		fi
		;;

	* ) 
		echo "USAGE: GetCompanies.sh {-generate|-portfolio|-file}"
		exit 1
		;;
esac

# for i in `cat tickers.txt`
for i in `cat tickers.txt | awk -F ',' '{print $1}'`
do
	/var/local/invest/TestCloud.sh R $i company 
	sleep 1
done | sed 's/[{}]//g' | sed 's/"//g' | sed "s/'//g" | textp -f > Companies.dat


#
#USAGE: TestCloud.sh {T|R} TICKER {company quote previous splits div1mo div1yr div5yr divnext 5yr 2yr 1mo {date yyyymmdd} ohlc book financials stats}
#USAGE: TestCloud.sh X daily
#USAGE: TestCloud.sh X sectors
#USAGE: TestCloud.sh X symbols
#
