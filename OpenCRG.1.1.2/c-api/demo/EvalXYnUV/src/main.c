/* ===================================================
 *  file:       main.c
 * ---------------------------------------------------
 *  purpose:    main program for CRG test program
 *              converting x/y and u/v co-ordinates
 * ---------------------------------------------------
 *  first edit: 18.11.2008 by M. Dupuis
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
    crgMsgPrint( dCrgMsgLevelNotice, "usage: crgEvalxyuv [options] <filename>\n" );
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
        
        if ( !argc )    /* last argument is the filename */
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
            
            if ( !crgEvaluv2xy( cpId, u, v, &x, &y ) )
                crgMsgPrint( dCrgMsgLevelWarn, "main: error converting u/v = %.3f / %.3f to x/y.\n", u, v );
            else
            {
                double uMem = u;
                double vMem = v;
                
                /* --- now all the way back and check the result --- */
                if ( !crgEvalxy2uv( cpId, x, y, &u, &v ) )
                    crgMsgPrint( dCrgMsgLevelWarn, "main: error converting x/y = %.3f / %.3f to u/v.\n",  x, y );
                else
                {
                    double deltaU = uMem - u;
                    double deltaV = vMem - v;
                    
                    crgMsgPrint( dCrgMsgLevelNotice, "main: u/v = %+10.4f / %+10.4f ----> x/y = %+10.4f / %+10.4f ----> u/v = %+10.4f / %+10.4f.\n",
                                                     uMem, vMem, x, y, u, v );
                                                    
                    if ( fabs( deltaU ) > 1.0e-5 || fabs( deltaV ) > 1.e-5 )
                        crgMsgPrint( dCrgMsgLevelNotice, "main: computation error when converting back: du/dv = %.8f / %.8f\n",
                                                         deltaU, deltaV );
                }
            }
        }
    }
    crgMsgPrint( dCrgMsgLevelNotice, "main: normal termination\n" );
    
    return 1;
}

