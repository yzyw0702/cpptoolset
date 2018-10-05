# cpptoolset
simple-prelim and file-independent tool kits for operations of string, path, memory / cpu / disk info, matrix, date / time, video, roi. in c++

## test hint
> * on linux-gcc: use command 
```bash
	rm -rf innertest* && g++ *.hpp testToolset.cpp -o innertest.o 2>innertest-error.txt && ./innertest.o > innertest-report.out && vim innertest-report.out
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

> 1. (Zero) StringTool: `split, rstrip, lstrip, replace`
> 2. (Zero) PathTool: `joinPath, getAbsPath, getFiles, getParentPath, getAllFiles, getFileName,` `isFileExist, safeCreateDir, safeCreateFile, safeAppendFile`;
> 3. (StringTool) SystemTool: (`Win-Version`, Linux-Version) `getRamSize, getRamUseRatio, getRamFreeRatio, getCpuNum, getCpuUseRatio, getDiskSize, getDiskFreeRatio`;
> 4. MatTool (OpenCV-based, yaml-format): readMat, writeMat;
> 5. TimeTool (date and time): isValidTm, isValidDate, isValidDateTm, isDateSeriesContinuous, getAbsDate, getAbsTm, isDayOrNight, getCurrDateTm, getTmDiff, sortByDateTm;
> 6. VideoTool: readVideoSeries, replayVideoSeries, setTmPtInVideoSeries, setVideoRange, setLengthScale;
> 7. RoiTool: setRectRoi, detectRectRoi, readRectRoiSeries, writeRectRoiSeries;
> 8. BgTool: getStaticBg, getDynamicBg, refreshDynamicBg;
> 9. ChipTool: (`Win-version`, Linux-version) `connectTo, receiveSignal, sendSignal`;
> 10. WebTool: emailReport.

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
