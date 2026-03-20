# GNUPlot script for Question 11
set terminal pngcairo size 1000,600 enhanced font 'Arial,12'
set output 'report/q11_land_vs_ocean.png'
set title 'Land vs Land+Ocean Average Temperature (1850-2015)' font 'Arial,14'
set xlabel 'Year'
set ylabel 'Average Temperature (°C)'
set grid
set key top left
plot 'data/q11_data.dat' using 1:2 with lines lw 2 lc rgb 'red'  title 'Land Average Temp', \
     'data/q11_data.dat' using 1:3 with lines lw 2 lc rgb 'blue' title 'Land+Ocean Average Temp'
