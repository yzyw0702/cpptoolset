//===========================
//====>  MSystemTool
//===========================
#ifndef _MSYSTEMTOOL_HPP_
#define _MSYSTEMTOOL_HPP_
#if defined(__linux__)
	#include <fstream>
	#include <stdlib.h>
	#include <sys/statvfs.h>
	#include "MStringTool.hpp"
	#include "MPathTool.hpp"
	
#elif defined(_WIN32)
	#include <windows.h>
	#include <direct.h>
#endif

#include <iostream>
#include <stdio.h>
using namespace std;

namespace toolsystem {
#define UNIT_GB (1024 * 1024 * 1024)
	class MSystemInfo {
	public:
		MSystemInfo() {
		#if defined(__linux__)
			// Memory info
			this->fMemInfo = "/proc/meminfo";
			if (!toolpath::isFileExist(this->fMemInfo)) {
				cout << "memory information file " << this->fMemInfo << " is missing.\n";
				return;
			}
			this->nMemTotal = this->getMemAttrbKb(this->fMemInfo, "MemTotal") / (1024 * 1024);
			// Processor info
			this->fCpuInfo = "/proc/cpuinfo";
			this->fCpuStat = "/proc/stat";
			vector <string> lLines = toolpath::getlines(this->fCpuInfo);
			this->nCpu = 0;
			for(int i=0; i < (int)lLines.size(); i++) {
				if (lLines[i].size() < 1) continue;
				if (toolstring::isIncludedIn("processor", lLines[i])) this->nCpu ++;
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
			this->nMemFree = this->getMemAttrbKb(this->fMemInfo, "MemFree") / (1024 * 1024);
		#elif defined(_WIN32)
			GlobalMemoryStatusEx(this->pStatex);
			this->nMemTotal = (float)this->pStatex->ullTotalPhys / UNIT_GB;
			this->nMemFree = (float)this->pStatex->ullAvailPhys / UNIT_GB;
		#endif
		}
	
	#if defined(__linux__)
		float getMemAttrbKb(const string fMemInfo, const string sAttrb) {
			vector<string> lInfo = toolpath::getlines(fMemInfo);
			int val = -1;
			for (int i=0; i< (int)lInfo.size(); i++) {
				if (toolstring::isIncludedIn(sAttrb, lInfo[i])) {
					vector <string> lTerms = toolstring::split(lInfo[i], ':');
					string term = toolstring::rstrip(toolstring::lstrip(lTerms[1], ' '), " kB\r\n");
					val = atoi(term.c_str());
					return (float)val;
				}
			}
			cout << "memoryinfo (" << sAttrb << ") does not exist.\n";
			return -1;
		}
		
		vector<int> getCpuTm() {
			vector<string> lInfo = toolpath::getlines(this->fCpuStat);
			vector<int> lVal;
			vector<string> cpustat = toolstring::split(lInfo[0], ' ');
			for (int i=0; i < (int)cpustat.size(); i++) {
				if (i == 0 || cpustat[i].size() < 1) continue;
				lVal.push_back(atoi(cpustat[i].c_str()));
			}
			return lVal;
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
	
	#if defined(__linux__)
		float getCpuUseRatio() {
			vector<int> lTmBef = this->getCpuTm();
			sleep(1);
			vector<int> lTmAft = this->getCpuTm();
			float totalDiff = 0, idlDiff = 0;
			for(int i=0; i < (int)lTmBef.size(); i++) {
				int diff = lTmAft[i] - lTmBef[i];
				totalDiff += diff;
				if (i == 3) { // the 4th value is idle time
					idlDiff = diff;
				}
			}
			return (totalDiff - idlDiff) / totalDiff;
		}
	
	#elif defined(_WIN32)
		float getCpuUseRatio() {
			FILETIME tmIdlBef, tmIdlAft;
			FILETIME tmKernBef, tmKernAft;
			FILETIME tmUsrBef, tmUsrAft;
			BOOL isOk = GetSystemTimes(&tmIdlAft, &tmKernAft, &tmUsrAft);
			tmIdlBef = tmIdlAft;
			tmKernBef = tmKernAft;
			tmUsrBef = tmUsrAft;
			HANDLE hSampl = CreateEventA(NULL, FALSE, FALSE, NULL);
			WaitForSingleObject(hSampl, 10);
			isOk = GetSystemTimes(&tmIdlAft, &tmKernAft, &tmUsrAft);
			__int64 tmIdlDiff = this->CompareFileTime(tmIdlBef, tmIdlAft);
			__int64 tmKernDiff = this->CompareFileTime(tmKernBef, tmKernAft);
			__int64 tmUsrDiff = this->CompareFileTime(tmUsrBef, tmUsrAft);
			float diff = (float)(tmKernDiff + tmUsrDiff - tmIdlDiff) / (float)(tmKernDiff + tmUsrDiff);
			// float freeRatio = tmIdlDiff / (tmKernDiff + tmUsrDiff);
			return diff;
		}
	
	#endif
	
	#if defined(__linux__)
		float getDiskSize(string rootpath = "/home") {
			struct statvfs vfs;
			int error = statvfs(rootpath.c_str(), &vfs);
			fsblkcnt_t volTotal = vfs.f_blocks;
			fsblkcnt_t volUnit = vfs.f_bsize;
			return (float)volTotal * (float)volUnit / UNIT_GB;
		}
		
		float getDiskFreeRatio(string rootpath) {
			struct statvfs vfs;
			int error = statvfs(rootpath.c_str(), &vfs);
			fsblkcnt_t volFree = vfs.f_bfree;
			fsblkcnt_t volTotal = vfs.f_blocks;
			return (float)volFree / (float)volTotal;
		}
	
	#elif defined(_WIN32)
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
			if (isOk) return (float)btTotal / (float)UNIT_GB;
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
		string fCpuStat;
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
		cout << "# ---- MSystemTool ----\n";
		/* test RAM info */
		cout << "\n## Test RAM info functions\n";
		MSystemInfo sysinfo;
		float nMemTotal = sysinfo.getRamSize();
		float nMemUseRatio = sysinfo.getRamUseRatio();
		float nMemFreeRatio = sysinfo.getRamFreeRatio();
		cout << "\tRAM info: Total " << nMemTotal << " GB, "
			<< "Occupy " << nMemUseRatio * 100 << "%, "
			<< "Free " << nMemFreeRatio * 100 << "%.\n";
	#if defined(__linux__)
		sleep(1);
	#elif defined(_WIN32)
		Sleep(1000);
	#endif
		/* test CPU info */
		cout << "\n## Test CPU info functions\n";
		cout << "\tCPU Num = " << sysinfo.getCpuNum()
			<< ", CPU usage = " << sysinfo.getCpuUseRatio() * 100 << "%\n";
		/* test disk info */
		cout << "\n## Test disk info functions\n";
	#if defined(__linux__)
		string diskflag = "/home";
	#elif defined(_WIN32)
		char currdir[255];
		getcwd(currdir, 255);
		string cwd = currdir;
		string diskflag = cwd.substr(0, 3); // demo: "d:\\"
	#endif
		cout << "\tDisk " << diskflag.c_str() << " total = " << sysinfo.getDiskSize(diskflag) << " GB, "
			<< "free = " << sysinfo.getDiskFreeRatio(diskflag) * 100 << "%\n";

	}
}


#endif