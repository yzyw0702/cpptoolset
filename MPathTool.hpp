#if defined(__linux__)
	#include <sys/stat.h>
	#include <dirent.h>
	#include <unistd.h>
#elif defined(_WIN32)
	#include <io.h>
	#include <direct.h>
	#include <Windows.h>
	#include <wchar.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
using namespace std;
#ifndef _MPATHTOOL_HPP_
#define _MPATHTOOL_HPP_

namespace toolpath {

	/* --- cite from MStringTool.hpp */
	vector <string> split(const string& src, char delim = ' ') {
		int iL = 0, iR = 0;
		vector <string> ret;
		while (src[iL] == delim) {  // jump all delims at left space
			iL++;
		}
		for (int i = iL; i < (int)src.size(); i++) {  // traverse all char
			if (src[i] == delim) {  // delim found
				iR = i;  // set right marker
				ret.push_back(src.substr(iL, iR - iL));  // split current word
				iL = i + 1;  // set left marker to current position
			}
		}
		if (src[src.size() - 1] != delim) {  // add last one if it's not a delim
			ret.push_back(src.substr(iL, src.size() - iL));
		} return ret;
	}

	void printStringList(const vector <string>& v) {
		for (size_t i = 0; i < v.size(); i++) {
			cout << "[" << i << "] = " << v[i].c_str() << endl;
		}
	}

	string replace(const string& src, char tgt = '\\', char rep = '/') {
		string dst = src;
		for (int i = 0; i<(int)src.size(); i++) {
			if (src[i] == tgt) {
				dst[i] = rep;
			}
		}
		return dst;
	}

	/* --- cite end */
	
	string joinPath(string prefix, string suffix) {
		char delim = '/';
		string sep = "";
	#if defined(__linux__)
		delim = '/';
		sep = "/";
	#elif defined(_WIN32)
		delim = '\\';
		sep = "\\";
	#endif
		if (prefix[prefix.size() - 1] == delim) {
			return prefix + suffix;
		}
		else {
			return prefix + sep + suffix;
		}
	}

#if defined(__linux__)

	vector <string> getFiles(const string rootpath, const string pattern = "*.avi") {
		DIR *pDirRoot = opendir(rootpath.c_str());
		if (pDirRoot == NULL) {
			return vector <string>();
		}
		dirent *pSub = NULL;
		vector <string> lFiles;
		while (pSub = readdir(pDirRoot)) {
			string fQuery(pSub->d_name);
			bool isMatch = true;
			if (fQuery == "." || fQuery == "..") {
				isMatch = false;
			}
			else if (pattern.find("*") != string::npos) {
				vector <string> lWords = split(pattern, '*');
				for (int i = 0; i< (int)lWords.size(); i++) {
					size_t iFind;
					if (i == 0 && lWords[i][0] == '^') {
						string subpattern = lWords[i].substr(1, lWords[i].size() - 1);
						string subquery = fQuery.substr(0, subpattern.size());
						iFind = subquery.find(subpattern);
					}
					else if (i == lWords.size() - 1 && lWords[i][lWords[i].size() - 1] == '$') {
						if (lWords[i].size() == 1) {
							continue;
						}
						string subpattern = lWords[i].substr(0, lWords[i].size() - 1);
						string subquery = fQuery.substr(fQuery.size() - subpattern.size(), subpattern.size());
						iFind = subquery.find(subpattern);
					}
					else {
						iFind = fQuery.find(lWords[i]);
					}
					if (iFind == string::npos) {
						isMatch = false;
					}
					else {
						size_t iNext = iFind + lWords.size();
						fQuery = fQuery.substr(iNext - 1, fQuery.size() - iNext + 1);
					}
				}
			}
			else if (fQuery != pattern) {
				isMatch = false;
			}
			if (isMatch) {
				lFiles.push_back(pSub->d_name);
			}
		}
		return lFiles;
	}

#elif defined(_WIN32)
	// rootpath should be absolute path
	vector <string> getFiles(const string rootpath, const string pattern = "*.avi") {
		// get absolute file path
		string queryDir = joinPath(rootpath, "*");
		// check if such kind of file exists
		_finddata_t fileinfo;
		long hFind = _findfirst(queryDir.c_str(), &fileinfo);
		// filter all files with pattern
		vector<string> lFiles;
		while (!_findnext(hFind, &fileinfo)) {
			bool isMatch = true;
			string fQuery = fileinfo.name;
			if (fQuery == "." || fQuery == "..") {
				isMatch = false;
			}
			else if (pattern.find("*") != string::npos) {
				vector <string> lWords = split(pattern, '*');
				for (int i = 0; i< (int)lWords.size(); i++) {
					size_t iFind;
					if (i == 0 && lWords[i][0] == '^') {
						string subpattern = lWords[i].substr(1, lWords[i].size() - 1);
						string subquery = fQuery.substr(0, subpattern.size());
						iFind = subquery.find(subpattern);
					}
					else if (i == lWords.size() - 1 && lWords[i][lWords[i].size() - 1] == '$') {
						if (lWords[i].size() == 1) {
							continue;
						}
						string subpattern = lWords[i].substr(0, lWords[i].size() - 1);
						string subquery = fQuery.substr(fQuery.size() - subpattern.size(), subpattern.size());
						iFind = subquery.find(subpattern);
					}
					else {
						iFind = fQuery.find(lWords[i]);
					}
					if (iFind == string::npos) {
						isMatch = false;
					}
					else {
						size_t iNext = iFind + lWords.size();
						fQuery = fQuery.substr(iNext - 1, fQuery.size() - iNext + 1);
					}
				}
			}
			else if (fQuery != pattern) {
				isMatch = false;
			}
			if (isMatch) {
				lFiles.push_back(fileinfo.name);
			}
		} _findclose(hFind);
		return lFiles;
	}

#endif

	string getAbsPath(const string path) {
		string respath;
	#if defined(__linux__)
		respath = replace(path, '\\', '/');
		if (respath[0] == '/') {
			return respath;
		}
	#elif defined(_WIN32)
		respath = replace(path, '/', '\\');
		if (respath.substr(1, 2) == ":\\") {
			return respath;
		}
	#endif
		if (path.substr(0, 2) == "./" || path.substr(0, 2) == ".\\") {
			respath = path.substr(2, path.size() - 2);
		}
		else if (path.substr(0, 2) == "..") {
			respath = path.substr(3, path.size() - 3);
		}
		else {
			respath = path;
		}
		char cwd[256];
		getcwd(cwd, 256);
		return joinPath(string(cwd), respath);
	}

	string getParentDir(const string path) {
		char delim;
	#if defined(__linux__)
		delim = '/';
	#elif defined(_WIN32)
		delim = '\\';
	#endif
		size_t iLast = path.find_last_of(delim, path.size());
		string ret;
		if (iLast < path.size()) ret = path.substr(0, iLast);
		else ret = "";
		return ret;
	}

	string getFileName(const string path) {
		char delim;
	#if defined(__linux__)
		delim = '/';
	#elif defined(_WIN32)
		delim = '\\';
	#endif
		vector <string> lTerms = split(path, delim);
		return lTerms[lTerms.size()-1];
	}

	bool isFileExist(const string path) {

	#if defined(__linux__)
		return ((access(path.c_str(), F_OK)) == 0);
	#elif defined(_WIN32)
		return ((_access(path.c_str(), 0)) != -1);
	#endif
	}

	bool isFileOrDirectory(const string path) {
	#if defined(__linux__)
		DIR *pDir = opendir(path.c_str());
		if (pDir != NULL) {
			closedir(pDir);
			return false;
		}
		else
			return true;
	#elif defined(_WIN32)
		DWORD flagFile = GetFileAttributesA(path.c_str());
		return !(flagFile == FILE_ATTRIBUTE_DIRECTORY);
	#endif
	}

	vector <string> getAllFiles(const string rootpath, const string pattern = "*.dav") {
		vector <string> lRelativePaths = getFiles(rootpath, "*");
		if (lRelativePaths.size() == 0)
			return vector <string>();
		vector <string> lAllFiles;
		vector <string> lAllCurrRelativeFiles = getFiles(rootpath, pattern);
		for (int i = 0; i < (int)lAllCurrRelativeFiles.size(); i++) {
			if (rootpath == "." || rootpath == "./" || rootpath == ".\\")
				lAllFiles.push_back(getAbsPath(lAllCurrRelativeFiles[i]));
			else
				lAllFiles.push_back(joinPath(rootpath, lAllCurrRelativeFiles[i]));
		}
		//vector <string> lSubDir;
		for (int i = 0; i < (int)lRelativePaths.size(); i++) {
			string absPath;
			if (rootpath == "." || rootpath == "./" || rootpath == ".\\")
				absPath = getAbsPath(lRelativePaths[i]);
			else
				absPath = joinPath(rootpath, lRelativePaths[i]);
			if (isFileOrDirectory(absPath)) {

			}
			if (!isFileOrDirectory(absPath)) {
				//lSubDir.push_back(absPath);
				vector <string> lNewFiles = getAllFiles(absPath, pattern);
				lAllFiles.insert(lAllFiles.end(), lNewFiles.begin(), lNewFiles.end());
			}
		}
		return lAllFiles;
	}

	bool safeCreateDirectory(const string path) {
		if (!isFileExist(path) || isFileOrDirectory(path)) {
		#if defined(__linux__)
			mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH);
		#elif defined(_WIN32)
			mkdir(path.c_str());
		#endif
			return true;
		}
		else{
			cout << "\t[warning] failed to create the directory '" << path << "' or it already exists.\n";
			return false;
		}
	}
	
	bool safeCreateFile(const string path) {
		if (isFileExist(path) && isFileOrDirectory(path)) {
			cout << "\t[warning] File " << path << " already exists.\n";
			string src = path;
			string dst = path + "-history";
			for (int i=0; isFileExist(dst); i++) {
				char idx[25];
			#if defined(__linux__)
				sprintf(idx, "%d", i+1);
			#elif defined(_WIN32)
				itoa(i+1, idx, 10);
			#endif
				dst = path + "-history-" + string(idx);
			}
		#if defined(__linux__)
			string cmd = "mv \"" + src + "\" \"" + dst + "\"";
			system(cmd.c_str());
		#elif defined(_WIN32)
			MoveFileA(src.c_str(), dst.c_str());
		#endif
		}
		ofstream ofs;
		ofs.open(path.c_str());
		if (!ofs.is_open())
			return false;
		else {
			ofs.close();
			return true;
		}
		
	}
	
	void safeAppendFile(const string path, const string s) {
		if (isFileExist(path) && isFileOrDirectory(path)) {
			fstream fs(path.c_str(), ios::out | ios::app);
			fs << s;
			fs.close();
		} else {
			fstream fs(path.c_str(), ios::out);
			fs << s;
			fs.close();
		}
	}

	vector <string> getlines(const string path) {
		vector <string> lLines;
		string line;
		fstream ifs(path.c_str(), ios::in);
		while(getline(ifs, line)) {
			lLines.push_back(line);
		}
		ifs.close();
		return lLines;
	}
}

namespace debug_toolpath {
	using namespace toolpath;

	void debug_path() {
		cout << "# ---- MPathTool ----\n";
		// test func joinPath
		cout << "\n## Test func joinPath\n";
		string prefix = "directory";
		string filename = "filename.txt";
		cout << joinPath(prefix, filename) << endl;
		
		// test func getFiles
		cout << "\n## Test func getFiles\n";
		vector <string> lpp = getFiles(".", "*pp$");
		string rootdir = ".\\";
		vector <string> lPathpp;
		for (int i = 0; i < (int)lpp.size(); i++) {
			lPathpp.push_back(rootdir + lpp[i]);
		}
		printStringList(lPathpp);
		
		// test func getAbsPath
		cout << "\n## Test func getAbsPath\n";
		string pathHpp1 = getAbsPath(lpp[0]);
		string pathHpp2 = getAbsPath(lPathpp[0]);
		cout << pathHpp1 << endl << pathHpp2 << endl;
		
		// test func isFileOrDirectory
		cout << "\n## Test func isFileOrDirectory\n";
		vector <string> lPaths = getFiles(".", "*");
		printStringList(lPaths);
		for (int i = 0; i< (int)lPaths.size(); i++) {
			string absPath = getAbsPath(lPaths[i]);
			if (!isFileExist(absPath)) {
				cout << absPath << " not exists!\n";
				continue;
			}
			if (isFileOrDirectory(absPath)) cout << absPath << " is a file.\n";
			else cout << absPath << " is a directory.\n";
		}
		
		// test func getParentDir
		cout << "\n## Test func getParentDir\n";
		cout << "parent = " << getParentDir(getAbsPath(lPaths[0])) << endl;
		
		// test func getFileName
		cout << "\n## Test func getFileName\n";
		cout << "filename = " << getFileName(getAbsPath(lPaths[0])) << endl;
		
		// test func getAllFiles
		cout << "\n## Test func getAllFiles\n";
		vector <string> lAllPaths = getAllFiles(".", "*e$");
		printStringList(lAllPaths);
		
		// test func safeCreateDirectory
		cout << "\n## Test func safeCreateDirectory\n";
		bool isCreateDir = safeCreateDirectory("innertest-safedir");
		if (isCreateDir) cout << "\tcreated a directory.\n";
		else cout << "\tfailed to create a new directory. already exists.\n";
		
		// test func safeCreateFile
		cout << "\n## Test func safeCreateFile\n";
		string fNew = "./innertest-NewFile.txt";
		for (int i=0; i < 3; i++) {
			if (!safeCreateFile(fNew)) cout << "failed to safe-create new file.\n";
		}
		
		// test func safeAppendFile
		cout << "\n## Test func safeAppendFile\n";
		string fTarget = getAbsPath("innertest-AppendFile.txt");
		string strToAppend = "new line\n";
		for (int i=0; i < 3; i++) {
			safeAppendFile(fTarget, strToAppend);
		}
	}
}

#endif
