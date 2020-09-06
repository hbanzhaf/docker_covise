/* ===================================================
 *  file:       main.c
 * ---------------------------------------------------
 *  purpose:    main program for CRG test program
 *              calculating z values from a registered
 *              array of x/y co-ordinates
 * ---------------------------------------------------
 *  first edit: 27.07.2009 by M. Dupuis
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
#include "crgBaseLib.h"


void usage()
{
    crgMsgPrint( dCrgMsgLevelNotice, "usage: crgMemTest [options] <filename>\n" );
    crgMsgPrint( dCrgMsgLevelNotice, "       options: -h    show this info\n" );
    crgMsgPrint( dCrgMsgLevelNotice, "       <filename> use indicated file as input file\n" );
    exit( -1 );
}

int main( int argc, char** argv ) 
{
    char*  filename = "";
    int    dataSetId = 0;
    int    i;
    int    j;
    int    nStepsU = 20;
    int    nStepsV = 20;
    int    cpId;
    int    nTest = 0;
    double du;
    double dv;
    double uMin;
    double uMax;
    double vMin;
    double vMax;
    double u;
    double v;
    double z;
    double x;
    double y;
    
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
    
    /* --- first set the message level --- */
    crgMsgSetLevel( dCrgMsgLevelNotice );
    
    /* --- perform the following test endless (until user kills the process) --- */
    for (;;)
    {
        crgMsgPrint( dCrgMsgLevelNotice, "main: running test no. %d.\n", ++nTest );
    
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
        
        crgMsgPrint( dCrgMsgLevelNotice, "main: data set id = %d.\n", dataSetId );
        
        /* --- apply (default) modifiers --- */
        crgDataSetModifiersPrint( dataSetId );
        crgDataSetModifiersApply( dataSetId );
        
        /* --- create a contact point --- */
        cpId = crgContactPointCreate( dataSetId );
        
        if ( cpId < 0 )
        {
            crgMsgPrint( dCrgMsgLevelFatal, "main: could not create contact point.\n" );
            return -1;
        }
        
        /* --- get extents of data set --- */
        crgDataSetGetURange( dataSetId, &uMin, &uMax );
        crgDataSetGetVRange( dataSetId, &vMin, &vMax );
        
        /* --- now test the position conversion --- */
        du = ( uMax - uMin ) / nStepsU;
        dv = ( vMax - vMin ) / nStepsV;
    
        for ( i = 0; i <= nStepsU; i++ )
        {
            u = uMin + du * i;
            
            for ( j = 0; j <= nStepsV; j++ )
            {
                v = vMin + dv * j;
                
                /* --- check direct computation from u and v --- */
                if ( !crgEvaluv2z( cpId, u, v, &z ) )
                    crgMsgPrint( dCrgMsgLevelWarn, "main: error converting u/v = %+10.4f / %+10.4f to z.\n", u, v );
                
                /* --- check computation via x and y --- */
                if ( !crgEvaluv2xy( cpId, u, v, &x, &y ) )
                    crgMsgPrint( dCrgMsgLevelWarn, "main: error converting u/v = %.3f / %.3f to x/y.\n", u, v );
                else
                {
                   /* make sure no old values are used */
                    z = 0.0;
                    
                    /* --- now all the way back and check the result --- */
                    if ( !crgEvalxy2z( cpId, x, y, &z ) )
                        crgMsgPrint( dCrgMsgLevelWarn, "main: error converting x/y = %.3f / %.3f to z.\n",  x, y );
                }
            }
        }
        
        crgMsgPrint( dCrgMsgLevelNotice, "main: Finished tests. Releasing all data\n" ); 
        /* crgDataSetRelease( dataSetId ); */
        crgMemRelease();
        crgMsgPrint( dCrgMsgLevelNotice, "main: -----------------------------------------\n" ); 
    }
    crgMsgPrint( dCrgMsgLevelNotice, "main: normal termination\n" );
    
    return 1;
}

