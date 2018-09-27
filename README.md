# cpptoolset
zero-prelim and file-independent tool kits for operations of string, path, memory, matrix, time, video, roi. in c++

## test hint
> * on liniux-gcc: use command 
```bash
	g++ *.hpp testToolset.cpp -o test.o && ./test.o > report.out && vim report.out
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

> 1. StringTool: `split, rstrip, lstrip, replace`
> 2. PathTool: `joinPath, getAbsPath, getFiles, getParentPath, getAllFiles, getFileName`, safeCreateFile, appendToFile, safeReadFile;
> 3. MemoryTool: getMemorySize;
> 4. MatTool (OpenCV-based, yaml-format): readMat, writeMat;
> 5. TimeTool (date and time): isValidTm, isValidDate, isValidDateTm, getAbsDate, getAbsTm, isDayOrNight, getCurrDateTm, getTmDiff, sortByDateTm;
> 6. VideoTool: readVideoSeries, replayVideoSeries, setTmPtInVideoSeries, setVideoRange, setLengthScale;
> 7. RoiTool: setRectRoi, detectRectRoi, readRectRoiSeries, writeRectRoiSeries;
> 8. BgTool: getStaticBg, getDynamicBg, refreshDynamicBg;
> 9. ChipTool: connectArduino, readArduino, writeArduino;
> 10. WebTool: emailReport.

## 20180927
> 1. add func getAllFiles in MPathTool.hpp;
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
