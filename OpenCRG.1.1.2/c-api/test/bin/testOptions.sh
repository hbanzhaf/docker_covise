#!/bin/sh

DATA_DIR='../../../crg-txt'

runSingleTest()
{
    if [ $5 != "auto" ] ; then
        XRANGE="set xrange $5"
    else
        XRANGE=""
    fi

    if [ $6 != "auto" ] ; then
        YRANGE="set yrange $6"
    else
        YRANGE=""
    fi

    if [ $7 != "auto" ] ; then
        ZRANGE="set zrange $7"
    else
        ZRANGE=""
    fi

    ../../demo/bin/crgEvalOpts -t 0 -p $DATA_DIR/$2
    echo -e "set title \"default settings\"\n""set view $3\n""$XRANGE\n""$YRANGE\n""$ZRANGE\n""splot \"crgPlotData.txt\" with $4\n" > gnuplotCmds.txt 
    gnuplot -persist gnuplotCmds.txt   
    
    ../../demo/bin/crgEvalOpts -t $1 -p $DATA_DIR/$2
    echo -e "set title \"$8\"\n""set view $3\n""$XRANGE\n""$YRANGE\n""$ZRANGE\n""splot \"crgPlotData.txt\" with $4\n" > gnuplotCmds.txt 
    gnuplot -persist gnuplotCmds.txt   
    
    echo "Test successful? [y]es/[n]o (y)"
    
    read RESULT

    if [[ $RESULT != "y" && $RESULT != "" ]] ; then
        echo "FAIL: no. $1, desc = <$8>" >> testReport.txt
    else
        echo "OK:   no. $1, desc = <$8>" >> testReport.txt
    fi
    
    killall gnuplot_x11
    
}

# reset old tests and test results
NOW=`date`

echo "report of test run on $NOW:" > testReport.txt

# now run the tests
 runSingleTest  0 "handmade_sloped.crg"    "100,8,1,1"  "points" "[-60:60]" "auto"   "auto"  "default settings" 
 runSingleTest  1 "handmade_sloped.crg"    "100,8,1,1"  "points" "[-60:60]" "auto"   "auto"  "curvature on reference line" 
 runSingleTest  2 "handmade_sloped.crg"    "100,8,1,1"  "points" "[-60:60]" "auto"   "auto"  "refuse query exceeding u dimensions of data"
 runSingleTest  3 "handmade_sloped.crg"    "100,8,1,1"  "points" "[-60:60]" "auto"   "auto"  "set elevation to zero when exceeding u and v dimensions of data, apply -1.0m offset for u"
 runSingleTest  4 "handmade_sloped.crg"    "100,8,1,1"  "points" "[-60:60]" "auto"   "auto"  "keep last value when exceeding u dimensions of data"
 runSingleTest  5 "handmade_sloped.crg"    "100,8,1,1"  "points" "[-60:60]" "auto"   "auto"  "repeat data in u direction"
 runSingleTest  6 "handmade_sloped.crg"    "100,8,1,1"  "points" "[-60:60]" "auto"   "auto"  "reflect data in u direction"
 runSingleTest  7 "handmade_sloped.crg"    "100,8,1,1"  "points" "auto"     "auto"   "auto"  "repeat data in u direction with offset"
 runSingleTest  8 "handmade_vtest.crg"     "90,90,1,1"  "points" "auto"     "[-8:8]" "auto"  "refuse query exceeding v dimensions of data"
 runSingleTest  9 "handmade_vtest.crg"     "90,90,1,1"  "points" "auto"     "auto"   "auto"  "set elevation to zero when exceeding v dimensions of data"
 runSingleTest 10 "handmade_vtest.crg"     "90,90,1,1"  "points" "auto"     "auto"   "auto"  "keep last value when exceeding v dimensions of data"
 runSingleTest 11 "handmade_vtest.crg"     "90,90,1,1"  "points" "auto"     "auto"   "auto"  "repeat data in v direction"
 runSingleTest 12 "handmade_vtest.crg"     "90,90,1,1"  "points" "auto"     "auto"   "auto"  "reflect data in v direction"
 runSingleTest 13 "handmade_vtest.crg"     "90,90,1,1"  "points" "auto"     "auto"   "auto"  "repeat data in v direction with offset"
 runSingleTest 14 "handmade_platform.crg"  "90, 0,1,1"  "points" "[-5:27]"  "auto"   "[0:2]" "10.0m smoothing zone at the begin, elevation zero outside u range"
 runSingleTest 15 "handmade_platform.crg"  "90, 0,1,1"  "points" "[-5:27]"  "auto"   "[0:2]" "10.0m smoothing zone at the end"
 runSingleTest 16 "handmade_circle.crg"    " 0, 0,1,1"  "points" "auto"     "auto"   "auto"  "extrapolating reference line"
 runSingleTest 17 "handmade_circle.crg"    " 0, 0,1,1"  "points" "auto"     "auto"   "auto"  "continuing (closing) reference line"

# finally a test reading the options/modifiers from a master file
 runSingleTest -1 "testOptionBorderMode.crg" "100,10,1,1" "points" "auto" "auto" "auto" "border options from file" 
 

echo "test results can be retrieved from <testReport.txt>"
