# GNUPlot script for Question 7
set terminal pngcairo size 1000,600 enhanced font 'Arial,12'
set output 'report/q7_century_comparison.png'
set title '19th vs 20th Century Land Average Temperatures' font 'Arial,14'
set xlabel 'Year within Century'
set ylabel 'Average Temperature (°C)'
set grid
set key top left
plot 'data/q7_data.dat' using 1:2 with lines lw 2 lc rgb 'blue'  title '19th Century (1800-1899)', \
     'data/q7_data.dat' using 1:3 with lines lw 2 lc rgb 'red'   title '20th Century (1900-1999)'
