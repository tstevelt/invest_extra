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

##for i in INTL_Analyst.csv INTL_Income.csv INTL_Performance.csv
##do
##	echo "==== $i ===="
##	head -1 $i | sed 's/".*",/xxx,/g' | sed 's/|/\n/g' | textp -n 
##done
##exit

##  ==== INTL_Basic.csv ====
##       1 
##       2 Symbol
##       3 Fidelity Sponsored
##       4 ETP Name
##       5 ETP Type
##       6 Leveraged / Inverse
##       7 Market Price
##       8 % Price Change Today
##       9 Sponsor
##      10 Asset Class
##      11 Investment Philosophy
##      12 Benchmark Index
##      13 Equity: Style Box
##      14 Morningstar Category
##      15 Volume (Today)
##      16 Net Assets
##      17 Net Expense Ratio
##      18 Inception Date
##      19 Optionable

cat INTL_Basic.csv US_Basic.csv BOND_Basic.csv | awk -F '|' '{printf("%s,%s,%s,%s\n",$2,$16,$17,$18);}' > Basic.csv
head Basic.csv

##  ==== INTL_Analyst.csv ====
##       1 
##       2 Symbol
##       3 Fidelity Sponsored
##       4 ETP Name
##       5 ETP Type
##       6 Leveraged / Inverse
##       7 FactSet
##       8 Morningstar (TM) Rating

cat INTL_Analyst.csv US_Analyst.csv BOND_Analyst.csv | awk -F '|' '{printf("%s,%s,%s\n",$2,$7,$8);}' > Analyst.csv
head Analyst.csv

##  ==== INTL_Income.csv ====
##       1 
##       2 Symbol
##       3 Fidelity Sponsored
##       4 ETP Name
##       5 ETP Type
##       6 Leveraged / Inverse
##       7 Asset Class
##       8 30-day SEC Yield
##       9 Distribution Yield (TTM)
##      10 Dividend Paid in Last 12 Months
##      11 Fixed Income: Weighted Average Coupon
##      12 Schedule K-1
##      13 Tax Cost Ratio
##      14 Annual Turnover Ratio
##      15 Potential Cap Gains Exposure

cat INTL_Income.csv US_Income.csv BOND_Income.csv | awk -F '|' '{printf("%s,%s,%s\n",$2,$9,$10);}' > Income.csv
head Income.csv

##  ==== INTL_Performance.csv ====
##       1 
##       2 Symbol
##       3 Fidelity Sponsored
##       4 ETP Name
##       5 ETP Type
##       6 Leveraged / Inverse
##       7 % Price Change Today
##       8 Market Total Returns (YTD)
##       9 Market Total Returns (1 Yr)
##      10 Market Total Returns (3 Yr)
##      11 Market Total Returns (5 Yr)
##      12 Standard Deviation (Month-End 3 Yr)
##      13 Sharpe Ratio (Month-End 3 Yr)
##      14 Beta (Month-End 3 Yr)
##      15 R2 (Month-End 3 Yr)
##      16 Tracking Error

cat INTL_Performance.csv US_Performance.csv BOND_Performance.csv | awk -F '|' '{printf("%s,%s,%s\n",$2,$10,$11);}' > Performance.csv
head Performance.csv


