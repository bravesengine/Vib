
@echo off

set VC2017PROBAT="C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\VC\Auxiliary\Build\vcvars64.bat"
set VC2017COMBAT="C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat"
set VC2019PROBAT="C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build\vcvars64.bat"
set VC2019COMBAT="C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"
set VCBAT=""


IF EXIST %VC2019PROBAT% (
  ECHO VS2019 Pro
  set VCBAT=%VC2019PROBAT%
  set VCCMAKEGENERATOR="Visual Studio 16 2019"
  goto vcfound
)

IF EXIST %VC2019COMBAT% (
  ECHO VS2019 Community
  set VCBAT=%VC2019COMBAT%
  set VCCMAKEGENERATOR="Visual Studio 16 2019"
  goto vcfound
)

IF EXIST %VC2017PROBAT% (
  ECHO VS2017 Pro
  set VCBAT=%VC2017PROBAT%
  set VCCMAKEGENERATOR="Visual Studio 15 2017"
  goto vcfound
)

IF EXIST %VC2017COMBAT% (
  ECHO VS2017 Community
  set VCBAT=%VC2017COMBAT%
  set VCCMAKEGENERATOR="Visual Studio 15 2017"
  goto vcfound
)


echo Can't find VS version

goto exit:

:vcfound

call %VCBAT%

:exit
