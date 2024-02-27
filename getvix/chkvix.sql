--     Invest extras
-- 
--     Copyright (C)  2019 - 2024 Tom Stevelt
-- 
--     This program is free software: you can redistribute it and/or modify
--     it under the terms of the GNU Affero General Public License as
--     published by the Free Software Foundation, either version 3 of the
--     License, or (at your option) any later version.
-- 
--     This program is distributed in the hope that it will be useful,
--     but WITHOUT ANY WARRANTY; without even the implied warranty of
--     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
--     GNU Affero General Public License for more details.
-- 
--     You should have received a copy of the GNU Affero General Public License
--     along with this program.  If not, see <https://www.gnu.org/licenses/>.


select Hdate, Hclose from history 
 where Hticker = 'VIX' and Hdate = (select max(Hdate) from history where Hticker = 'VIX')
  into @DATE, @CLOSE;

select Ama20 from average where Aticker = 'VIX' and Adate = @DATE into @AVERAGE ;

select @DATE, @CLOSE, @AVERAGE;

select if ( @CLOSE < @AVERAGE, 'RISK ON', 'RISK OFF' );

