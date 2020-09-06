/* ===================================================
 *  file:       main.c
 * ---------------------------------------------------
 *  purpose:    main program for CRG test program
 *              calculating z values and  from a registered
 *              array of x/y co-ordinates
 * ---------------------------------------------------
 *  first edit: 29.09.2009 by M. Dupuis
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
    crgMsgPrint( dCrgMsgLevelNotice, "usage: crgDump [options] <filename>\n" );
    crgMsgPrint( dCrgMsgLevelNotice, "       options: -h    show this info\n" );
    crgMsgPrint( dCrgMsgLevelNotice, "                -u    plot u/v->z (default: plot x/y->z)\n" );
    crgMsgPrint( dCrgMsgLevelNotice, "                -xu   plot x/y->z (list u and v as last columns)\n" );
    crgMsgPrint( dCrgMsgLevelNotice, "                -d    enable debug mode\n" );
    crgMsgPrint( dCrgMsgLevelNotice, "       <filename> use indicated file as input file\n" );
    exit( -1 );
}

int main( int argc, char** argv ) 
{
    char*          filename  = "";
	FILE*          fPtr      = NULL;
    CrgDataStruct* crgData   = NULL;
    int            dataSetId = 0;
    int            i;
    int            j;
    int            cpId;
    int            count;
    int            resolution;
    int            plotUV   = 0;
    int            plotXYUV = 0;
    int            plotRefZ = 0;
    int            result;
    int            debugMode = 0;
    double         u;
    double         x;
    double         y;
    
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
        
        if ( !strcmp( *argv, "-u" ) )
            plotUV = 1;
        
        if ( !strcmp( *argv, "-xu" ) )
            plotXYUV = 1;
        
        if ( !strcmp( *argv, "-r" ) )
            plotRefZ = 1;
        
        if ( !strcmp( *argv, "-d" ) )
            debugMode = 1;
        
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
    
    if ( plotUV )
        crgMsgPrint( dCrgMsgLevelNotice, "Plotting uv->z.\n" );
    else if ( plotXYUV )
        crgMsgPrint( dCrgMsgLevelNotice, "Plotting x / y / z / u / v\n" );
    else
        crgMsgPrint( dCrgMsgLevelNotice, "Plotting xy->z.\n" );
    
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
    if ( ( fPtr = fopen( "crgDump.txt", "w" ) ) == NULL ) 
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
    
    crgMsgPrint( dCrgMsgLevelNotice, "main: u first/last = %.3f / %.3f \n", crgData->channelU.info.first, crgData->channelU.info.last );
    
    /* --- compute a bit coarser --- */
    resolution = 1;
    count      = 0;
    
    for ( i = 0; i < crgData->channelU.info.size; i += resolution )
    {
        u = crgData->channelU.info.first + i * crgData->channelU.info.inc;
        count++;
        
        for ( j = 0; j < crgData->channelV.info.size; j += resolution )
        {
            int index = j;
            
            if ( count % 2 )
                index = crgData->channelV.info.size - 1 - index;
            
            result = crgEvaluv2xy( cpId, u, crgData->channelV.data[index], &x, &y );
            
            if ( !result )
                crgMsgPrint( dCrgMsgLevelWarn, "main: error converting u/v = %.3f / %.3f to x/y.\n", u, crgData->channelV.data[index] );
            else if ( plotUV )
                fprintf( fPtr, "%10.4f %10.4f %10.6f\n", u, crgData->channelV.data[index], crgData->channelZ[index].data[i] + crgData->channelZ[index].info.mean );
            else if ( plotXYUV )
                fprintf( fPtr, "%.16e %.16e %.16e %.6f %.6f\n", x, y, crgData->channelZ[index].data[i] + crgData->channelZ[index].info.mean, u, crgData->channelV.data[index] );
            else if ( plotRefZ )
                fprintf( fPtr, "%.16e %.16e\n", u, crgData->channelRefZ.data[i] );
            else
                fprintf( fPtr, "%.3f %.3f %.3f\n", x, y, crgData->channelZ[index].data[i] + crgData->channelZ[index].info.mean );
        }
    }
    crgMsgPrint( dCrgMsgLevelNotice, "main: normal termination\n" );
    
    return 1;
}

