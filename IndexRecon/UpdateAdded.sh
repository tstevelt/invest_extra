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

if [ ! -f Added.txt ]
then
	echo "Added.txt not found"
	exit 1
fi

#for i in `cat Added.txt`
#do
#	getdata -one $i -fmt csv -2yr
#	# UpdateAverage and getfund take long enough that IEX_RateLimit is not needed.
#	UpdateAverage -ticker $i A
#	getfund -ticker $i
#done

#
# IndexRecon should update sector and industry, then this wouldn't be needed.
#
#for i in `cat Added.txt`
#do
#	/var/local/invest/TestCloud.sh R $i company 
#done | sed 's/[{}]//g' | sed 's/"//g' | sed "s/'//g" | textp -f > Companies.dat

UpdateCompany Companies.dat


mv Added.txt Added.txt-




