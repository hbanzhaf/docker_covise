/* ===================================================
 *  file:       main.c
 * ---------------------------------------------------
 *  purpose:    main program for CRG test program
 *              demonstrating the effects of various
 *              option settings
 * ---------------------------------------------------
 *  first edit: 13.01.2009 by M. Dupuis
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
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "crgBaseLib.h"


void usage()
{
    crgMsgPrint( dCrgMsgLevelNotice, "usage: crgOpts [options] <filename>\n" );
    crgMsgPrint( dCrgMsgLevelNotice, "       options: -h      show this info\n" );
    crgMsgPrint( dCrgMsgLevelNotice, "                -t <n>  run only test no. <n>\n" );
    crgMsgPrint( dCrgMsgLevelNotice, "                -p      write resulting data to <crgPlotFile.txt>\n" );
    crgMsgPrint( dCrgMsgLevelNotice, "       <filename>  input file, default: [%s]\n", "../../Data/handmade_straight.crg" );
    exit( -1 );
}

/* --- for demonstration purposes, define a callback method for the message --- */
/* --- outputs instead of printing them directly to the console             --- */
int myMsgHandler( int level, char* message )
{
    static int msgNo = 0;
    
    /* --- count the messages --- */
    msgNo++;

    /* --- remove trailing \n characters --- */
    if ( strlen( message ) )
    {
        char *bufPtr = &( message[ strlen( message ) - 1 ] );
        
        while ( ( *bufPtr == '\n' ) && ( bufPtr != message ) )
        {
            *bufPtr = '\0';
            bufPtr--;
        }
    }
    printf( "myMsgHandler: got msg no. %d, level = %d, text = <%s>\n",
            msgNo, level, message );
            
    return 0;
}

int main( int argc, char** argv ) 
{
    char*  filename = "../../Data/handmade_straight.crg";
    int    dataSetId = 0;
    int    cpId;
    int    testNo = 0;
    int    i;
    int    j;
    int    nStepsU       = 22;
    int    nStepsV       = 6;
    int    nExcessStepsU = 50;
    int    nExcessStepsV = 10;
    int    done          = 0;
    int    singleTest    = 0;
    int    writePlotFile = 0;
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
    FILE*  fPtr = NULL;
    
    /* --- for test purposes, pipe handling of messages to my own routine --- */
    crgMsgSetCallback( myMsgHandler );
    
    /* --- print the release notes --- */
    crgMsgPrint( dCrgMsgLevelNotice, "Library version info: <%s>\n", crgGetReleaseInfo() );
    
    /* --- decode the command line --- */
    if ( argc < 2 )
        usage();
    
    while ( argc > 1 )
    {
        argv++;
        argc--;
        
        if ( !strcmp( *argv, "-h" ) )
            usage();
        else if ( !strcmp( *argv, "-t" ) )
        {
            argv++;
            argc--;
            
            testNo = atoi( *argv );
            singleTest = 1;
        }
        else if ( !strcmp( *argv, "-p" ) )
        {
            writePlotFile = 1;
            
            fPtr = fopen( "crgPlotData.txt", "w+" );
            
            if ( !fPtr )
            {
                crgMsgPrint( dCrgMsgLevelFatal, "main: error opening plot file <crgPlotData.txt>.\n" );
                return -1;
            }
                
            /* --- be silent, with the exception of fatal messages --- */
            crgMsgSetLevel( dCrgMsgLevelFatal );
        }
        else
            filename = *argv;
    }
    
    /* --- now load the file --- */
    if ( ( dataSetId = crgLoaderReadFile( filename ) ) <= 0 )
    {
        crgMsgPrint( dCrgMsgLevelFatal, "main: error reading data file <%s>.\n", filename );
        return -1;
    }

    /* --- check CRG data for consistency and accuracy --- */
    if ( !crgCheck( dataSetId ) )
    {
        crgMsgPrint ( dCrgMsgLevelFatal, "main: could not validate crg data. \n" );
        return -1;
    }
    
    /* --- create a contact point for evaluation of the data set --- */
    if ( ( cpId = crgContactPointCreate( dataSetId ) ) < 0 )
    {
        crgMsgPrint( dCrgMsgLevelFatal, "main: could not create contact point.\n" );
        return -1;
    }
    
    /* --- run a series of tests each using different option settings --- */
    crgMsgPrint( dCrgMsgLevelNotice, "main: performing tests....\n" );
    
    while ( !done )
    {
        crgMsgPrint( dCrgMsgLevelNotice, "main: --------------------------------------------------------------\n" );
        crgMsgPrint( dCrgMsgLevelNotice, "main: Starting test no. %d\n", testNo );
        
        /* --- reset any modified options to the default state --- */
        if ( testNo >= 0 )
            crgContactPointSetDefaultOptions( cpId );
        
        /* --- set the options according to the test number --- */
        switch ( testNo )
        {
            case -1:
                crgMsgPrint( dCrgMsgLevelNotice, "main: test purpose: using options / modifiers from file\n" );
                break;
                
            case 0:
                crgMsgPrint( dCrgMsgLevelNotice, "main: test purpose: default settings\n" );
                break;
                
            case 1:
                crgMsgPrint( dCrgMsgLevelNotice, "main: test purpose: curvature on reference line\n" );
                crgContactPointOptionSetInt( cpId, dCrgCpOptionCurvMode, dCrgCurvRefLine );
                break;
                
            case 2:
                crgMsgPrint( dCrgMsgLevelNotice, "main: test purpose: refuse query exceeding u dimensions of data\n" );
                crgContactPointOptionSetInt( cpId, dCrgCpOptionBorderModeU, dCrgBorderModeNone );
                break;
                
            case 3:
                crgMsgPrint( dCrgMsgLevelNotice, "main: test purpose: set elevation to zero when exceeding u and v dimensions of data\n" );
                crgContactPointOptionSetDouble( cpId, dCrgCpOptionBorderOffsetU, -1.0 );
                /* crgContactPointOptionSetDouble( cpId, dCrgCpOptionBorderOffsetV, -1.0 ); */
                crgContactPointOptionSetInt( cpId, dCrgCpOptionBorderModeU, dCrgBorderModeExZero );
                crgContactPointOptionSetInt( cpId, dCrgCpOptionBorderModeV, dCrgBorderModeExZero );
                break;
                
            case 4:
                crgMsgPrint( dCrgMsgLevelNotice, "main: test purpose: keep last value when exceeding u dimensions of data\n" );
                crgContactPointOptionSetInt( cpId, dCrgCpOptionBorderModeU, dCrgBorderModeExKeep );
                break;
               
            case 5:
                crgMsgPrint( dCrgMsgLevelNotice, "main: test purpose: repeat data in u direction\n" );
                crgContactPointOptionSetInt( cpId, dCrgCpOptionBorderModeU, dCrgBorderModeRepeat );
                break;
                
            case 6:
                crgMsgPrint( dCrgMsgLevelNotice, "main: test purpose: reflect data in u direction\n" );
                crgContactPointOptionSetInt( cpId, dCrgCpOptionBorderModeU, dCrgBorderModeReflect );
                break;
                
            case 7:
                crgMsgPrint( dCrgMsgLevelNotice, "main: test purpose: repeat data in u direction with offset\n" );
                crgContactPointOptionSetDouble( cpId, dCrgCpOptionBorderOffsetU, 1.0 );
                crgContactPointOptionSetInt( cpId, dCrgCpOptionBorderModeU, dCrgBorderModeRepeat );
                break;
                
            case 8:
                crgMsgPrint( dCrgMsgLevelNotice, "main: test purpose: refuse query exceeding v dimensions of data\n" );
                crgContactPointOptionSetInt( cpId, dCrgCpOptionBorderModeV, dCrgBorderModeNone );
                break;
                
            case 9:
                crgMsgPrint( dCrgMsgLevelNotice, "main: test purpose: set elevation to zero when exceeding v dimensions of data\n" );
                crgContactPointOptionSetInt( cpId, dCrgCpOptionBorderModeV, dCrgBorderModeExZero );
                break;
                
            case 10:
                crgMsgPrint( dCrgMsgLevelNotice, "main: test purpose: keep last value when exceeding v dimensions of data\n" );
                crgContactPointOptionSetInt( cpId, dCrgCpOptionBorderModeV, dCrgBorderModeExKeep );
                break;
                
            case 11:
                crgMsgPrint( dCrgMsgLevelNotice, "main: test purpose: repeat data in v direction\n" );
                crgContactPointOptionSetInt( cpId, dCrgCpOptionBorderModeV, dCrgBorderModeRepeat );
                break;
                
            case 12:
                crgMsgPrint( dCrgMsgLevelNotice, "main: test purpose: reflect data in v direction\n" );
                crgContactPointOptionSetInt( cpId, dCrgCpOptionBorderModeV, dCrgBorderModeReflect );
                break;
                
            case 13:
                crgMsgPrint( dCrgMsgLevelNotice, "main: test purpose: repeat data in v direction with offset\n" );
                crgContactPointOptionSetDouble( cpId, dCrgCpOptionBorderOffsetV, 1.0 );
                crgContactPointOptionSetInt( cpId, dCrgCpOptionBorderModeV, dCrgBorderModeRepeat );
                break;
                
            case 14:
                crgMsgPrint( dCrgMsgLevelNotice, "main: test purpose: 10.0m smoothing zone at the begin\n" );
                crgContactPointOptionSetInt( cpId, dCrgCpOptionBorderModeU, dCrgBorderModeExZero );
                crgContactPointOptionSetDouble( cpId, dCrgCpOptionSmoothUBegin, 10.0 );
                break;
                
            case 15:
                crgMsgPrint( dCrgMsgLevelNotice, "main: test purpose: 10.0m smoothing zone at the end\n" );
                crgContactPointOptionSetDouble( cpId, dCrgCpOptionSmoothUEnd, 10.0 );
                break;
                
            case 16:
                crgMsgPrint( dCrgMsgLevelNotice, "main: test purpose: extrapolating reference line\n" );
                crgContactPointOptionSetInt( cpId, dCrgCpOptionBorderModeU, dCrgBorderModeExKeep );
                crgContactPointOptionSetInt( cpId, dCrgCpOptionBorderModeV, dCrgBorderModeExKeep );
                crgContactPointOptionSetInt( cpId, dCrgCpOptionRefLineContinue, dCrgRefLineExtrapolate );
                break;
                
            case 17:
                crgMsgPrint( dCrgMsgLevelNotice, "main: test purpose: continuing (closing) reference line\n" );
                crgContactPointOptionSetInt( cpId, dCrgCpOptionBorderModeU, dCrgBorderModeExKeep );
                crgContactPointOptionSetInt( cpId, dCrgCpOptionBorderModeV, dCrgBorderModeNone );
                crgContactPointOptionSetInt( cpId, dCrgCpOptionRefLineContinue, dCrgRefLineCloseTrack );
                break;
                
            case 18:
                crgMsgPrint( dCrgMsgLevelNotice, "main: test purpose: z scaling\n" );
                crgDataSetModifierRemoveAll( dataSetId );
                crgDataSetModifierSetInt( dataSetId, dCrgModGridNaNMode, dCrgGridNaNKeepLast );
                crgDataSetModifierSetDouble( dataSetId, dCrgModScaleZ, 2.0 );
                break;
                
            case 19:
                crgMsgPrint( dCrgMsgLevelNotice, "main: test purpose: slope scaling\n" );
                crgDataSetModifierRemoveAll( dataSetId );
                crgDataSetModifierSetInt( dataSetId, dCrgModGridNaNMode, dCrgGridNaNKeepLast );
                crgDataSetModifierSetDouble( dataSetId, dCrgModScaleSlope, 10.0 );
                break;
                
            case 20:
                crgMsgPrint( dCrgMsgLevelNotice, "main: test purpose: bank scaling\n" );
                crgDataSetModifierRemoveAll( dataSetId );
                crgDataSetModifierSetInt( dataSetId, dCrgModGridNaNMode, dCrgGridNaNKeepLast );
                crgDataSetModifierSetDouble( dataSetId, dCrgModScaleBank, 10.0 );
                break;
                
            case 21:
                crgMsgPrint( dCrgMsgLevelNotice, "main: test purpose: length scaling\n" );
                crgDataSetModifierRemoveAll( dataSetId );
                crgDataSetModifierSetInt( dataSetId, dCrgModGridNaNMode, dCrgGridNaNKeepLast );
                crgDataSetModifierSetDouble( dataSetId, dCrgModScaleLength, 2.0 );
                break;
                
            case 22:
                crgMsgPrint( dCrgMsgLevelNotice, "main: test purpose: width scaling\n" );
                crgDataSetModifierRemoveAll( dataSetId );
                crgDataSetModifierSetInt( dataSetId, dCrgModGridNaNMode, dCrgGridNaNKeepLast );
                crgDataSetModifierSetDouble( dataSetId, dCrgModScaleWidth, 2.0 );
                break;
                
            case 23:
                crgMsgPrint( dCrgMsgLevelNotice, "main: test purpose: curvature scaling\n" );
                crgDataSetModifierRemoveAll( dataSetId );
                crgDataSetModifierSetInt( dataSetId, dCrgModGridNaNMode, dCrgGridNaNKeepLast );
                crgDataSetModifierSetDouble( dataSetId, dCrgModScaleCurvature, 0.5 );
                break;
                
            case 24:
                crgMsgPrint( dCrgMsgLevelNotice, "main: test purpose: curvature scaling\n" );
                crgDataSetModifierRemoveAll( dataSetId );
                crgDataSetModifierSetInt( dataSetId, dCrgModGridNaNMode, dCrgGridNaNKeepLast );
                crgDataSetModifierSetDouble( dataSetId, dCrgModScaleCurvature, 0.0 );
                break;
                
            case 25:
                crgMsgPrint( dCrgMsgLevelNotice, "main: test purpose: repositioning by u and v\n" );
                crgDataSetModifierRemoveAll( dataSetId );
                crgDataSetModifierSetInt( dataSetId, dCrgModGridNaNMode, dCrgGridNaNKeepLast );
                crgDataSetModifierSetDouble( dataSetId, dCrgModRefPointU,   5.0 );
                crgDataSetModifierSetDouble( dataSetId, dCrgModRefPointV,   1.5 );
                crgDataSetModifierSetDouble( dataSetId, dCrgModRefPointX, 100.0 );
                crgDataSetModifierSetDouble( dataSetId, dCrgModRefPointY, 200.0 );
                crgDataSetModifierSetDouble( dataSetId, dCrgModRefPointZ,  10.0 );
                /*
                crgContactPointOptionSetInt( cpId, dCrgCpOptionBorderModeU, dCrgBorderModeNone );
                crgContactPointOptionSetInt( cpId, dCrgCpOptionBorderModeV, dCrgBorderModeNone );
                */
                break;
                
            case 26:
                crgMsgPrint( dCrgMsgLevelNotice, "main: test purpose: repositioning by dx/dy/dz\n" );
                crgDataSetModifierRemoveAll( dataSetId );
                crgDataSetModifierSetInt( dataSetId, dCrgModGridNaNMode, dCrgGridNaNKeepLast );
                crgDataSetModifierSetDouble( dataSetId, dCrgModRefLineOffsetX,  100.0 );
                crgDataSetModifierSetDouble( dataSetId, dCrgModRefLineOffsetY,  200.0 );
                crgDataSetModifierSetDouble( dataSetId, dCrgModRefLineOffsetZ,   10.0 );
                break;
                
            case 27:
                crgMsgPrint( dCrgMsgLevelNotice, "main: test purpose: repositioning by dpi around rx,ry and translated by dx/dy/dz\n" );
                crgDataSetModifierRemoveAll( dataSetId );
                crgDataSetModifierSetInt( dataSetId, dCrgModGridNaNMode, dCrgGridNaNKeepLast );
                crgDataSetModifierSetDouble( dataSetId, dCrgModRefLineRotCenterX, 0.0 );
                crgDataSetModifierSetDouble( dataSetId, dCrgModRefLineRotCenterY, 0.0 );
                crgDataSetModifierSetDouble( dataSetId, dCrgModRefLineOffsetPhi, 1.57 );
                crgDataSetModifierSetDouble( dataSetId, dCrgModRefLineOffsetX,  100.0 );
                crgDataSetModifierSetDouble( dataSetId, dCrgModRefLineOffsetY,  100.0 );
                crgDataSetModifierSetDouble( dataSetId, dCrgModRefLineOffsetZ,  100.0 );
                break;
                
            case 28:
                crgMsgPrint( dCrgMsgLevelNotice, "main: test purpose: repositioning by dphi\n" );
                crgDataSetModifierRemoveAll( dataSetId );
                crgDataSetModifierSetInt( dataSetId, dCrgModGridNaNMode, dCrgGridNaNKeepLast );
                crgDataSetModifierSetDouble( dataSetId, dCrgModRefLineOffsetPhi, -0.6981317 );   /* 40 deg */
                crgContactPointOptionSetInt( cpId, dCrgCpOptionBorderModeU, dCrgBorderModeExZero );
                crgContactPointOptionSetInt( cpId, dCrgCpOptionBorderModeV, dCrgBorderModeExZero );
                break;

            default:
                done = 1;
                break;
        }
        
        if ( done )
        {
            crgMsgPrint( dCrgMsgLevelNotice, "main: all tests finished\n" );
            break;
        }
        
        /* --- apply the (default) modifiers --- */
        crgDataSetModifiersApply( dataSetId );
        
        /* --- get extents of data set --- */
        crgDataSetGetURange( dataSetId, &uMin, &uMax );
        crgDataSetGetVRange( dataSetId, &vMin, &vMax );
        
        /* --- calculate the test point spacing --- */
        du = ( uMax - uMin ) / nStepsU;
        dv = ( vMax - vMin ) / nStepsV;
        
        /* --- tweak the test boundaries, so that queries outside the original  --- */
        /* --- data area are forced and the boundary behaviour corresponding to --- */
        /* --- various options can be shown                                     --- */        
        
        uMax += nExcessStepsU * du;
        uMin -= nExcessStepsU * du;
        
        vMax += nExcessStepsV * dv;
        vMin -= nExcessStepsV * dv;

        /* --- print the options set for the contact point --- */
        crgContactPointOptionsPrint( cpId );
    
        /* --- run the actual evaluation routines --- */
        for ( i = 0; i <= nStepsU + 2 * nExcessStepsU; i++ )
        {
            u = uMin + du * i;
            
            for ( j = 0; j <= nStepsV + 2 * nExcessStepsV; j++ )
            {
                v = vMin + dv * j;
                
                /* --- check direct computation from u and v --- */
                if ( !crgEvaluv2z( cpId, u, v, &z ) )
                    crgMsgPrint( dCrgMsgLevelWarn, "main: error converting u/v = %+10.4f / %+10.4f to z.\n", u, v );
                else
                {
                    crgMsgPrint( dCrgMsgLevelNotice, "main: converted u/v = %+10.4f / %+10.4f ----> z = %+10.4f\n", u, v, z );
                    
                    /*
                    if ( writePlotFile && fPtr )
                        fprintf( fPtr, " %+10.4f  %+10.4f  %+10.4f\n", u, v, z );
                    */
                
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
                            if ( writePlotFile && fPtr )
                                fprintf( fPtr, " %+10.4f  %+10.4f  %+10.4f\n", x, y, z );
                            
                            /* any discrepancies? */
                            if ( fabs( z - zMem ) > 1.e-4 )
                            {
                                double uComp;
                                double vComp;
                                crgEvalxy2uv( cpId, x, y, &uComp, &vComp );
                                crgMsgPrint( dCrgMsgLevelNotice, "main: converted u/v = %3f / %.3f --> x/y = %+10.4f / %+10.4f --> u/v = %+.16f / %+.16f ----> z = %+10.4f\n",
                                                                  u, v, x, y, uComp, vComp, z );
                                crgMsgPrint( dCrgMsgLevelWarn, "main: inconsistent z values: %+10.4f vs. %+10.4f\n", zMem, z );
                            }
                        }
                    }
                }
            }
        }
        
        crgMsgPrint( dCrgMsgLevelNotice, "main: Finished test no. %d\n\n", testNo );
        
        if ( singleTest )
            done = 1;
        
        testNo++;
    }
    
    crgMsgPrint( dCrgMsgLevelNotice, "main: releasing data set\n" );
    
    crgDataSetRelease( dataSetId );
    
    crgMsgPrint( dCrgMsgLevelNotice, "main: normal termination\n" );
    
    return 1;
}

