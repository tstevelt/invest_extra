#!/usr/local/bin/python3
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

import pandas as pd

df = pd.read_excel('US_ETF.xls', sheet_name=None)
df['Basic Facts'].to_csv('US_Basic.csv',sep='|',float_format='%.4f')  
df['Income Characteristics'].to_csv('US_Income.csv',sep='|')  
df['Performance & Risk'].to_csv('US_Performance.csv',sep='|')  
df['Analyst Opinions'].to_csv('US_Analyst.csv',sep='|')  

df = pd.read_excel('INTL_ETF.xls', sheet_name=None)
df['Basic Facts'].to_csv('INTL_Basic.csv',sep='|',float_format='%.4f')  
df['Income Characteristics'].to_csv('INTL_Income.csv',sep='|')  
df['Performance & Risk'].to_csv('INTL_Performance.csv',sep='|')  
df['Analyst Opinions'].to_csv('INTL_Analyst.csv',sep='|')  

df = pd.read_excel('BOND_ETF.xls', sheet_name=None)
df['Basic Facts'].to_csv('BOND_Basic.csv',sep='|',float_format='%.4f')  
df['Income Characteristics'].to_csv('BOND_Income.csv',sep='|')  
df['Performance & Risk'].to_csv('BOND_Performance.csv',sep='|')  
df['Analyst Opinions'].to_csv('BOND_Analyst.csv',sep='|')  

