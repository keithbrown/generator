========================================================================

File:      $RCSfile: README.txt,v $
Version:   $Revision: 1.22 $
Modified:  $Date: 2013/03/14 05:36:48 $

(c) Copyright 1992-2013 by Mentor Graphics Corp. All rights reserved.

========================================================================
This document contains information proprietary and confidential to
Mentor Graphics Corp., and is not for external distribution.
========================================================================

Generator Procedures Document
How to Build and Configure Generator

1  Introduction
----------------
This file contains instructions on how to compile and configure generator
(gen_erate.exe).  It is designed to be used by persons wanting to compile
on either on the build server or their local development machine.


2  Prerequisites
-----------------
2.1  Machine setup
  The build server is already configured to build generator.  The following
  steps only apply to new development machine setup.
  
2.1.1  Applications
  The following applications are required to exist and be properly
  configured for development use.
        . Cygwin
        . Microsoft Visual C++ 6

2.1.2  Libraries
  The following libraries are required to exist.  In order to simplify
  the process as much as possible, we highly recommend they be placed
  under C:/lib
    . GalaxyGC30       - the Galaxy library
    . PCCTS/v1.33_mr20 - the PCCTS library
    . mgls             - the Mentor Graphics License Server library

  If you desire to place the libraries somewhere other than C:/lib, you
  will have to modify bp_prod/win32/7.1B/build_scripts/get_host_platform.inc
  starting at line 160.  You will also need to adjust step 3.3, below,
  accordingly.

  
3 Configure the workspace
--------------------------
3.1  On the build server, open the workspace c:/workspaces/Generator_git with
     BridgePoint.
  . Update the source code for the projects to the latest from master 
  . No additional configuration is necessary, the workspace build files are 
    already set up for this machine.
      
3.2  If you're doing a local build
  . Check out (or sync with master) <github>/xtuml/mc
  . Check out (or sync with master) <github>/xtuml/generator
  . Check out (or sync with master) <github>/xtuml/internal
  
  Setup up for the Build
  _- Navigate to <git root>/xtuml/generator/gen_erate
  _- Edit the file "msvc6.bat"
       Replace "set HOME=/home/build"
          with "set HOME=/home/<your username>"
  _- cd <git root>/generator/gen_erate
  _- Edit the file "create_bp_build"
      . In libtrans_dir variable, replace "/git/xtuml/internal/src/libTRANS/"
           with "/cygdrive/c/<git repo root dir>/xtuml/internal/src/libTRANS/"
  _- cd bp_prod/win32/7.1B/build_scripts
  _- Edit the file "get_host_platform.inc"
      . Replace "/c/cygwin/git/xtuml/generator/"
           with "/cygdrive/c/<git repo root dir>/xtuml/generator/"
      . Replace "c:/cygwin/git/xtuml/generator/"
           with "c:/<git repo root dir>/xtuml/generator/"
      . Replace CC_DIR=C:/PROGRA~1/MICROS~1/VC98/BIN
           with CC_DIR=<"short-style" path to MS VC6 bin folder>
  _- Note: The following build steps can be repeated after modifying source or
           archetypes during development (to rebuild).  The
           build_{release|debug} scripts can be used at this point instead of
           these manual steps (if desired).
   
  
4  Compiling the Generator EXEs
--------------------------------
4.1  Open cygwin shell
  _- Navigate to <git repo root dir>/xtuml/generator/src/gen_erate
  _- ./msvc6.bat
     Warning: This causes the current directory to change.

4.2  Update the generator version number.
  _- Increment the current version number on line 77 of
     .../gen_erate/bp_source/win32/7.1B/main/src/gen_erate/main_gen_erate.cc
     while leaving the revision tag.
  _- Check in the file to git and put the change to github 

4.3  Build
  _- cd <git repo root dir>/xtuml/generator/src/gen_erate
  _- rm -rf bp_build
  _- ./create_bp_build -p C:/lib/PCCTS/v1.33_mr20 -m C:/lib/mgls
  _- cd bp_prod/win32/7.1B/build_scripts
  _- dos2unix * 

4.3.1 Release build
  _- ./comp_private win32 7.1B compile
    NOTE: If the build fails, see the troubleshooting section below.
  _- ./prod_private win32 7.1B mc_dbms trans gen_erate

4.3.2 Debug build
  -- This is for development purposes only.  You normally would skip building
  -- the debug version.
  _- ./comp_private win32 7.1B debug
  _- ./prod_private win32 7.1B debug mc_dbms trans gen_erate
     
4.5  Verify complete
  _- ls -l ../../../../bp_build/win32/7.1B/build/*.exe
  _R gen_erate.exe should exist

4.6 Make gen_erate LARGEADDRESSAWARE  
  _- cd ../../../../bp_build/win32/7.1B/build/
  _- editbin.exe /LARGEADDRESSAWARE gen_erate.exe
  _- dumpbin.exe /headers gen_erate.exe | grep "Application can handle large (>2GB) addresses"
  _R The string IS present in the generate binary.
  _R Note: This is simply a second chance to check that the gen_erate binary 
     is properly modified by editbin.exe.  Repeat that dumpbin.exe line and 
     ASSURE that the string being grepped IS found.
     

5  Using the EXEs
------------------
5.1  Backup current version and copy the new one into place
5.1.1 If you built on the server, get the gen_erate.exe from mentorweb and put 
      it in c:\temp
5.1.2 Update DAP plugin
  _- cd C:\MentorGraphics\BridgePoint\eclipse_extensions\Bridgepoint\eclipse\
          plugins\com.mentor.nucleus.bp.dap.pkg_<version>\
          bridgepoint\win32\client\bin
  _- mv gen_erate.exe gen_erate.exe.orig
  . if you built on the server
    _- cp c:/temp/gen_erate.exe .
  . if you built locally
    _- cp <git repo root dir>/xtuml/generator/src/gen_erate/bp_build/win32
           /7.1B/build/gen_erate.exe .
  _- cp gen_erate.exe gen_file.exe
  _- cp gen_erate.exe gen_import.exe
5.1.3 Update MC plugin
  _- cd C:\MentorGraphics\BridgePoint\eclipse_extensions\Bridgepoint\eclipse\
          plugins\com.mentor.nucleus.bp.mc.mc3020_<version>\mc3020\bin        
  _- mv gen_erate.exe gen_erate.exe.orig
  . if you built on the server
    _- cp c:/temp/gen_erate.exe .
  . if you built locally
    _- cp <git repo root dir>/xtuml/generator/src/gen_erate/bp_build/win32
           /7.1B/build/gen_erate.exe .
5.1.4 Move the new gen_erate so it is picked up by the build server 
  _- Perform smoke tests to verify the generator
     works properly.
  _- Remote desktop to svr-azt-eng-03 (if not already there)
  _- Start BridgePoint on the workspace c:/workspaces/build_artifacts
  _- Switch to the Resource perspective
  _- Expand the extra_files_for_build project
  _- Copy the new gen_erate.exe into this folder
  _- Commit the change to SVN


6. Using MS Visual Studio as a class browser
--------------------------------------------
6.1 The following files are present to allow the generator classes to be viewed
    using the MS Visual Studio IDE (MS VC6).
    <CVS>model_compilers/generator/gen_erate/gen_erate.dsw
    <CVS>model_compilers/generator/gen_erate/gen_erate.dsp

    This project's purpose is to provide a class browser for the generator, it
    can also aide in debugging (by attaching to the running process).  A MS VC6
    project was selected, because that is what generator is built with.
    However, note that these MS project files can not currently be used to build
    gen_erate.  It shouldn't be hard to extend the project files to do this
    if needed.

    -The Microsoft IDE does not understand how to interpret ".cc" and ".hh"
     files by default.  The registry must be hacked to force DevStudio to
     interpret these files properly.  The instructions in the MS knowledgebase
     article "How to make Visual C++ recognize file extensions as C/C++ files"
     must be followed.  Here is the link to that article:
     http://support.microsoft.com/kb/181506
     
7. Troubleshooting Build Problems
---------------------------------
7.1  The build steps outlined above will work nearly all the time for the build
     run on the build server svr-azt-eng-01.  Occasionally, however, due to 
     running the build on a different machine, or an upgrade to cygwin, etc...
     the build will not function correctly.  Here are some tips for debugging
     problems.
     
7.1.1  Make sure all variables and paths to be used are correctly specified in
       msvc6.bat and get_host_platform.inc
7.1.1.1  Use "dir /x" on windows to get paths with "~<n>" in them

7.1.2  Errors output about incorrect usage of "set"
7.1.2.1  Possible due to /bin/sh (specified in the script) not pointing to the
         necessary command shell which is bash.  Check the system configuration.
7.1.2.2  Shell scripts not in the proper format.  Run dos2unix on the scripts,
         and also perhaps the source code and makefiles.
           $ cd model_compilers/generator/gen_erate/bp_prod
           $ find . -exec dos2unix {} \;
           
7.1.3  To debug makefile problems, add "--debug=bj" on line 172 of comp_private
       where make is invoked.
       
7.1.4  If make outputs the error "invalid option <val>", this is likely due to 
       an issue with the command line specified on line 173 of comp_private.
7.1.4.1  Past resolutions for this issue have been to reorder the command line
         or set the variable "MAKEFLAGS=--unix".  Another recent solution to
         this problem was to add and "export BUILD_INCLUDE" line to export the
         environment variable instead of passing it on the make command line.
