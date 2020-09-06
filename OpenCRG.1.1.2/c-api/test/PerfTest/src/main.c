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
#include "crgBaseLibPrivate.h"


void usage()
{
    crgMsgPrint( dCrgMsgLevelNotice, "usage: crgPerfTest [options] <filename>\n" );
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
    int    k;
    int    cpId;
    double uMin;
    double uMax;
    double vMin;
    double vMax;
    double u;
    double v;
    double z;
    double x;
    double y;
    double phi;
    double curv;
    
    /* --- this code simulates the ride with a car having four wheels
       and a patch of n by n points under each wheel used as 
       contact surface running along the data set with a certain
       variance in u/v co-ordinates --- */
       
    /* --- here are the required variables for this test --- */
#define FULL_TEST    
#ifdef FULL_TEST    
    double wheelPatchWidth  = 0.1;   /* [m] width of the patch under a wheel                */
    double wheelPatchLength = 0.1;   /* [m] length of the patch under a wheel               */
    int    noPtsPatchWidth  =  10;   /* [-] number of tesselation points along patch width  */
    int    noPtsPatchLength =  10;   /* [-] number of tesselation points along patch length */
    int    noWheels         =   4;   /* [-] number of wheels                                */

    double wheelBase = 2.50;     /* [m] distance from front to rear axle  */
    double wheelDist = 1.45;     /* [m] distance from left to right wheel */
    double stepSizeU = 0.01;     /* [m] per time step                     */
#else
    double wheelPatchWidth  = 0.1;   /* [m] width of the patch under a wheel                */
    double wheelPatchLength = 0.1;   /* [m] length of the patch under a wheel               */
    int    noPtsPatchWidth  =   1;   /* [-] number of tesselation points along patch width  */
    int    noPtsPatchLength =   1;   /* [-] number of tesselation points along patch length */
    int    noWheels         =   4;   /* [-] number of wheels                                */

    double wheelBase = 2.50;     /* [m] distance from front to rear axle  */
    double wheelDist = 1.45;     /* [m] distance from left to right wheel */
    double stepSizeU = 0.1;      /* [m] per time step                     */
#endif
    
    double *testX = 0;      /* array of x positions to be used for queries */
    double *testY = 0;      /* array of y positions to be used for queries */
    size_t noTestPts;       /* size of the test point array                */
    size_t idxTestPt;       /* test point index                            */

    struct timeval tme;     /* measure the run-time                        */
    double startTime;       /* [s]                                         */
    double endTime;         /* [s]                                         */
    
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
    
    /* --- now load the file --- */
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
    
    /* --- compute the array of x/y contact points to be fed into the algorithms later on --- */
    noTestPts = ( size_t ) ( ( ( uMax - uMin ) * noPtsPatchWidth * noPtsPatchLength * noWheels / stepSizeU ) + 0.5 );
    
    /* --- apply modifiers --- */
    crgDataSetModifiersPrint( dataSetId );
    crgDataSetModifiersApply( dataSetId );
    
    /* --- set and print the current options --- */
    crgContactPointSetDefaultOptions( cpId );
    crgContactPointOptionsPrint( cpId );
    
    crgMsgPrint( dCrgMsgLevelDebug, "main: generating %ld test points\n", noTestPts );
    
    testX = ( double* ) calloc( noTestPts, sizeof( double ) );
    testY = ( double* ) calloc( noTestPts, sizeof( double ) );
    
    if ( !testX || !testY )
    {
        crgMsgPrint( dCrgMsgLevelNotice, "main: could not allocate memory. Sorry.\n" );
        exit( -1 );
    }
    
    /* --- start collecting the points for the queries. Some of the first points will be --- */
    /* --- out of range, but also this behavior should be measured                       --- */
    u         = uMin;
    v         = 0.0;
    idxTestPt = 0;
    
    while ( ( u < uMax ) && ( idxTestPt <  noTestPts - 1 ) )
    {
        /* --- compute the reference point (center of rear axle) --- */
        
        if ( !crgEvaluv2xy( cpId, u, v, &x, &y ) )
            crgMsgPrint( dCrgMsgLevelWarn, "main: error converting u/v = %.3f / %.3f to x/y.\n", u, v );
        else
        {
            crgMsgPrint( dCrgMsgLevelDebug, "main:  u/v = %.3f / %.3f   x/y = %.3f / %.3f.\n", u, v, x, y );
            crgEvaluv2pk( cpId, u, v, &phi, &curv );
            
            for ( i = 0; i < noWheels; i++ )
            {
                double dx = 0.0;
                double dy = 0.0;
                double wx;
                double wy;
                
                switch ( i )
                {
                    case 0: /* left rear wheel */
                        dx = 0.0;
                        dy = 0.5 * wheelDist;
                        break;
                    case 1: /* right rear wheel */
                        dx =  0.0;
                        dy = -0.5 * wheelDist;
                        break;
                    case 2: /* left front wheel */
                        dx = wheelBase;
                        dy = 0.5 * wheelDist;
                        break;
                    case 3: /* right front wheel */
                        dx =  wheelBase;
                        dy = -0.5 * wheelDist;
                        break;
                }
                
                for ( j = 0; j < noPtsPatchLength; j++ )
                {
                    if ( noPtsPatchLength > 1 )
                        wx = dx - 0.5 * wheelPatchLength + ( wheelPatchLength / ( noPtsPatchLength - 1 ) ) * j;
                    else
                        wx = dx;
                    
                    for ( k = 0; k < noPtsPatchWidth; k++ )
                    {
                        if ( noPtsPatchWidth > 1 )
                            wy = dy - 0.5 * wheelPatchWidth + ( wheelPatchWidth / ( noPtsPatchWidth - 1 ) ) * k;
                        else
                            wy = dy;
                        
                        /* compute the real-world co-ordinates of the test point (convert vehicle to inertial co-ordinates) */
                        testX[idxTestPt] = x + wx * cos( phi ) - wy * sin( phi );
                        testY[idxTestPt] = y + wy * cos( phi ) + wx * sin( phi );
                        
                        idxTestPt++;
                    }
                }
            }
        }
        u += stepSizeU;
    }
    
    if ( idxTestPt < noTestPts )
        noTestPts = idxTestPt;

    crgMsgPrint( dCrgMsgLevelNotice, "main: generated %d test points. Now running actual test....\n", idxTestPt );

    crgContactPointActivatePerfStat( cpId );
    
    /* --- all right, I have the test points, now let's go through all of them and measure the time required --- */
    gettimeofday(&tme, 0);
    startTime = tme.tv_sec + 1.0e-6 * tme.tv_usec;
    
    idxTestPt = 0;
    
    while ( idxTestPt < noTestPts )
    {
        /* make sure no old values are used */
        z = 0.0;
        
        /* --- now all the way back and check the result --- */
        if ( !crgEvalxy2z( cpId, testX[idxTestPt], testY[idxTestPt], &z ) )
            crgMsgPrint( dCrgMsgLevelWarn, "main: error converting x/y = %.3f / %.3f to z.\n", testX[idxTestPt], testY[idxTestPt] );
        
        /* fprintf( stderr, "%.3lf %.3lf %.3lf\n", testX[idxTestPt], testY[idxTestPt], z ); */
        
        idxTestPt++;
    }
    
    gettimeofday(&tme, 0);
    endTime = tme.tv_sec + 1.0e-6 * tme.tv_usec;
    
    crgMsgPrint( dCrgMsgLevelWarn, "main: total time for %d queries: %.3lf seconds (i.e. %.3lfus per query)\n",  noTestPts, endTime - startTime, ( endTime - startTime ) / noTestPts * 1.0e6 );
    
    crgContactPointPrintPerfStat( cpId );

    crgMsgPrint( dCrgMsgLevelNotice, "main: normal termination\n" );
    
    return 1;
}

