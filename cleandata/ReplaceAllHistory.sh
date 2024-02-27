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
	echo " ReplaceAllHistory.sh TICKER"
	exit 1
fi

TICKER=$1

echo "delete from history where Hticker = '$TICKER';"            > script
echo "delete from average where Aticker = '$TICKER';"           >> script
echo "update stock set Slast = NULL where Sticker = '$TICKER';" >> script
mysql -D invest < script

getdata -one $TICKER -2yr -slast

UpdateAverage -ticker $TICKER A

getfundSEC -ticker $TICKER -v -ignore

UpdateFundamental -ticker $TICKER -v

