copy %1\*.jpg .
for %%X in (*.jpg) DO montage %1\%%X %2\%%X %3\%%X %4\%%X -geometry 400x300+0+0 combovid\%%X
call makevideo combovid %1%2%3%4 %5
del *.jpg