//===========================
//====>  MStringTool
//===========================
#include <iostream>
#include <vector>
#include <string>
using namespace std;
#ifndef _MSTRINGTOOL_HPP_
#define _MSTRINGTOOL_HPP_

namespace toolstring {
	void printStringList(const vector <string>& v) {
		for (size_t i = 0; i < v.size(); i++) {
			cout << "\t[" << i << "] = " << v[i] << endl;
		}
	}
	
	string replace(const string src, char tgt = '\\', char rep = '/') {
		string dst = src;
		for (int i=0; i<(int)src.size(); i++) {
			if (src[i] == tgt) {
				dst[i] = rep;
			}
		}
		return dst;
	}
	
	string replace(const char* src, char tgt = '\\', char rep = '/' ) {
		return replace(string(src), tgt, rep);
	}

	vector <string> split(const string src, char delim = ' ') {
		int iL = 0, iR = 0;
		vector <string> ret;
		while (src[iL] == delim) {  // jump all delims at left space
			iL++;
		}
		for (int i = iL; i < (int)src.size(); i++) {  // traverse all char
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
	
	string rstrip(const string src, char c = ' ') {
		int n = src.size();
		string dst;
		while (src[n - 1] == c && n > 0) n--;
		dst = src.substr(0, n);
		return dst;
	}

	string lstrip(const string src, char c = ' ') {
		int n = 0;
		int N = src.size();
		string dst;
		while (src[n] == c && n < N) n++;
		dst = src.substr(n, N);
		return dst;
	}
	
	string rstrip(const string src, const string lpatterns) {
	bool isFinish = false;
	char suffix;
	int iLast = (int)src.size();
	while (!isFinish) {  // clear all the characters listed in lpatterns
		suffix = src[iLast - 1];
		bool isClear = true;
		for (int i = 0; i < lpatterns.size(); i++) {  // check the list lpatters
			if (suffix == lpatterns[i]) {
				iLast--;
				isClear = false;
				break;
			}
		}
		if (isClear || iLast == 0)  // exit only when all listed suffices are cleared
			isFinish = true;
	}
	return src.substr(0, iLast);
	}

	string lstrip(const string src, const string lpatterns) {
		bool isFinish = false;
		char prefix;
		int iFirst = 0;
		while (!isFinish) {  // clear all the characters listed in lpatterns
			prefix = src[iFirst];
			bool isClear = true;
			for (int i = 0; i < lpatterns.size(); i++) {  // check the list lpatters
				if (prefix == lpatterns[i]) {
					iFirst++;
					isClear = false;
					break;
				}
			}
			if (isClear || iFirst == src.size() - 1)  // exit only when all listed suffices are cleared
				isFinish = true;
		}
		return src.substr(iFirst, src.size());
	}
	
	bool isIncludedIn(const string query, const string target) {
		if (query.size() > target.size()) {
			cout << "\t[toolstring::isIncludedIn()] target string \"" << target.c_str() << "\" is too short.\n"; 
			return false;
		}
		for(int i=0; i < (int)(target.size() - query.size()); i++) {
			if (query == target.substr(i, query.size())) return true;
		}
		return false;
	}

};

namespace debug_toolstring {
	using namespace toolstring;

	void debug_split() {
		string s1 = "This is a string list.";
		string s2 = "This is another string list with some bug. ";
		string s3 = " There is a space at first position.";
		string s4 = "     Several left spaces appear. ";
		string s5 = "Several in-between     spaces should be split.";
		string s6 = "use,comma,to,split,this,string,";
		printStringList(split(s1));
		printStringList(split(s2));
		printStringList(split(s3));
		printStringList(split(lstrip(s4)));
		printStringList(split(s5));
		printStringList(split(s6, ','));
	}
};

#endif
