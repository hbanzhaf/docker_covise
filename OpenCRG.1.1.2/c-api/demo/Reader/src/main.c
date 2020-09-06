/* ===================================================
 *  file:       main.c
 * ---------------------------------------------------
 *  purpose:    main program for CRG reader
 * ---------------------------------------------------
 *  first edit: 02.11.2008 by M. Dupuis
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
#include "crgBaseLib.h"


void usage()
{
    crgMsgPrint( dCrgMsgLevelNotice, "usage: crgReader [options] <filename>\n" );
    crgMsgPrint( dCrgMsgLevelNotice, "       options: -h    show this info\n" );
    crgMsgPrint( dCrgMsgLevelNotice, "       <filename> use indicated file as input file\n" );
    exit( -1 );
}

int main( int argc, char** argv ) 
{
    char* filename = "";
    int dataSetId = 0;
    
    /* --- decode the command line --- */
    if ( argc < 2 )
        usage();
    
    argc--;
    
    while( argc )
    {
        argc--;
        argv++;
        
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
    crgMsgSetLevel( dCrgMsgLevelDebug );
    
    if ( ( dataSetId = crgLoaderReadFile( filename ) ) > 0 )
    {
        crgDataPrintHeader( dataSetId );
        crgDataPrintChannelInfo( dataSetId );
        crgDataPrintRoadInfo( dataSetId );
    }

    /* --- check CRG data for consistency and accuracy --- */
    if ( !crgCheck( dataSetId ) )
    {
        crgMsgPrint ( dCrgMsgLevelFatal, "main: could not validate crg data. \n" );
        return -1;
    }
    
    crgMsgPrint( dCrgMsgLevelNotice, "main: normal termination\n" );
    
    return 1;
}

