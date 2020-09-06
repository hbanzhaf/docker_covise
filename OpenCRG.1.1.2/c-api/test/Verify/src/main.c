/* ===================================================
 *  file:       main.c
 * ---------------------------------------------------
 *  purpose:    main program for CRG test program
 *              reads a file of x/y/z data, computes 
 *              z again at given x/y locations from
 *              given CRG data file and compares
 *              the result; primary purpose: guarantee
 *              consistency between e.g. Matlab and
 *              C-API routines
 * ---------------------------------------------------
 *  first edit: 31.10.2009 by M. Dupuis
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
    crgMsgPrint( dCrgMsgLevelNotice, "usage: crgVerify [options] <CRG file> <XYZ file>\n" );
    crgMsgPrint( dCrgMsgLevelNotice, "       options: -h    show this info\n" );
    crgMsgPrint( dCrgMsgLevelNotice, "                -t n  tolerance value for z comparison [m]\n" );
    crgMsgPrint( dCrgMsgLevelNotice, "       <CRG file> CRG file from which to compute the z data\n" );
    crgMsgPrint( dCrgMsgLevelNotice, "       <XYZ file> file containing the x/y/z positions for verification\n" );
    exit( -1 );
}

int main( int argc, char** argv ) 
{
    char*          crgFilename  = "";
    char*          xyzFilename  = "";
	FILE*          fPtr         = NULL;
    CrgDataStruct* crgData      = NULL;
    char           buffer[1024];
    int            dataSetId = 0;
    unsigned int   lineNo = 0;
    int            cpId;
    int            result;
    double         tolerance = 1.0e-4;
    double         minError  = 0.0;
    double         maxError  = 0.0;
    double         meanError = 0.0;
    unsigned int   noTestPoints = 0;
    unsigned int   noErrors     = 0;
    unsigned int   noFail       = 0;
    
    /* --- decode the command line --- */
    if ( argc < 3 )
        usage();
    
    argc--;
    
    while( argc )
    {
        argv++;
        argc--;
        
        if ( !strcmp( *argv, "-h" ) )
            usage();
        
        if ( !strcmp( *argv, "-t" ) )
        {
            argc--;
            argv++;
            
            if ( argc >= 0 )
                tolerance = atof( *argv );
        }
        else
        {
            if ( argc == 1 ) /* CRG file */
                crgFilename = *argv;
            
            if ( argc == 0 ) /* XYZ file */
                xyzFilename = *argv;
        }
        
        if ( argc < 0 )
        {
            crgMsgPrint( dCrgMsgLevelFatal, "Name of input file is missing.\n" );
            usage();
        }
    }
    
    if ( !strlen( crgFilename ) || !strlen( xyzFilename ) )
    {
        crgMsgPrint( dCrgMsgLevelFatal, "Empty or missing name of CRG or XYZ input file.\n" );
        usage();
    }
    
    /* --- now load the file --- */
    crgMsgSetLevel( dCrgMsgLevelNotice );
    
    if ( ( dataSetId = crgLoaderReadFile( crgFilename ) ) <= 0 )
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
    if ( ( fPtr = fopen( xyzFilename, "r" ) ) == NULL ) 
    {
        crgMsgPrint( dCrgMsgLevelFatal,  "main: could not open <%s>\n", xyzFilename );
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
    
    /* --- print the applicable options --- */
    crgContactPointOptionsPrint( cpId );
    
    /* --- apply some modifiers --- */
    crgDataSetModifiersPrint( dataSetId ); 
    crgDataSetModifiersApply( dataSetId );

    memset( buffer, 0, sizeof( buffer ) );
    
    while ( fgets( buffer, 1024, fPtr ) )
    {
        lineNo++;
        
        if ( buffer[0] != '#' )
        {
            double x;
            double y;
            double z;
            double zRef;
            double uRef;
            double vRef;
            int    doTest  = 0;
            int    hasuRef = 1;
            
            doTest = sscanf( buffer, "%lf%lf%lf%lf%lf", &x, &y, &zRef, &uRef, &vRef ) == 5;
            
            if ( !doTest )
            {
                doTest  = sscanf( buffer, "%lf%lf%lf", &x, &y, &zRef ) == 3;
                uRef    = -1.0;
                vRef    = -1.0;
                hasuRef = 0;
            }
            
            if ( doTest )
            {
                noTestPoints++;
                
                result = crgEvalxy2z( cpId, x, y, &z );
                
                if ( result )
                {
                    double delta = fabs( z - zRef );
                    
                    if ( delta > tolerance )
                    {
                        double u;
                        double v;
                        
                        noErrors++;
                        crgMsgPrint( dCrgMsgLevelWarn, "main: value out of tolerance at x/y = %.6f / %.6f deltaZ = %.6f (line %d of input file)\n", 
                                                       x, y, delta, lineNo );
                        if ( hasuRef )
                        {
                            double xRef = x;
                            double yRef = y;
                            
                            crgEvalxy2uv( cpId, x, y, &u, &v );
                        
                            crgMsgPrint( dCrgMsgLevelWarn, "      u / v = %.6f / %.6f, uRef / vRef = %.6f / %.6f, du / dv = %.6f / %.6f\n", 
                                                       u, v, uRef, vRef, u - uRef, v - vRef );

                            crgMsgPrint( dCrgMsgLevelWarn, "      computing values from uRef / vRef = %.6f / %.6f\n", uRef, vRef );
                            crgEvaluv2xy( cpId, uRef, vRef, &x, &y );
                            crgEvalxy2z( cpId, x, y, &z );
                            crgMsgPrint( dCrgMsgLevelWarn, "      uRef/vRef -> x/y -> z   = %.6f / %.6f -> %.6f / %.6f -> %.6f\n", uRef, vRef, x, y, z );
                            crgMsgPrint( dCrgMsgLevelWarn, "                   x/y/z Ref  = %.6f / %.6f / %.6f\n", xRef, yRef, zRef );
                            crgEvaluv2z( cpId, uRef, vRef, &z );
                            crgMsgPrint( dCrgMsgLevelWarn, "      uRef/vRef -> z          = %.6f\n", z );
                            if ( uRef < crgData->channelU.info.first || uRef > crgData->channelU.info.last )
                                crgMsgPrint( dCrgMsgLevelWarn, "                                                                 uRef out of bounds!!!\n" );
                            if ( vRef < crgData->channelV.info.first || vRef > crgData->channelV.info.last )
                                crgMsgPrint( dCrgMsgLevelWarn, "                                                                 vRef out of bounds!!!\n" );

                                                     
                        }
                                                       
                        if ( minError == 0.0 || ( delta < minError ) )
                            minError = delta;
                        
                        if ( maxError == 0.0 || ( delta > maxError ) )
                            maxError = delta;
                        
                        meanError += delta;
                    }
                }
                else if ( !( zRef != zRef ) ) /* NaN is not counted as a failure */
                    noFail++;
            }
            else
                crgMsgPrint( dCrgMsgLevelWarn, "main: error in line %d.\n", lineNo );
                
        }
    }
    
    crgMsgPrint( dCrgMsgLevelNotice, "main: finished verification. Result:\n" );
    crgMsgPrint( dCrgMsgLevelNotice, " Number of test points:             %d\n",  noTestPoints );
    crgMsgPrint( dCrgMsgLevelNotice, " Tolerance:                        %.6f\n", tolerance );
    crgMsgPrint( dCrgMsgLevelNotice, " Number of failed queries:          %d\n",  noFail );
    crgMsgPrint( dCrgMsgLevelNotice, " Number of values out of tolerance: %d\n",  noErrors );
    
    if ( noErrors )
    {
        crgMsgPrint( dCrgMsgLevelNotice, " Minimum absolute error: %.6f\n",  minError );
        crgMsgPrint( dCrgMsgLevelNotice, " Maximum absolute error: %.6f\n",  maxError );
        crgMsgPrint( dCrgMsgLevelNotice, " Mean absolute error:    %.6f\n",  meanError / noErrors );
    }

    crgMsgPrint( dCrgMsgLevelNotice, "main: normal termination\n" );
    
    return 1;
}

