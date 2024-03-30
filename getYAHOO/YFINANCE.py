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

# from: https://medium.com/@kasperjuunge/yfinance-10-ways-to-analyze-stock-prices-18d2062ab35c

# from: https://algotrading101.com/learn/yfinance-guide/
#  and: https://algotrading101.com/learn/yahoo-finance-api-guide/
# 1m, 2m, 5m, 15m, 30m, 60m, 90m, 1h, 1d, 5d, 1wk, 1mo, 3mo


import yfinance as yf

TICKER='AAPL'
TICKER='RSP'
df = yf.Ticker(TICKER)

# interval='1d' is the default
#hist = df.history(period="1y")

hist = df.history(period="10y")
print ( hist )

hist = hist.drop(['Dividends','Stock Splits'], axis=1)
print ( hist )

hist.to_csv ( TICKER + '.csv', float_format='%.2f' )

quit()

hist['SMA'] = hist['Close'].rolling(window=20).mean()
print ( hist['SMA'] )

hist['EMA'] = hist['Close'].ewm(span=20, adjust=False).mean()
print ( hist['EMA'] )

hist['UpperBB'] = hist['SMA'] + (hist['Close'].rolling(20).std() * 2)
hist['LowerBB'] = hist['SMA'] - (hist['Close'].rolling(20).std() * 2)
print ( hist['UpperBB'] )
print ( hist['LowerBB'] )

hist['Daily_Return'] = hist['Close'].pct_change()
print ( hist['Daily_Return'] )

hist['Cumulative_Return'] = (1 + hist['Daily_Return']).cumprod()
print ( hist['Cumulative_Return'] )

spy = yf.Ticker("SPY").history(period="1y")
print ( spy )
# error hist['Covariance'] = hist['Daily_Return'].rolling(window=60).cov(spy['Daily_Return'])
#hist['Beta'] = hist['Covariance'] / spy['Daily_Return'].rolling(window=60).var()
#print ( hist['Beta'] )

# error div_yield = apple.info['dividendYield']
# print ( div_yield )

quit()

import matplotlib.pyplot as plt

hist[['Close', 'SMA', 'EMA']].plot(figsize=(10,5))
plt.show()

hist['Volume'].plot(figsize=(10,5))

