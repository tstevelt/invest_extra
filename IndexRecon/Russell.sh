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

# download pdf file from russell website.
# use online pdf to txt to convert to text.
# 2022 i used https://www.zamzar.com
# vi ru*.txt. eliminate junk. convert set fileformat=unix
# should look like this (except for # space)

# ==> ru1000_membershiplist_20220624.txt <==
# Company                         Ticker   Company                           Ticker
# 123456789-123456789-123456789-123456789-123456789-
# 10X GENOMICS                    TXG      AMCOR                             AMCR
# 3M COMPANY                      MMM      AMDOCS                            DOX
# ABBOTT LABORATORIES             ABT      AMEDISYS INC                      AMED
# ABBVIE INC                      ABBV     AMERCO                            UHAL
# ABIOMED INC                     ABMD     AMEREN CORP                       AEE
# ACADIA HEALTHCARE               ACHC     AMERICAN AIRLINES GROUP           AAL
# ACCENTURE CL A                  ACN      AMERICAN CAMPUS COMMUNITIES       ACC
# ACTIVISION BLIZZARD             ATVI     AMERICAN ELEC POWER COM           AEP
# ACUITY BRANDS INC               AYI      AMERICAN EXPRESS COM              AXP
# 
# ==> ru2000_membershiplist_20220624.txt <==
# Company                          Ticker   Company                             Ticker
# 123456789-123456789-123456789-123456789-123456789-
# 1-800-FLOWERS.COM CL A           FLWS     AEMETIS                             AMTX
# 1LIFE HEALTHCARE                 ONEM     AERIE PHARMACEUTICALS INC           AERI
# 1ST SOURCE                       SRCE     AEROJET ROCKETDYNE HOLDINGS         AJRD
# 1STDIBS.COM                      DIBS     AEROVATE THERAPEUTICS               AVTE
# 22ND CENTURY GROUP INC.          XXII     AEROVIRONMENT                       AVAV
# 23ANDME HOLDING CO               ME       AERSALE CORPORATION                 ASLE
# 2SEVENTY BIO                     TSVT     AEVA TECHNOLOGIES INC               AEVA
# 2U                               TWOU     AEYE INC                            LIDR
# 3-D SYSTEMS                      DDD      AFC GAMMA                           AFCG

# cut files and reassemble. NOTE DIFFERENT COLUMN WIDTH!!!
# THIS IF FUCKED UP.  EVERY PAGE HAS DIFFERENT COLUMN WIDTH!!!!

#echo "select Srussell, count(*) from stock group by Srussell ;" | mysql -D invest

#echo "update stock set Srussell = 'N' where Srussell != 'N';" | mysql -D invest

IndexRecon 1 2023-07-20 ru1000.csv -add

IndexRecon 2 2023-07-20 ru2000.csv -add

echo "select Srussell, count(*) from stock group by Srussell ;" | mysql -D invest


