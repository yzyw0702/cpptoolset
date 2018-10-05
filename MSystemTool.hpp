//===========================
//====>  MSystemTool
//===========================
#if defined(__linux__)
	#include <fstream>
	#include <stdlib.h>
	#include "MStringTool.hpp"
	#include "MPathTool.hpp"
	using namespace toolstring;
	using namespace toolpath;

#elif defined(_WIN32)
	#include <windows.h>
	#include <direct.h>
#endif

#include <iostream>
#include <stdio.h>
using namespace std;


#ifndef _MSYSTEMTOOL_HPP_
#define _MSYSTEMTOOL_HPP_
namespace toolsystem {
	class MSystemInfo {
	public:
		MSystemInfo() {
		#if defined(__linux__)
			// Memory info
			this->fMemInfo = "/proc/meminfo";
			if (!isFileExist(this->fMemInfo)) {
				cout << "memory information file " << this->fMemInfo << " is missing.\n";
				return;
			}
			this->nMemTotal = getMemAttrbKb(this->fMemInfo, "MemTotal") / (1024 * 1024);
			// Processor info
			this->fCpuInfo = "/proc/cpuinfo"
			vector <string> lLines = getlines(this->fCpuInfo);
			this->nCpu = 0;
			for(int i=0; i < (int)lLines.size(); i++) {
				if (isIncludedIn("processor", lLines[i])) this->nCpu ++;
			}
		#elif defined(_WIN32)
			// Memory info
			this->pStatex = new MEMORYSTATUSEX;
			this->pStatex->dwLength = sizeof(*this->pStatex);
			// Processor info
			this->pSysInfo = new SYSTEM_INFO;
			GetSystemInfo(pSysInfo);
			this->nCpu = pSysInfo->dwNumberOfProcessors;
		#endif
			this->update();
		}

	private:
		void update() {
			// Memory info
		#if defined(__linux__)
			this->nMemFree = getMemAttrbKb(this->fMemInfo, "MemFree") / (1024 * 1024);
		#elif defined(_WIN32)
			GlobalMemoryStatusEx(this->pStatex);
			float unitGb = 1024 * 1024 * 1024;
			this->nMemTotal = (float)this->pStatex->ullTotalPhys / unitGb;
			this->nMemFree = (float)this->pStatex->ullAvailPhys / unitGb;
		#endif
		}
	
	#if defined(__linux__)
		float getMemAttrbKb(const string fMemInfo, const string sAttrb) {
			vector<string> lInfo = getlines(fMemInfo);
			int val = -1;
			for (int i=0; i< (int)lInfo.size(); i++) {
				if (isIncludedIn(sAttrb, lInfo[i])) {
					vector <string> lTerms = toolstring::split(lInfo[i], ':');
					string term = rstrip(lstrip(lTerms[1], ' '), " kB\r\n");
					val = atoi(term.c_str());
					return (float)val;
				}
			}
			cout << "memoryinfo (" << sAttrb << ") does not exist.\n";
			return -1;
		}
		
		vector<int> getCpuStat(const string fCpuStat) {
			vector<string> lInfo = getlines(fCpuStat);
			vector<int> lVal;
			vector<string> cpuinfo = toolstring::split(lInfo[0], ' ');
			for (int i=0; i < (int)cpuinfo.size(); i++) {
				if (i == 0 || cpuinfo[i].size() < 1) continue;
				
			}
		}
	
	#elif defined(_WIN32)
		__int64 CompareFileTime(FILETIME tm1, FILETIME tm2) {
			__int64 tma = tm1.dwHighDateTime << 32 | tm1.dwLowDateTime;
			__int64 tmb = tm2.dwHighDateTime << 32 | tm2.dwLowDateTime;
			return (tmb - tma);
		}
	#endif
		

	public:
		float getRamSize() {
			return this->nMemTotal;
		}

		float getRamUseRatio() {
			this->update();
			return 1 - (float)this->nMemFree / (float)this->nMemTotal;
		}

		float getRamFreeRatio() {
			this->update();
			return (float)this->nMemFree / (float)this->nMemTotal;
		}

		int getCpuNum() {
			return this->nCpu;
		}

		float getCpuUseRatio() {
			FILETIME tmIdlBef, tmIdlAft;
			FILETIME tmKernBef, tmKernAft;
			FILETIME tmUsrBef, tmUsrAft;
			BOOL isOk = GetSystemTimes(&tmIdlAft, &tmKernAft, &tmUsrAft);
			tmIdlBef = tmIdlAft;
			tmKernBef = tmKernAft;
			tmUsrBef = tmUsrAft;
			HANDLE hSampl = CreateEventA(NULL, FALSE, FALSE, NULL);
			WaitForSingleObject(hSampl, 1000);
			isOk = GetSystemTimes(&tmIdlAft, &tmKernAft, &tmUsrAft);
			__int64 tmIdlDiff = this->CompareFileTime(tmIdlBef, tmIdlAft);
			__int64 tmKernDiff = this->CompareFileTime(tmKernBef, tmKernAft);
			__int64 tmUsrDiff = this->CompareFileTime(tmUsrBef, tmUsrAft);
			float diff = (float)(tmKernDiff + tmUsrDiff - tmIdlDiff) / (float)(tmKernDiff + tmUsrDiff);
			// float freeRatio = tmIdlDiff / (tmKernDiff + tmUsrDiff);
			return diff;
		}

	#if defined(_WIN32)
		float getDiskSize(string diskflag = "C:\\") {
			int DType = GetDriveTypeA(diskflag.c_str());
			if (DType == DRIVE_CDROM || DType == DRIVE_UNKNOWN) {
				cout << "invalid disk type.\n";
				return -1;
			}
			unsigned _int64 bt2cFree, btTotal, btFree;
			BOOL isOk = GetDiskFreeSpaceExA(
				diskflag.c_str(),
				(PULARGE_INTEGER)&bt2cFree,
				(PULARGE_INTEGER)&btTotal,
				(PULARGE_INTEGER)&btFree
			);
			if (isOk) return (float)btTotal / (float)(1024 * 1024 * 1024);
			else return -1;
		}

		float getDiskFreeRatio(string diskflag) {
			int DType = GetDriveTypeA(diskflag.c_str());
			if (DType == DRIVE_CDROM || DType == DRIVE_UNKNOWN) {
				cout << "invalid disk type.\n";
				return 0;
			}
			unsigned _int64 bt2cFree, btTotal, btFree;
			BOOL isOk = GetDiskFreeSpaceExA(
				diskflag.c_str(),
				(PULARGE_INTEGER)&bt2cFree,
				(PULARGE_INTEGER)&btTotal,
				(PULARGE_INTEGER)&btFree
			);
			if (isOk) return (float)bt2cFree / (float)btTotal;
			else return 0;
		}

	#endif
		
	private:
	#if defined(__linux__)
		string fMemInfo;
		string fCpuInfo;
	#elif defined(_WIN32)
		MEMORYSTATUSEX* pStatex;
		SYSTEM_INFO* pSysInfo;
	#endif
		float nMemTotal;
		float nMemFree;
		int nCpu;

	};
}

namespace debug_toolsystem {
	void debug_system() {
		using namespace toolsystem;
		/* test RAM info */
		cout << "\n## Test RAM info functions\n";
		MSystemInfo sysinfo;
		float nMemTotal = sysinfo.getRamSize();
		float nMemUseRatio = sysinfo.getRamUseRatio();
		float nMemFreeRatio = sysinfo.getRamFreeRatio();
		cout << "\tRAM info: Total " << nMemTotal << " GB, "
			<< "Occupy " << nMemUseRatio * 100 << "%, "
			<< "Free " << nMemFreeRatio * 100 << "%.\n";
		Sleep(1000);
		/* test CPU info */
		cout << "\n## Test CPU info functions\n";
		cout << "\tCPU Num = " << sysinfo.getCpuNum()
			<< ", CPU usage = " << sysinfo.getCpuUseRatio() * 100 << "%\n";
		/* test disk info */
		//cout << "\n## Test disk info functions\n";
		//char currdir[255];
		//getcwd(currdir, 255);
		//string cwd = currdir;
		//string diskflag = cwd.substr(0, 3); // demo: "d:\\"
		//cout << "\tDisk " << diskflag.c_str() << " total = " << sysinfo.getDiskSize(diskflag) << " GB, "
			//<< "free = " << sysinfo.getDiskFreeRatio(diskflag) * 100 << "%\n";

	}
}


#endif