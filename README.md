# cpptoolset
simple-pre-requisite and file-independent tool kits for operations of string, path, matrix, memory etc. in c++

## aims
> (`Finished entries are highlighted.`)
> 1. StringTool: `split, rstrip, lstrip, replace`
> 2. PathTool: `joinPath, getAbsPath, getFiles`, getParentPath, getFileName, getAllFiles, safeCreateFile, appendToFile, safeReadFile;
> 3. MatTool (OpenCV-based, txt-format): readMat, saveMat;
> 4. MemoryTool: getMemorySize.

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
