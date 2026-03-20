# GNUPlot script for Question 8
set terminal pngcairo size 1000,600 enhanced font 'Arial,12'
set output 'report/q8_avg_max_min.png'
set title 'Land Temperature: Average, Max, and Min (1850-2015)' font 'Arial,14'
set xlabel 'Year'
set ylabel 'Temperature (°C)'
set grid
set key top left
plot 'data/q8_data.dat' using 1:2 with lines lw 3 lc rgb 'red'   title 'Land Avg Temp', \
     'data/q8_data.dat' using 1:3 with lines lw 1 lc rgb 'orange' dt 2 title 'Land Max Temp', \
     'data/q8_data.dat' using 1:4 with lines lw 1 lc rgb 'blue'  dt 2 title 'Land Min Temp'
