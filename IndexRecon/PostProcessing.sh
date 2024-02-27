#!/bin/sh -x
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


#echo "select Sticker from stock where Slast is NULL ;" | mysql -D invest

#getdata -null -fmt csv -2yr

# if you don't have a list of ticker from IndexRecon or the sql above, then
# make a list with this sql.

#echo "select Sticker from stock where (select count(*) from average where Aticker = Sticker) = 0 ;" | mysql -D invest > ticker.txt
#vi ticker.txt

for ticker in `cat ticker.txt`
do
	UpdateAverage -ticker $ticker A
done

echo "Updating stock Slast and 52 week high ..."
mysql -D invest < /home/tms/src/invest/sql/FixSlast.sql

echo "Updating UpdateMarket ..."
DATE=`date +%Y-%m-%d`
/usr/local/bin/UpdateMarket -market $DATE

cd ../UpdateCompany
GetCompanies.sh -generate
UpdateCompany Companies.dat

