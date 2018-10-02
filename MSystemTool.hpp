//===========================
//====>  MSystemTool
//===========================
#if defined(__linux__)
#elif defined(_WIN32)
	#include <windows.h>
#endif

#include <iostream>
#include <stdio.h>
#include <tchar.h>
using namespace std;


#ifndef _MSYSTEMTOOL_HPP_
#define _MSYSTEMTOOL_HPP_
namespace toolsystem {
	class MSystemInfo {
	public:
		MSystemInfo() {
			// Memory info
			this->pStatex = new MEMORYSTATUSEX;
			this->pStatex->dwLength = sizeof(*this->pStatex);
			// Processor info
			this->pSysInfo = new SYSTEM_INFO;
			GetSystemInfo(pSysInfo);
			this->nCpu = pSysInfo->dwNumberOfProcessors;
			this->update();
		}

	private:
		void update() {
			// Memory info
			GlobalMemoryStatusEx(this->pStatex);
			float unitGb = 1024 * 1024 * 1024;
			this->nMemTotal = (float)this->pStatex->ullTotalPhys / unitGb;
			this->nMemFree = (float)this->pStatex->ullAvailPhys / unitGb;
		}

		__int64 CompareFileTime(FILETIME time1, FILETIME time2)
		{
			__int64 a = time1.dwHighDateTime << 32 | time1.dwLowDateTime;
			__int64 b = time2.dwHighDateTime << 32 | time2.dwLowDateTime;
			return (b - a);
		}


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


	private:
		MEMORYSTATUSEX* pStatex;
		SYSTEM_INFO* pSysInfo;
		float nMemTotal;
		float nMemFree;
		int nCpu;

	};
}

namespace debug_toolsystem {
	void debug_system() {
		using namespace toolsystem;
		MSystemInfo sysinfo;
		float nMemTotal = sysinfo.getRamSize();
		float nMemUseRatio = sysinfo.getRamUseRatio();
		float nMemFreeRatio = sysinfo.getRamFreeRatio();
		cout << "RAM info: Total " << nMemTotal << " GB, "
			<< "Occupy " << nMemUseRatio * 100 << "%, "
			<< "Free " << nMemFreeRatio * 100 << "%.\n";
		Sleep(1000);
		cout << "CPU Num = " << sysinfo.getCpuNum()
			<< ", CPU usage = " << sysinfo.getCpuUseRatio() * 100 << "%\n";

	}
}


#endif
