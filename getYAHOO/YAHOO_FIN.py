#!/bin/python3
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

import sys
import os

if len(sys.argv) == 1:
	print ( "USAGE: YAHOO_FIN.py ticker [startdate]" )
	print ( "   startdate format mm-dd-yyyy. default 01-01-2014" )
	quit ()

symbol = sys.argv[1:]
symbol = symbol[0]

if len(sys.argv) > 2:
	startdate = sys.argv[2:]
	startdate = startdate[0]
else:
	startdate = "01-01-2014"

from yahoo_fin.stock_info import *

try:
	info = get_data(symbol, startdate)
	if info.empty:
		print ('no prices')
	else:
		# print (info)
		info.to_csv( symbol + ".csv", float_format="%.2f")
except:
	print ('no prices')


