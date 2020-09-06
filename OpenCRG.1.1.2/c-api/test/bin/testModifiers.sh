#!/bin/sh

DATA_DIR='../../../crg-txt'

runSingleTest()
{
    if [ $4 != "auto" ] ; then
        XRANGE="set xrange $4"
    else
        XRANGE=""
    fi

    if [ $5 != "auto" ] ; then
        YRANGE="set yrange $5"
    else
        YRANGE=""
    fi

    if [ $6 != "auto" ] ; then
        ZRANGE="set zrange $6"
    else
        ZRANGE=""
    fi
    ../../demo/bin/crgEvalOpts -t 0 -p $DATA_DIR/$2
    echo -e "set title \"unmodified data\"\n""set view $3\n""$XRANGE\n""$YRANGE\n""$ZRANGE\n""splot \"crgPlotData.txt\" with points\n" > gnuplotCmds.txt 
    gnuplot -persist gnuplotCmds.txt   
    
    ../../demo/bin/crgEvalOpts -t $1 -p $DATA_DIR/$2
    echo -e "set title \"$7\"\n""set view $3\n""$XRANGE\n""$YRANGE\n""$ZRANGE\n""splot \"crgPlotData.txt\" with points\n" > gnuplotCmds.txt 
    gnuplot -persist gnuplotCmds.txt   
    
    echo "Test successful? [y]es/[n]o (y)"
    
    read RESULT

    if [[ $RESULT != "y" && $RESULT != "" ]] ; then
        echo "FAIL: no. $1, desc = <$7>" >> testReport.txt
    else
        echo "OK:   no. $1, desc = <$7>" >> testReport.txt
    fi
    
    killall gnuplot_x11
    
}

# reset old tests and test results
NOW=`date`

echo "report of test run on $NOW:" > testReport.txt


# now run the tests

 runSingleTest 18 "handmade_straight.crg"    "70,10,1,1" "auto"       "auto"      "[-0.1:0.1]" "z scale 2.0" 
 runSingleTest 19 "handmade_sloped.crg"      "70,10,1,1" "auto"       "auto"      "[0.0:14.0]" "slope scale 10.0" 
 runSingleTest 20 "handmade_banked.crg"      "50,40,1,1" "auto"       "auto"      "[-3.0:3.0]" "bank scale 10.0" 
 runSingleTest 21 "handmade_sloped.crg"      "70,10,1,1" "[-120:120]" "auto"      "auto"       "length scale 2.0" 
 runSingleTest 22 "handmade_banked.crg"      "70,30,1,1" "auto"       "[-16:16]"  "auto"       "width scale 2.0" 
 runSingleTest 23 "handmade_arc.crg"         "0,0,1,1"   "[-200:100]" "[-50:300]" "auto"       "curvature scale 0.50" 
 runSingleTest 24 "handmade_arc.crg"         "0,0,1,1"   "[-200:300]" "[-10:45]"  "auto"       "curvature scale 0.00" 
 runSingleTest 25 "handmade_straight.crg"    "70,10,1,1" "auto"       "auto"      "auto"       "refpoint u/v 5.0/1.5 to x/y/z 100.0/200.0/10.0" 
 runSingleTest 26 "handmade_straight.crg"    "70,10,1,1" "auto"       "auto"      "auto"       "refline offset x/y/z 100.0/200.0/10.0" 
 runSingleTest 27 "handmade_straight.crg"    "45,45,1,1" "auto"       "auto"      "auto"       "refline offset x/y/z/phi 100.0/100.0/100.0/1.57" 
 runSingleTest 28 "handmade_straight.crg"    "45,45,1,1" "auto"       "auto"      "auto"       "refline offset phi 45 deg" 

echo "test results can be retrieved from <testReport.txt>"
