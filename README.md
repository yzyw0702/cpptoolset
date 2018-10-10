# cpptoolset
simple-prelim and file-independent tool kits for operations of string, path, memory / cpu / disk info, matrix, date / time, video, roi. in c++

## test hint
> * on linux-gcc: use command 
```bash
	rm -rf innertest* && g++  *.hpp testToolset.cpp `pkg-config --libs --cflags opencv` -o innertest.o 2>innertest-error.txt && ./innertest.o > innertest-report.out && vim innertest-report.out
	rm -rf innertest*
```

> * on windows-msvs: load all .hpp and testToolset.cpp into visual studio 
```cpp
	#include "stdafx.h"
	// the major content of .hpp ...
```
> * and add the above line at the beginning of each .hpp file, if needed.

## aims
> * (`Finished entries are highlighted.`)
> * (`All Tested on both linux-gcc and windows-msvs`)
> * (`prelim are labeled at the begining of each line`)
> * (`require cite protocol on github, e.g., MIT license, BSD etc.`)
> * (`only use field operator :: to call functions of this toolset`)
> * (require code annotation in details, and in the following format)
``` cpp
	// initialization
	/// <summary>
	/// init data manager
	/// </summary>
	/// <param name="pManData"> data manager pointer </param>
	/// <return> false=failed, true=success </return>
	bool init_with(
		int cps = 1
		)
```
> * (require standard debug function name, including alert and try-catch sentences)
> * (readible API)
> * (comprehensive tutorial)

> 1. (Zero) StringTool: `split, rstrip, lstrip, replace, printStringList, isIncludedIn, isInList`;
> 2. (1) PathTool: `joinPath, getAbsPath, getFiles, getParentPath, getAllFiles, getFileName,` `isFileExist, safeCreateDir, safeCreateFile, safeAppendFile`;
> 3. (1+2) SystemTool: `getRamSize, getRamUseRatio, getRamFreeRatio, getCpuNum, getCpuUseRatio, getDiskSize, getDiskFreeRatio`;
> 4. (OpenCV+1+2) IOTool (.yaml, hdf5, etc.): readConfig, writeConfig, readMat, writeMat;
> 5. (1) TimeTool :`MDateTm, isValidTm, str2tm, operators < + += -, getTmStr, getDate, getClock, MTmFile, init, fname2tmrg, getStartTm, getStopTm, getFileTmRange, getErrTmPt, getCtTime, getCurrDateTm, sortByDateTm`;
> 6. (OpenCV+1+2+3+4+5) VideoTool: `createFromSingleVideo, createFromVideoSeries`, replayVideo, setTmPtInVideo, setVideoRange, setLengthScale;
> 7. RoiTool: setRectRoi, detectRectRoi, readRectRoiSeries, writeRectRoiSeries;
> 8. BgTool: getStaticBg, getDynamicBg, refreshDynamicBg;
> 9. (Zero) ChipTool: (`Win-version`, Linux-version) `connectTo, receiveSignal, sendSignal`;
> 10. WebTool: emailReport.

## 20181008
> 1. complete MTimeTool.hpp;
> 2. stop abuse of 'using namespace toolxx', add field operators instead;
> 3. add func (abstracted template) isInList in MStringTool.hpp;
> 4. setup and test opencv environment, both in win32 and ubuntu;
> 5. generate MVideoTool.hpp file.

## 20181007
> 1. update MTimeTool.hpp

## 20181006
> 1. Add MTimeTool.hpp and add func MDateTm, isValidTm, str2tm, operators < + += -, getTmStr, geFileTmRange, getErrTmPt;

## 20181005
> 1. Add Win-version of MChipTool.hpp;
> 2. Complete Linux-version of MSystemTool.hpp

## 20181001
> 1. Add MSystemTool.hpp;
> 2. Add windows version of func getRamSize, getRamUseRatio, getRamFreeRatio, getCpuNum, getCpuUseRatio in MSystemTool.hpp.

## 20180928
> 1. rename func isExist -> isFileExist in MPathTool.hpp;
> 2. add func safeCreateDir, safeCreateFile, safeAppendFile in MPathTool.hpp;

## 20180927
> 1. add func isExist, getAllFiles in MPathTool.hpp;
> 2. fix bug in windows-version func getFiles in MPathTool.hpp;
> 3. add func getParentPath, getFileName in MPathTool.hpp;

## 20180926

> 1. modify func getFiles at MPathTool.hpp;
> 2. add simple RE features (*, ^, $) in func getFiles.
> 3. add func getAbsPath at MPathTool.hpp;
> 4. add func replace at MStringTool.hpp.

## 20180925

> 1. modify and test MPathTool.hpp to make it linux-compatible;
> 2. set local git linked with github repository;
> 3. push local repository online.
> 4. add function getAllFiles in MPathTool.hpp;

## 20180924

> 1. upload old version previously edited in windows;
> 2. edit README.md;
> 3. modify and test MStringTool.hpp to make it linux-compatible.
