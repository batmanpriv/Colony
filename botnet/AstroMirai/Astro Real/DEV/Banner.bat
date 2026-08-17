@echo off
:next
mode con lines=25 cols=85
title Zombies: 568 l User: %USERNAME%
cls
echo TYPE NEXT TO SEE NECT BANNER
echo.
type banner.py
set /p action= action:~$

:help
cls
echo TYPE NEXT TO SEE NECT BANNER
echo.
type help.py
set /p action= action:~$

:methods
cls
echo TYPE NEXT TO SEE NECT BANNER
echo.
type methods.py
set /p action= action:~$

:api
cls
echo TYPE NEXT TO SEE NECT BANNER
echo.
type api.py
set /p action= action:~$

if %action% == next goto next
if %action% == next goto methods
if %action% == next goto help
if %action% == next goto api
if %action% == a goto next
if %action% == b goto next
if %action% == c goto next
if %action% == d goto next
if %action% == e goto next
if %action% == f goto next
if %action% == g goto next
if %action% == h goto next
if %action% == i goto next
if %action% == j goto next
if %action% == k goto next
if %action% == l goto next
if %action% == m goto next
if %action% == n goto next
if %action% == o goto next
if %action% == p goto next
if %action% == q goto next
if %action% == r goto next
if %action% == s goto next
if %action% == t goto next
if %action% == u goto next
if %action% == v goto next
if %action% == w goto next
if %action% == x goto next
if %action% == y goto next
if %action% == z goto next