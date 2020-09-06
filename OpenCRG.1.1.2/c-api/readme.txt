#
#    Copyright 2017 VIRES Simulationstechnologie GmbH
#
#    Licensed under the Apache License, Version 2.0 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at
#
#        http://www.apache.org/licenses/LICENSE-2.0
#
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#    See the License for the specific language governing permissions and
#    limitations under the License.
#

Version Info
--------------------------------------------------------------
This readme file was updated on January 17, 2017 by
VIRES Simulationstechnologie GmbH


About this readme file
--------------------------------------------------------------
This readme file is a complementary file to the readme file
provided in the OpenCRG root directory. Please read the root
directory's file first. 


Overview
--------------------------------------------------------------
The C-API provides a tool suite for reading, modifying and
evaluating OpenCRG data files. It is based on plain ANSI-C code
and should therefore allow for the compilation on all relevant 
platforms without modification of the source code.


Directory structure:
--------------------------------------------------------------
|----baselib................OpenCRG basic library - the core of the toolset
|    |----lib...............location of the compiled OpenCRG library
|    |----inc...............include files providing the interface to the library 
|    |----makefile..........sample makefile for users preferring the make mechanism
|    |----obj...............target directory for sources compiled with the make mechanism
|    |----src...............the library's sources
|----compileScript.sh.......script for the compilation of all demos and tools, 
|                           based on simple compiler calls; this is an alternative to
|                           using the make mechanism; all files of the base library are
|                           also compiled with this script, so there is no need for a
|                           separate compilation of the library files.
|----demo...................demo sources showing the usage of the basic library
|    |----Simple............a really simple application covering all basics of the API,
|    |                      runs with fix data sample "handmade_straight.crg"
|    |----EvalOptions.......a set of routines demonstrating the usage of various options
|    |----EvalXYnUV.........a set of routines for the evaluation of OpenCRG reference lines
|    |----EvalZ.............an advanced example for the evaluation of OpenCRG data
|    |----Reader............a sample application for a CRG file reader
|    |----bin
|    |    |----crgSimple....executable of the very simple example
|    |    |----crgEvalxyuv..executable of the reference line evaluator
|    |    |----crgReader....executable of the sample reader
|    |    |----crgEvalz.....executable of the complex z data evaluator
|    |    |----crgEvalOpts..executable of the option usage example
|    |----makefile..........makefile for all demos (alternative to "compileScript.sh")
|----makefile
|----readme.txt
|----test
|    |----Dump....................reads an OpenCRG file and dumps the values x/y/z/u/v into
|    |                            into a text file "crgDump.txt" - very helpful for debugging
|    |----MemTest.................just a quick test for allocating and releasing CRG data sets
|    |----MultiCp.................test with multiple contact points
|    |----MultiRead...............read multiple data files, evaluate on last file
|    |----PerfTest................test tool for evaluating the performance of the library
|    |----Scan....................perform an x/y-scan of arbitrary CRG data set
|    |----Verify..................test tool for verifying the c-api algorithms; reads an
|    |                            OpenCRG file AND an x/y/z or x/y/z/u/v reference text
|    |                            file containing test points. This will compute the z value
|    |                            at the given x/y locations from the OpenCRG file and then
|    |                            compare the result with the given z reference value
|    |----bin
|    |    |----testModifiers.sh...script for performing a series of tests using the
|    |    |                       modifier mechanisms; requires gnuplot
|    |    |----testOptions.sh.....script for performing a series of tests using the
|    |    |                       evaluation option mechanisms; requires gnuplot
|    |    |----crgPerfTest........performance test tool; may not run on all platforms
|    |----makefile................makefile for all tests (alternative to "compileScript.sh")


Compiling:
--------------------------------------------------------------
Three methods for compiling the tool-set are provided:

A) On machines with gcc and standard make environment, just type 

      make

   in the root directory. This should result in a series of executable files
   in the directories 

      demo/bin/
      test/bin/
      
   In addition, a library containing all object files of the baselib/ sources
   is created in
   
      baselib/lib

B) On machines having trouble with the provided makefiles, either adapt those
   files or use the very basic fallback solution which is a compile script. 
   The script "compileScript.sh" is located in the root directory

      c-api/

   Open the script, set the compiler variable "COMP" to the name of your compiler,
   re-save the script and execute it. The results should - again - be found in

      demo/bin/
      test/bin/
      
   In contrast to the makefile mechanism, no library is explicitly created 
   from the baselib/ files.

C) If you don't like makefiles and our scripts, you may just write your own
   simple compile instruction at command line level. 
   
   For this purpose, please note the following hints:
   
   1) In order to compile a demo, set your include file search path to
        baselib/inc
   2) Always compile in combination with all .c-files in
        baselib/src
   
   Example: For the compilation of EvalXYnUV, use:
   
            cc -lm -o EvalXYnUV -I baselib/inc demo/EvalXYnUV/src/main.c baselib/src/*.c

        
Release Notes:
--------------------------------------------------------------

March 30, 2018 Release 1.1.2
-----------------------------
- C-API:
  fixes multiplatform issues with files bigger than 2GB, WIN64 now uses stat64 while WIN32 build are still possible

January 17, 2017 Release 1.1.1
-----------------------------
- C-API:
  fixes to handle crg input files bigger than 2GB

October 26, 2015 Release 1.1.0 - Stable
---------------------------------------
- C-API:
  fix memory corruption (from ver. 1.0.7 RC2)

October 15, 2015 Release 1.1.0 - Stable
---------------------------------------
- fix warning

April 03, 2015 Release 1.0.7 - RC2
------------------------------------
- handle crg input files bigger than 2GB
- feature #3079: add public access to closed track utility data (uIsClosed, uCloseMin, uCloseMax)
- bug #1210: reuse deleted contact point positions

October 31, 2014 Release 1.0.7 - RC1
------------------------------------
- corrected check routines for modifiers (determination of byref and byoff)
- added message if modifiers are changed implicitly by crgCheck()

April 08, 2014 Release 1.0.6:
-------------------------------
-  add new modifier to define a rotation center (refline by offset)
-  revert strchr by memchr fix of Release 1.0.5 - RC2
-  implement MATLAB-API equivalent check routines for
   * options
   * modifier

July 26, 2013 Release 1.0.6 - RC1
---------------------------
-  revert to c89/90 standard
-  set optimization level 3 as default
-  bugfix: in some cases the history may be currupt
-  rename: mBigEndian -> mCrgBigEndian
-  add crg check data routine for consistency and accuracy
   * validate option if track can be closed

April 12, 2013, Release 1.0.5 - RC3:
--------------------------------------
- bug-fix: Contact point table will be delete only if all contact points are removed

January 04. 2012, Release 1.0.5 - RC1:
--------------------------------------
- C-API:
  fix non-critic memory leak( create contact point )
    * free pre initialized option entry before setting default data set options.
  fix warnings
    * replaced strchr by memchr with fixed size ( reading char* )

December 19. 2011, Release 1.0.4:
--------------------------------------
- bug-fix: reading data format

June 01, 2011, Release 1.0.3 - RC1:
--------------------------------------
- New policy:
  extrapolation considers now banking (uv->z)
  border mode 'set zero' overwrites smoothing and banking (uv->z)
  border mode 'keep last' overwrites smoothing and banking (uv->z)
- bug-fix: comment behind data format token is no longer considered
- update test/Verify                  : add 'v out of core area' comment

August 21, 2010, Release 1.0.2:
--------------------------------------
- reduced warning messages for unhandled options
- improved compilation on 64bit systems (less compiler warnings)
- debugged loader for data formad LDFI, increased loading performance
- ignoring "reference_line_offset_xxx" in file header (was introduced
  for Matlab routines only in 1.0.2)
- debugged callback for "check_tol" in crgLoader.c

June 04, 2010, Release 1.0.1:
--------------------------------------
- debugged reading of files containing "CRLF" line ends

April 14, 2010, Release 1.0.0:
--------------------------------------
- slightly modified printing of options in c-api
- for complete documentation of differences between 0.8 and 1.0, 
  please review all release notes concerning the release candidates
  for release 1.0.0 (i.e. Feb 10 until April 12)

April 12, 2010, Release 1.0.0, rc7:
--------------------------------------
- added methods for providing user-defined memory management methods
  (see crgPortability.c)

March 30, 2010, Release 1.0.0, rc6:
--------------------------------------
- harmonized default modifiers with Matlab routines
- adjusted test routines "crgEvalOpts"

March 26, 2010, Release 1.0.0, rc5:
--------------------------------------
- increased compatibility with older MS compilers (crgPortability.c)
- loader recognizes <cr> in all areas of ASCII files
- first occurence of modifier block in data file clears default modifiers
- option / modifier print improved to distinguish between the two types
- harmonized closed-reference line algorithm with Matlab tool-set

March 08, 2010, Release 1.0.0, rc4:
--------------------------------------
- debugged modifier "grid_nan_offset"
- debugged calculation of centerline phi value
- new (primitive) test tool "crgScan" which scans data in x/y direction and 
  writes output file with x/y/z data (like crgDump)

February 26, 2010, Release 1.0.0, rc3:
--------------------------------------
- corrected interpretation zBeg and zEnd if no slope is given
- corrected computation of direction angle (no longer interpolated)
- corrected various issues concerning transformations (via modifiers)
- checked / debugged repeat and reflect options in combination with
  smoothing, bank and slope

February 16, 2010, Release 1.0.0, rc2:
--------------------------------------
- corrected interpretation of slope values
- corrected handling of reference line elevation outside core area
- corrected weighted integration of phi values

February 10, 2010, Release 1.0.0, rc1:
--------------------------------------
- corrected example "handmade_circle.crg" so that phi stays within [-PI;PI]
- corrected closed reference line calculation
- increased performance of z evaluation routines

December 17, 2009, Release 0.8.0:
---------------------------------
- reduced memory footprint by using float data type for storage of z data
- increased performance of z evaluation routines

December 04, 2009, Release 0.7.2:
---------------------------------
- debugged computation of remaining file size after reading header information

November 19, 2009, Release 0.7.1:
---------------------------------
- introduced new test routines 
    c-api/test/MultiRead
  here, a file is being read multiple times (once for each contact point)
- debugged file loading via reference file

November 12, 2009, Release 0.7:
-------------------------------
- corrected default mode for NaNs to "keep last"
- (default) modifiers must now be applied explicitly after loading a file;
  check the demo files for the corresponding examples
- corrected algorithm for smoothing zone at begin/end of data; behavior
  depends on the selected u border mode; if border mode is "set to 0" then
  smoothing will be carried out between 0 and the actual z value; if border
  mode is set to "keep last", then smoothing will be carried out between the
  z mean value (including reference line elevation) at the corresponding end
  of the data set and the actual z value
- corrected algorithm for computation of elevation on closed tracks
- new debug tool "crgDump" which reads a file and dumps the data contents into
  an ASCII file "crgDump.txt"; each line of the file represents a triplet which
  is either (u,v,z) or (x,y,z), depending on the options that have been set;
  the file may be used e.g. for gnuplot to visualize the contents of an OpenCRG
  file
    usage: crgDump [options] <filename>
           options: -h    show this info
                    -u    plot u/v->z (default: plot x/y->z)
           <filename> OpenCRG input file

- added release query method:
    crgGetReleaseInfo()
  This method returns a string with the relevant information about the release
- added new debug tool: crgVerify
  This tool may be used for verifying z values at given x/y positions; The
  x/y/z triplets have to be stored in an ASCII file (triplet per row, separated
  by whitespace character). The tool will read the ASCII file and another user-
  defined OpenCRG file, evaluate the OpenCRG file at the respective x/y positions
  and compare the result to the z value given in the ASCII file. If the 
  deviation is above a user-defined tolerance level, a warning will be printed.
  At the end, the tool will provide some statistics data about the test results.
    usage: crgVerify [options] <CRG file> <XYZ file>
           options: -h      show this info
                    -t <n>  tolerance value <n> for z comparison [m]
           <CRG file> CRG file from which to compute the z data
           <XYZ file> file containing the x/y/z positions for verification
- users may replace the message printing mechanisms with their own methods; 
  a user-defined callback may be registered which is then called for each
  message instead of printing it to stderr; criticality levels are still
  checked before invoking the callback, so if a user wants to perform also
  full level checking, the notification level should be set to "debug" beforehand;
  an example of the callback definition and usages is given in the main.c of
  crgEvalOpts (source: demo/EvalOptions/src/main.c); the callback is registered
  with "crgMsgSetCallback( ... )"
                                      
October 01, 2009, Release 0.7-rc1:
----------------------------------
- added memory allocation / release test
- debugged memory management
- fixed major bug in computation of data set ID as being returned by the crgLoader
- added API method for releasing all memory used by the C-API:
    crgMemRelease()
- added API method for querying u and v increments of the data set:
    crgDataSetGetIncrements(...)
- history is now (again) part of contactPoint management, not of crgDataSet; by this,
  some methods concerning the history were shifted from crgData domain to 
  crgContactPoint domain (all of them were part of the private baselib and are,
  therefore, not described in detail)
- fixed bug in preloading history with a uFrac value
- fixed bug in the reflect mode of the z evaluation routines
- activated header option "WARN_MSGS" which allows for the control of the number of
  messages that are to be printed to shell;
  valid values:
    0  = print no messages at all
   -1  = print unlimited number of messages
    n  = print the first n messages

August 15, 2009, Release 0.6beta:
--------------------------------
- new directory structure for better handling of an entire OpenCRG release
- bugfixed data set id for application of default modifiers and options (in method crgDataSetCreate())
- increased robustness of conditional queries for highly optimized code generation


May. 25, 2009, Release 0.4beta:
-------------------------------
- introduced channels for slope and bank
- evaluating slope and bank
- introduced maximum relative error for channel data as symbolic constant dCrgEps
- introduced new evaluation options:
    dCrgCpOptionBorderModeU         [integer], extrapolation / repeat mode for z values in u direction
        - dCrgBorderModeReflect
    dCrgCpOptionBorderModeV         [integer], extrapolation / repeat mode for z values in v direction
        - dCrgBorderModeReflect
    dCrgCpOptionBorderOffsetU       [double],  offset of z values when exceeding core area in u direction
    dCrgCpOptionBorderOffsetV       [double],  offset of z values when exceeding core area in v direction
    dCrgCpOptionSmoothUBegin        [double],  smoothly blend the data on-set over the given range [m]
                                               at the begin of the data set
    dCrgCpOptionSmoothUEnd          [double],  smoothly blend the data off-set over the given range [m]
                                               at the end of the data set
- introduced new option / modifier handling class "crgOptions.c" for the handling of options per
  contact point and modifiers per data set
- introduced routines for setting and printing data modifiers
  associated with a data set. Modifiers may be of integer or
  double data types
      crgDataSetModifierSet[Int|Double]( ... )
      crgDataSetModifierGet[Int|Double]( ... )
      crgDataSetModifierRemove( ... )
      crgDataSetModifierRemoveAll( ... )
      crgDataSetModifierPrint( ... )
      crgContactPointSetDefaultOptions()
- distinguishing levels during include operations, so that options and modifiers defined at
  top level will always be preferred to options and modifiers defined at lower levels
- routines which might be subject to portability issues (e.g. for an integration into 
  FORTRAN environments) are now located in crgPortability.c
- first release of comprehensive documentation as .pdf file



Feb. 10, 2009, Release 0.3beta:
-------------------------------
- harmonized command line options of sample tools:
    crgEvalOpts [-h] <filename>
    crgEvalz [-h] <filename>
    crgEvalxyuv [-h] <filename>
    crgReader [-h] <filename>
    crgSimple [-h] <filename>
- improved handling of NaNs in ASCII files
- introduced separate channels for v and z data
- removed routine for setting endian format from public interface 
  (endianness check is done automatically in the loader)
- introduced routines for setting and printing evaluation options
  associated with a contact point. Options may be of integer or
  double data types
      crgContactPointOptionSet[Int|Double]( ... )
      crgContactPointOptionGet[Int|Double]( ... )
      crgContactPointOptionRemove( ... )
      crgContactPointOptionRemoveAll( ... )
      crgContactPointOptionsPrint( ... )
      crgContactPointSetDefaultOptions()
- The following option types with respective value ranges are available:
    dCrgCpOptionBorderModeU             /* [integer], extrapolation / repeat mode for z values in u direction */
    dCrgCpOptionBorderModeV             /* [integer], extrapolation / repeat mode for z values in v direction */
        dCrgBorderModeNone              /* refuse computation, i.e. return error         */
        dCrgBorderModeExKeep            /* keep last value on border of data set         */
        dCrgBorderModeExLinear          /* linear extrapolation                          */
        dCrgBorderModeExZero            /* set z value to 0                              */
        dCrgBorderModeRepeatStandard    /* repeat the data set and perform interpolation */

    dCrgCpOptionCurvMode                /* [integer], curvature calculation mode aside the reference line     */
        dCrgCurvLateral                 /* compute curvature based on lateral position (v)  */
        dCrgCurvRefLine                 /* keep curvature value on reference line           */

    dCrgCpOptionScaleZ                  /* [double],  scale factor for resulting z values */
        numeric value of applicable scale factor

- added module for testing option settings; 
      new directory EvalOptions
      new binary    crgEvalOpts
- adapted makefiles and scripts for additional module


****** known bugs and/or deficiencies ********
- the samples "handmade_sloped.crg" and "handmade_banked.crg" are not yet handled correctly. To be fixed in 0.4beta.


Dec. 17, 2008, Release 0.2beta:
-------------------------------
- crgClearData() is now called crgDataSetRelease()
- crgGetDataSet() is now called crgDataSetAccess()
- added very simple example in directory Tools/Simple
- debugged "static" modifier in crgLoader.c at method "getNextRecord()"
- added compile scripts in order to overcome makefile conflicts on various platforms
- debugged reading of binary data on big endian machines


