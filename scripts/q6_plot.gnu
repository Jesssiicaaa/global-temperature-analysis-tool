# GNUPlot script for Question 6
set terminal pngcairo size 1000,600 enhanced font 'Arial,12'
set output 'report/q6_yearly_avg.png'
set title 'Yearly Average Land Temperature (1760-2015)' font 'Arial,14'
set xlabel 'Year'
set ylabel 'Average Temperature (°C)'
set grid
set key top left
plot 'data/q6_data.dat' using 1:2 with lines lw 2 lc rgb 'red' title 'Land Avg Temp'
