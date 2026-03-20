# GNUPlot script for Question 9
set terminal pngcairo size 1200,900 enhanced font 'Arial,12'
set output 'report/q9_bar_plots.png'
set multiplot layout 1,3 title 'Average, Max, and Min Temperatures by Century' font 'Arial,14'
set style data histogram
set style fill solid 1.0 border -1
set boxwidth 0.6
set xtic rotate by -30 scale 0 font 'Arial,10'
set ylabel 'Temperature (°C)'
set yrange [0:*]
set title 'Average Land Temperature'
plot 'data/q9_data.dat' using 2:xtic(1) lc rgb 'steelblue'  title 'Average'
set title 'Max Land Temperature'
plot 'data/q9_data.dat' using 3:xtic(1) lc rgb 'red'        title 'Max'
set title 'Min Land Temperature'
plot 'data/q9_data.dat' using 4:xtic(1) lc rgb 'navy'   title 'Min'
unset multiplot
