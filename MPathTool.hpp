#if defined(__linux__)
	#include <sys/stat.h>
	#include <dirent.h>
#elif defined(_WIN32)
	#include <io.h>
#endif
#include <stdio.h>
#include <iostream>
#include <vector>
using namespace std;
#ifndef _MPATHTOOL_HPP_
#define _MPATHTOOL_HPP_

namespace toolpath {
	string joinPath(string prefix, string suffix) {
		char delim = '/';
		string sep = "";
		#if defined(__linux__)
			delim = '/';
			sep = "/";
		#elif defined(_WIN32)
			delim = '\\';
			sep = "\\"
		#endif
		string result;
		if (prefix[prefix.size() - 1] == delim) {
			return prefix + suffix;
		} else {
			return prefix + sep + suffix;
		}
	}
#if defined(__linux__)
	vector <string> split(const string& src, char delim = ' ') {
		int iL = 0, iR = 0;
		vector <string> ret;
		while (src[iL] == delim) {  // jump all delims at left space
			iL++;
		}
		for (int i = iL; i < src.size(); i++) {  // traverse all char
			if (src[i] == delim) {  // delim found
				iR = i;  // set right marker
				ret.push_back(src.substr(iL, iR - iL));  // split current word
				iL = i+1;  // set left marker to current position
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
	
	vector <string> getFiles(const string rootpath, const string pattern = "*.avi") {
		DIR *pDirRoot = opendir(rootpath.c_str());
		if (pDirRoot == NULL) {
			return vector <string> ();
		}
		dirent *pSub = NULL;
		vector <string> lFiles;
		while(pSub = readdir(pDirRoot)) {
			string fQuery(pSub->d_name);
			bool isMatch = true;
			if (fQuery == "." || fQuery == "..") {
				isMatch = false;
			} else if (pattern.find("*") != string::npos) {
				vector <string> lWords = split(pattern, '*');
				for (int i=0; i< (int)lWords.size(); i++) {
					size_t iFind;
					if (i == 0 && lWords[i][0] == '^') {
						string subpattern = lWords[i].substr(1, lWords[i].size()-1);
						string subquery = fQuery.substr(0, subpattern.size());
						iFind = subquery.find(subpattern);
					} else if (i == lWords.size()-1 && lWords[i][lWords[i].size()-1] == '$'){
						if (lWords[i].size() == 1) {
							continue;
						}
						string subpattern = lWords[i].substr(0, lWords[i].size() - 1);
						string subquery = fQuery.substr(fQuery.size() - subpattern.size(), subpattern.size());
						iFind = subquery.find(subpattern);
					} else {
						iFind = fQuery.find(lWords[i]);
					}
					if (iFind == string::npos) {
						isMatch = false;
					} else {
						size_t iNext = iFind + lWords.size();
						fQuery = fQuery.substr(iNext-1, fQuery.size() - iNext+1);
					}
				}
			} else if (fQuery != pattern) {
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
		string query = joinPath(rootpath, pattern);
		// check if such kind of file exists
		_finddata_t fileinfo;
		long hFind = _findfirst(query.c_str(), &fileinfo);
		// if no such file exists, return empty struct and exit
		if (-1 == hFind) {
			cout << "No File for query: " << query.c_str() << endl;
			return vector<string>();
		}
		// else: enumerate all such files
		vector<string> lFiles;
		while (!_findnext(hFind, &fileinfo)) {
			lFiles.push_back(string(fileinfo.name));
		} _findclose(hFind);
		return lFiles;
	}
#endif
	vector <string> getFiles(const char* rootpath, const char* pattern = "*.avi") {
		return getFiles(string(rootpath), string(pattern) );
	}

}

namespace debug_toolpath {
	using namespace toolpath;
	
	void debug_path() {
		vector <string> lHpp = getFiles(".", "^M*pp");
		string rootdir = "tools of coding";
		vector <string> lPathHpp;
		for(int i=0; i < (int)lHpp.size(); i++) {
			lPathHpp.push_back(joinPath(rootdir, lHpp[i]));
		}
		printStringList(lPathHpp);
	}
}

#endif