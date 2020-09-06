/* ===================================================
 *  file:       main.c
 * ---------------------------------------------------
 *  purpose:    main program for CRG test program
 *              calculating z values from given 
 *              x/y and u/v co-ordinates
 * ---------------------------------------------------
 *  first edit: 27.11.2008 by M. Dupuis
 *  last mod.:  08.04.2014 by H. Helmich @ VIRES GmbH
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
#include <string.h>
#include <math.h>
#include "crgBaseLib.h"


void usage()
{
    crgMsgPrint( dCrgMsgLevelNotice, "usage: crgEvalz [options] <filename>\n" );
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
    int    nStepsU  = 20;
    int    nStepsV  = 20;
    int    nBorderU = 10;
    int    nBorderV = 10;
    int    cpId;
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
    double phi;
    double curv;
    
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

    for ( i = -nBorderU; i <= nStepsU+nBorderU; i++ )
    {
        u = uMin + du * i;
        
        for ( j = -nBorderV; j <= nStepsV+nBorderV; j++ )
        {
            v = vMin + dv * j;
            
            /* --- check direct computation from u and v --- */
            if ( !crgEvaluv2z( cpId, u, v, &z ) )
                crgMsgPrint( dCrgMsgLevelWarn, "main: error converting u/v = %+10.4f / %+10.4f to z.\n", u, v );
            else
                crgMsgPrint( dCrgMsgLevelNotice, "main: converted u/v = %+10.4f / %+10.4f ----> z = %+10.4f\n", u, v, z );
            
            /* --- check computation via x and y --- */
            if ( !crgEvaluv2xy( cpId, u, v, &x, &y ) )
                crgMsgPrint( dCrgMsgLevelWarn, "main: error converting u/v = %.3f / %.3f to x/y.\n", u, v );
            else
            {
                /* remember old z value */
                double zMem = z;
                
                /* make sure no old values are used */
                z = 0.0;
                
                /* --- now all the way back and check the result --- */
                if ( !crgEvalxy2z( cpId, x, y, &z ) )
                    crgMsgPrint( dCrgMsgLevelWarn, "main: error converting x/y = %.3f / %.3f to z.\n",  x, y );
                else
                {
                    crgMsgPrint( dCrgMsgLevelNotice, "main: converted x/y = %+10.4f / %+10.4f ----> z = %+10.4f\n", x, y, z );
                    
                    /* any discrepancies? */
                    if ( fabs( z - zMem ) > 1.e-4 )
                        crgMsgPrint( dCrgMsgLevelWarn, "main: inconsistent z values: %+10.4f vs. %+10.4f\n", zMem, z );
                }
            }
            
            /* --- calculate heading angle and curvature --- */
            if ( !crgEvaluv2pk( cpId, u, v, &phi, &curv ) )
                crgMsgPrint( dCrgMsgLevelWarn, "main: error converting u/v = %+10.4f / %+10.4f to phi/kappa.\n", u, v );
            else
                crgMsgPrint( dCrgMsgLevelNotice, "main: converted u/v = %+10.4f / %+10.4f ----> phi = %+10.4f kappa = %+10.6f\n", 
                                                  u, v, phi, curv );
        }
    }
    crgMsgPrint( dCrgMsgLevelNotice, "main: normal termination\n" );
    
    return 1;
}

