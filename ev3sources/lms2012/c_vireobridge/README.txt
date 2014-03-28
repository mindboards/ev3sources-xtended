Building libvireobridge.so requires some files that are not distributed with
this module:

    1. Vireo header files.  libvireobridge.so source files require Vireo
       headers to compile, so you need to have those files and specify their
       location in the Makefile by setting the INCLUDEVIREO variable.

    2. A libvireo.so library.  libvireobridge.so links against libvireo.so, so
       you must have libvireo.so (built for the appropriate target) in the
       Linuxlib_AM1808/ directory.

