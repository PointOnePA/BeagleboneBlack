This is a test build for the Beaglebone Black, using
the linaro cross compiler.  The build can be done from
the command line, or Eclipse with support for remote debugging.
This test project is also used for GIT training using Bitbucket
and Atlasian's SourceTree.


Show the result of the Makefile without executing.
C:\Gitrepo\demo1>make -n all
echo .
echo ---COMPILE---  /c/Gitrepo/demo1/src/demo01.c
"C:\gcc-linaro\bin\arm-linux-gnueabihf-gcc.exe" -c -o c:/Gitrepo/demo1/object/demo01.o c:/Gitrepo/demo1/src/demo01.c -marm -O0  -g  -I. -Ic:/Gitrepo/demo1/include
echo .
echo ---COMPILE---  /c/Gitrepo/demo1/src/lookup.c
"C:\gcc-linaro\bin\arm-linux-gnueabihf-gcc.exe" -c -o c:/Gitrepo/demo1/object/lookup.o c:/Gitrepo/demo1/src/lookup.c -marm -O0  -g  -I. -Ic:/Gitrepo/demo1/include
echo .
echo ---COMPILE---  /c/Gitrepo/demo1/src/thermistor.c
"C:\gcc-linaro\bin\arm-linux-gnueabihf-gcc.exe" -c -o c:/Gitrepo/demo1/object/thermistor.o c:/Gitrepo/demo1/src/thermistor.c -marm -O0  -g  -I. -Ic:/Gitrepo/demo1/include
echo .
echo ---LINK---
"C:\gcc-linaro\bin\arm-linux-gnueabihf-gcc.exe" -o demo01 c:/Gitrepo/demo1/object/demo01.o c:/Gitrepo/demo1/object/lookup.o c:/Gitrepo/demo1/object/thermistor.o -marm -O0  -g  -I. -Ic:/Gitrepo/demo1/include
echo .
echo ---SUCCESS---  demo01
