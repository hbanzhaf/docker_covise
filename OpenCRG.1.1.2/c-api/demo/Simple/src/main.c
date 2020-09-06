/* ===================================================
 *  file:       main.c
 * ---------------------------------------------------
 *  purpose:    main program for CRG test program
 *              calculating z values from given
 *              x/y co-ordinates;
 *              very simple but effective
 * ---------------------------------------------------
 *  first edit: 11.12.2008 by M. Dupuis
 *  last mod.:  03.04.2015 by H. Helmich @ VIRES GmbH
 * ===================================================
    Copyright 2015 VIRES Simulationstechnologie GmbH

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
    crgMsgPrint( dCrgMsgLevelNotice, "usage: crgSimple [options] <filename>\n" );
    crgMsgPrint( dCrgMsgLevelNotice, "       options: -h    show this info\n" );
    crgMsgPrint( dCrgMsgLevelNotice, "       <filename>  input file, default: [%s]\n", "../../Data/handmade_straight.crg" );
    exit( -1 );
}

int main( int argc, char** argv ) 
{
    char*  filename = "../../../crg-txt/handmade_straight.crg";
    int    dataSetId = 0;
    int    cpId;
    double x;
    double y;
    double z;
    
    /* --- decode the command line --- */
    if ( argc > 2 )
        usage();
    
    if ( argc == 2 )
    {
        argv++;
        argc--;
        
        if ( !strcmp( *argv, "-h" ) )
            usage();
        else
            filename = *argv;
    }
    
    /* --- now load the file --- */
    if ( ( dataSetId = crgLoaderReadFile( filename ) ) <= 0 )
    {
        crgMsgPrint( dCrgMsgLevelFatal, "main: error reading data file <%s>.\n", filename );
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
    
    /* --- create a contact point for evaluation of the data set --- */
    if ( ( cpId = crgContactPointCreate( dataSetId ) ) < 0 )
    {
        crgMsgPrint( dCrgMsgLevelFatal, "main: could not create contact point.\n" );
        return -1;
    }
    
    /* --- run a series of evaluations at a mesh of x and y positions --- */
    crgMsgPrint( dCrgMsgLevelNotice, "main: performing tests....\n" );
    
    for ( x = 0.0; x < 22.0; x += 2.0 )
    {
        for ( y = -1.50; y <= 1.50; y+= 0.50 )
        {
            if ( !crgEvalxy2z( cpId, x, y, &z ) )
                crgMsgPrint( dCrgMsgLevelWarn, "main: error converting x/y = %.3f / %.3f to z.\n",  x, y );
            else
                crgMsgPrint( dCrgMsgLevelNotice, "main: converted x/y = %+10.4f / %+10.4f ----> z = %+10.4f\n", x, y, z );
        }
    }

    crgMsgPrint( dCrgMsgLevelNotice, "main: releasing data set\n" );

    crgDataSetRelease( dataSetId );

    /* --- release remaining data set list --- */
    crgMemRelease();

    crgMsgPrint( dCrgMsgLevelNotice, "main: normal termination\n" );
    
    return 1;
}

