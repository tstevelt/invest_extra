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


for m in 01 02 03 04 05 06 07 08 09 10 11 12
do
	echo "==== $m-2016 ===="
	for d in 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31
	do
		UpdateMarket -market 2016-$m-$d
	done
done

for m in 01 02 03 04 05 06 07 08 09 10 11 12
do
	echo "==== $m-2017 ===="
	for d in 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31
	do
		UpdateMarket -market 2017-$m-$d
	done
done

echo "==== 12-2018 ===="
for d in 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31
do
	UpdateMarket -market 2018-12-$d
done

#for m in 01 02 03 04 05 06 07 08 09 10 11 12
#do
#	for d in 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31
#	do
#		UpdateMarket -market 2018-$m-$d
#	done
#done

#for m in 01 02
#do
#	for d in 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31
#	do
#		UpdateMarket -market 2019-$m-$d
#	done
#done

#for d in 01 02 03 04 05 06 07 08 09 10 11 12 13
#do
#	UpdateMarket -market 2019-03-$d
#done
