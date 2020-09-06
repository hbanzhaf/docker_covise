#!/bin/csh
#    Copyright 2008-2015 VIRES Simulationstechnologie GmbH
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
# This script compiles all samples avoiding any "make" mechanism
#
# set your compiler here
set COMP = 'gcc -O3'

# compile all demos

echo -n compiling crgEvalxyuv...
$COMP -o demo/bin/crgEvalxyuv -I baselib/inc demo/EvalXYnUV/src/main.c   baselib/src/*.c -lm
echo done

echo -n compiling crgEvalOpts...
$COMP -o demo/bin/crgEvalOpts -I baselib/inc demo/EvalOptions/src/main.c baselib/src/*.c -lm
echo done

echo -n compiling crgEvalz...
$COMP -o demo/bin/crgEvalz    -I baselib/inc demo/EvalZ/src/main.c       baselib/src/*.c -lm
echo done 

echo -n compiling crgReader...
$COMP -o demo/bin/crgReader   -I baselib/inc demo/Reader/src/main.c      baselib/src/*.c -lm 
echo done

echo -n compiling crgSimple...
$COMP -o demo/bin/crgSimple   -I baselib/inc demo/Simple/src/main.c      baselib/src/*.c -lm 
echo done


# compile all tests

echo -n compiling crgPerfTest...
$COMP  -o test/bin/crgPerfTest -I baselib/inc test/PerfTest/src/main.c baselib/src/*.c -lm 
#$COMP -m32 -O2 -Wall -fomit-frame-pointer -fno-strict-aliasing -fPIC -o test/bin/crgPerfTest -I baselib/inc test/PerfTest/src/main.c baselib/src/*.c -lm 
echo done

echo -n compiling crgDump...
$COMP -o test/bin/crgDump -I baselib/inc test/Dump/src/main.c baselib/src/*.c -lm 
echo done

echo -n compiling crgMemTest...
$COMP -o test/bin/crgMemTest -I baselib/inc test/MemTest/src/main.c baselib/src/*.c -lm 
echo done

echo -n compiling crgVerify...
$COMP -o test/bin/crgVerify -I baselib/inc test/Verify/src/main.c baselib/src/*.c -lm 
echo done

echo -n compiling crScan...
$COMP -o test/bin/crgScan -I baselib/inc test/Scan/src/main.c baselib/src/*.c -lm 
echo done
