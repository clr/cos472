#######################################################################
# Gnuplot file for generating confidence intervals
# Clare Bates Congdon Mon Oct  5 21:39:05 2009
# This file requires input files from this directory
#   and the input file is hardcoded into the following
# To run this file, start gnuplot (at the unix commandline)
#   and then load "confidence.g"
#   Both this file and data-confidence must be in the directory you
#     are working in
# Look at the resulting file using a browser

#######################################################################
# set the range on the y axis -- this will change for your specific data
set yrange[59:101];

#######################################################################
# set the range on the x axis and then convert the numbers to labels
# -- the labels will change for your specific data
set xrange [0:40];
set xtics ("Info Gain" 10, "Alphabetic" 20, "Fattest Value" 30);

#######################################################################
# set terminal and set output work together to control the display
#   use the first two to generate a postscript file
#   use the second two to generate a jpg file
#   comment out all four lines below to see the plot on the screen
# blah. jpg no longer supported in gnuplot --> png
#set terminal postscript portrait
#set output "intervals.ps"
set terminal png 
set output "cancer-class-intervals.png"
#set terminal jpeg
#set output "intervals.jpg"

#######################################################################
# Label the plot and the axes
set title "Adnan Fazeli and Casey Rosenthal: 95% confidence intervals for Cancer Class Accuracy"
set xlabel "learning system" 0,-1;
set ylabel "percent accuracy on testing runs" -1,0;

#######################################################################
# actually make the plot
plot 'cancer-class-data-confidence' using 14:3:9:12 notitle with errorbars, \
	'cancer-class-data-confidence' using 14:3 notitle with points 2
