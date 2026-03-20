# GNUPlot script for Question 10
set terminal pngcairo size 1000,600 enhanced font 'Arial,12'
set output 'report/q10_errorbars.png'
set title 'Average Monthly Land Temperature with Error Bars (2000-2015)' font 'Arial,14'
set xlabel 'Month'
set ylabel 'Temperature (°C)'
set xtics ('Jan' 1, 'Feb' 2, 'Mar' 3, 'Apr' 4, 'May' 5, 'Jun' 6, \
           'Jul' 7, 'Aug' 8, 'Sep' 9, 'Oct' 10, 'Nov' 11, 'Dec' 12)
set grid
set key top right
plot 'data/q10_data.dat' using 1:2:3 with yerrorbars lw 2 lc rgb 'dark-green' pt 7 title 'Avg Temp ± Uncertainty'
