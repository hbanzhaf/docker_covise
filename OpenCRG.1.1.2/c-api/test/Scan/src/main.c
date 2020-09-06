/* ===================================================
 *  file:       main.c
 * ---------------------------------------------------
 *  purpose:    main program for CRG test program
 *              calculating z values and from a 
 *              rectangular scan over the enclosing
 *              x/y co-ordinates
 * ---------------------------------------------------
 *  first edit: 02.03.2010 by M. Dupuis
 *  last mod.:  08.04.2014 by H. Helmich
 * ===================================================
    Copyright 2014 VIRES Simulationstechnologie GmbH

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
 */

/* ====== INCLUSIONS ====== */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include "crgBaseLibPrivate.h"


void usage()
{
    crgMsgPrint( dCrgMsgLevelNotice, "usage: crgSan [options] <filename>\n" );
    crgMsgPrint( dCrgMsgLevelNotice, "       options: -h                            show this info\n" );
    crgMsgPrint( dCrgMsgLevelNotice, "                -gp                           write output file in gnuplot format\n" );
    crgMsgPrint( dCrgMsgLevelNotice, "                -res <n>                      number of sample points per direction (x/y) \n" );
    crgMsgPrint( dCrgMsgLevelNotice, "                -d                            enable debug mode\n" );
    crgMsgPrint( dCrgMsgLevelNotice, "                -range <xMin yMin xMax yMax>  user-defined scan area\n" );
    crgMsgPrint( dCrgMsgLevelNotice, "                -uv                           calculate and print u/v co-ordinates\n" );
    crgMsgPrint( dCrgMsgLevelNotice, "       <filename> use indicated file as input file\n" );
    exit( -1 );
}

int main( int argc, char** argv ) 
{
    char*          filename  = "";
	FILE*          fPtr      = NULL;
    CrgDataStruct* crgData   = NULL;
    int            dataSetId = 0;
    int            cpId;
    int            count;
    int            result;
    int            debugMode = 0;
    int            forGnuPlot = 0;
    int            calcUV = 0;
    double         x;
    double         y;
    double         z;
    double         xMin = 0.0;
    double         yMin = 0.0;
    double         xMax = 0.0;
    double         yMax = 0.0;
    double         xInc;
    double         yInc;
    int            xRes = 200;
    int            yRes = 200;
    
    /* --- decode the command line --- */
    if ( argc < 2 )
        usage();
    
    argc--;
    
    while( argc )
    {
        argv++;
        argc--;
        
        if ( !strcmp( *argv, "-h" ) )
            usage();
        
        if ( !strcmp( *argv, "-d" ) )
            debugMode = 1;
        
        if ( !strcmp( *argv, "-gp" ) )
        {
            forGnuPlot = 1;
            crgMsgPrint( dCrgMsgLevelFatal, "main: writing gnuplot compatible data file.\n" );
        }
        
        if ( !strcmp( *argv, "-res" ) )
        {
            argv++;
            argc--;
            
            xRes = atoi( *argv );
            
            if ( xRes <= 0 ) 
                xRes = 1;
            
            yRes = xRes;
        }
        
        if ( !strcmp( *argv, "-range" ) )
        {
            argv++;
            argc--;
            xMin = atof( *argv );
            
            argv++;
            argc--;
            yMin = atof( *argv );
            
            argv++;
            argc--;
            xMax = atof( *argv );
            
            argv++;
            argc--;
            yMax = atof( *argv );
        }
        
        if ( !strcmp( *argv, "-d" ) )
            debugMode = 1;
        
        if ( !strcmp( *argv, "-uv" ) )
            calcUV = 1;
        
        if ( !argc ) /* last argument is the filename */
        {
            crgMsgPrint( dCrgMsgLevelInfo, "searching file\n" );
            
            if ( argc < 0 )
            {
                crgMsgPrint( dCrgMsgLevelFatal, "Name of input file is missing.\n" );
                usage();
            }
                
            filename = *argv;
        }
    }
    
    /* --- now load the file --- */
    if ( debugMode )
        crgMsgSetLevel( dCrgMsgLevelDebug );
    else
        crgMsgSetLevel( dCrgMsgLevelNotice );
    
    if ( ( dataSetId = crgLoaderReadFile( filename ) ) <= 0 )
    {
        crgMsgPrint( dCrgMsgLevelFatal, "main: error reading data.\n" );
        usage();
        return -1;
    }

    /* --- check CRG data for consistency and accuracy --- */
    if ( !crgCheck( dataSetId ) )
    {
        crgMsgPrint ( dCrgMsgLevelFatal, "main: could not validate crg data. \n" );
        return -1;
    }
    
    /* --- dump the data into a plot file --- */
    if ( ( fPtr = fopen( "crgScan.txt", "w" ) ) == NULL ) 
    {
        crgMsgPrint( dCrgMsgLevelFatal,  "main: could not open <%s>\n", filename );
        return -1;
    }

    /* --- access the data directly --- */
    crgData = crgDataSetAccess( dataSetId );
    
    if ( !crgData )
        return -1;
    
    /* --- create a contact point --- */
    cpId = crgContactPointCreate( dataSetId );
    
    if ( cpId < 0 )
    {
        crgMsgPrint( dCrgMsgLevelFatal, "main: could not create contact point.\n" );
        return -1;
    }
    
    /* --- apply some modifiers --- */
    crgDataSetModifiersPrint( dataSetId ); 
    crgDataSetModifiersApply( dataSetId );
    
    /* --- is the test range defined automatically or manually? --- */
    if ( xMin == 0.0 && yMin == 0.0 && xMax == 0.0 && yMax == 0.0 )
    {
        /* --- calculate min/max x/y values of reference line plus total width--- */
        xMin = crgData->channelX.info.first < crgData->channelX.info.last ? crgData->channelX.info.first : crgData->channelX.info.last;
        xMax = crgData->channelX.info.first > crgData->channelX.info.last ? crgData->channelX.info.first : crgData->channelX.info.last;
        yMin = crgData->channelY.info.first < crgData->channelY.info.last ? crgData->channelY.info.first : crgData->channelY.info.last;
        yMax = crgData->channelY.info.first > crgData->channelY.info.last ? crgData->channelY.info.first : crgData->channelY.info.last;
        
        xMin -= 0.5 * ( crgData->channelV.info.last - crgData->channelV.info.first );
        yMin -= 0.5 * ( crgData->channelV.info.last - crgData->channelV.info.first );
        xMax += 0.5 * ( crgData->channelV.info.last - crgData->channelV.info.first );
        yMax += 0.5 * ( crgData->channelV.info.last - crgData->channelV.info.first );
    }
    
    xInc = ( xMax - xMin ) / xRes;
    yInc = ( yMax - yMin ) / yRes;
    
    crgMsgPrint( dCrgMsgLevelNotice, "main: xMin/Max = %.3f / %.3f, yMin/Max = %.6f / %.6f \n", xMin, xMax, yMin, yMax );
    
    count = 0;
    
    for ( y = yMin; y <= yMax; y += yInc )
    {
        for ( x = xMin; x <= xMax; x += xInc )
        {
            count++;
            
            result = crgEvalxy2z( cpId, x, y, &z );
            
            if ( !result )
                crgMsgPrint( dCrgMsgLevelWarn, "main: error converting x/y = %.3f / %.3f to z.\n", x, y );
            
            if ( calcUV )
            {
                double u;
                double v;
                crgEvalxy2uv( cpId, x, y, &u, &v );
                fprintf( fPtr, "%10.4f %10.4f %10.4f %10.4f %10.6f\n", x, y, u, v, z );
            }
            else
                fprintf( fPtr, "%10.4f %10.4f %10.6f\n", x, y, z );
            
            if ( forGnuPlot )
            {
                result = crgEvalxy2z( cpId, x, y + yInc, &z );
                
                fprintf( fPtr, "%10.4f %10.4f %10.6f\n", x, y + yInc, z );
                fprintf( fPtr, "\n" );
    
                result = crgEvalxy2z( cpId, x + xInc, y, &z );
                
                fprintf( fPtr, "%10.4f %10.4f %10.6f\n", x + xInc, y, z );
                           
                result = crgEvalxy2z( cpId, x + xInc, y + yInc, &z );
                
                fprintf( fPtr, "%10.4f %10.4f %10.6f\n", x + xInc, y + yInc, z );
                fprintf( fPtr, "\n\n" );
            }
        }
    }
    crgMsgPrint( dCrgMsgLevelNotice, "main: computed %d test points\n", count );
    crgMsgPrint( dCrgMsgLevelNotice, "main: normal termination\n" );
    
    return 1;
}

