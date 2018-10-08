//===========================
//====>  MDateTimeTool
//===========================
#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <algorithm>
#include "MStringTool.hpp"
using namespace std;
#ifndef _MTIMETOOL_HPP_
#define _MTIMETOOL_HPP_

namespace tooltime {
	class MDateTime {
	public: // interfaces
		/* constructor */
		MDateTime(const string timestr) {
			this->m_pTm = str2tm(timestr);
			if (!this->isValidTm()) {
				cout << "invalid date-time: " << timestr << endl;
				this->m_pTm = NULL;
				this->m_sTm = "";
			}
			this->m_sTm = timestr;
		}
		
		MDateTime(tm tmIn) {
			this->m_pTm = new tm;
			*this->m_pTm = tmIn;
			char buff[64];
			strftime(buff, sizeof(buff), "%Y-%m-%d-%H.%M.%S", this->m_pTm);
			this->m_sTm = buff;
		}

		bool operator < (const MDateTime& other) {
			return difftime(mktime(this->m_pTm), mktime(other.m_pTm)) < 0;
		}

		string getTmStr() {
			return this->m_sTm;
		}

		MDateTime operator + (const double nSec) {
			time_t curr = mktime(this->m_pTm);
			curr += nSec;
			MDateTime* newDt = new MDateTime(this->m_sTm);
			newDt->m_pTm = localtime(&curr);
			char buff[64];
			strftime(buff, sizeof(buff), "%Y-%m-%d-%H.%M.%S", newDt->m_pTm);
			newDt->m_sTm = buff;
			return *newDt;
		}

		void operator += (const double nSec) {
			time_t curr = mktime(this->m_pTm);
			curr += nSec;
			this->m_pTm = localtime(&curr);
			char buff[64];
			strftime(buff, sizeof(buff), "%Y-%m-%d-%H.%M.%S", this->m_pTm);
			this->m_sTm = buff;
		}

		double operator - (const MDateTime& other) {
			return difftime(mktime(this->m_pTm), mktime(other.m_pTm));
		}

		string getDate() { // format: 2018-10-07
			return this->m_sTm.substr(0, 10);
		}
		
		double getClock() { // unit: sec
			double sec = (double)this->m_pTm->tm_hour * 3600;
			sec += (double)this->m_pTm->tm_min * 60;
			sec += (double)this->m_pTm->tm_sec;
			return sec;
		}
		
	private: // internal operations
		bool isValidTm() {
			int yr = this->m_pTm->tm_year + 1900;
			if (yr < 1900) return false;
			bool isLeapYr = false;
			if ((yr % 100 != 0 && yr % 4 == 0) || (yr % 100 == 0 && yr % 400 == 0))
				isLeapYr = true;
			int m = this->m_pTm->tm_mon + 1;
			if (m < 1 || m > 12)
				return false;
			int d = this->m_pTm->tm_mday;
			if ((isLeapYr && m == 2 && d > 28) || (!isLeapYr && m == 2 && d > 27))
				return false;
			if ((m == 1 || m == 3 || m == 5 || m == 7 || m == 8 || m == 10 || m == 12) && d > 31)
				return false;
			if ((m == 2 || m == 4 || m == 6 || m == 9 || m == 11) && d > 30)
				return false;
			int hr = this->m_pTm->tm_hour;
			if (hr < 0 || hr > 23) return false;
			int mi = this->m_pTm->tm_min;
			if (mi < 0 || mi > 59) return false;
			int se = this->m_pTm->tm_sec;
			if (se < 0 || se > 59) return false;
			return true;
		}

		tm* str2tm(const string tmFormated) {
			// specific for format "%Y-%m-%d-%H.%M.%S"
			vector<string> lTerms = toolstring::split(tmFormated, '-');
			tm* t = new tm;
			t->tm_year = atoi(lTerms[0].c_str()) - 1900;
			t->tm_mon = atoi(lTerms[1].c_str()) - 1;
			t->tm_mday = atoi(lTerms[2].c_str());
			vector<string> lTm = toolstring::split(lTerms[3], '.');
			t->tm_hour = atoi(lTm[0].c_str());
			t->tm_min = atoi(lTm[1].c_str());
			t->tm_sec = atoi(lTm[2].c_str());
			return t;
		}

	private: // members
		tm* m_pTm;
		string m_sTm;

	};

	class MTmFile {
	public:
		MTmFile() {}

		MTmFile(const string filename) {
			init(filename);
		}

		void init(const string filename) {
			this->m_fName = filename;
			this->fname2tmrg();
		}

		vector<string> getFileTmRange() {
			vector<string> lTm;
			lTm.push_back(this->m_pTmStart->getTmStr());
			lTm.push_back(this->m_pTmStop->getTmStr());
			return lTm;
		}
		
		MDateTime getStartTm() {
			return *this->m_pTmStart;
		}
		
		MDateTime getStopTm() {
			return *this->m_pTmStop;
		}

	private:
		void fname2tmrg() {
			string prefix = toolstring::rstrip(this->m_fName, ".avi");
			vector<string> lTerms = toolstring::split(prefix, '-');
			this->m_chName = lTerms[0];
			string sStart = lTerms[1] + "-" + lTerms[2] + "-" + lTerms[3] + "-" + lTerms[4];
			string sStop = lTerms[1] + "-" + lTerms[2] + "-" + lTerms[3] + "-" + lTerms[5];
			this->m_pTmStart = new MDateTime(sStart);
			this->m_pTmStop = new MDateTime(sStop);
			if (*this->m_pTmStop < *this->m_pTmStart) {
				*this->m_pTmStop += 24 * 60 * 60;
			}
		}

	public:
		string m_fName;
		string m_chName;
	private:
		MDateTime* m_pTmStart;
		MDateTime* m_pTmStop;
	};

	/* find in-continuous point in a time series */
	vector<int> getErrTmPt(vector<string>& lFTm, int thresh = 60) {
		MTmFile curr, prev;
		vector<int> lIdxErr;
		for (int i = 0; i < (int)lFTm.size(); i++) {
			curr.init(lFTm[i]);
			if (i == 0) {
				prev = curr;
				continue;
			}
			double diff = curr.getStartTm() - prev.getStopTm();
			if ((diff > 0 && diff > thresh) || (diff < 0 && -diff > thresh)) {
				lIdxErr.push_back(i - 1);
			}
			prev = curr;
		}
		return lIdxErr; // no interrupt time point
	}
	
	/* # get circadian time based on given light schedule */
	// ##	<pZt>: MDateTime pointer, its str-format must be "2018-10-07-09.00.00"
	// ##	<hrCtOn>: CT time when light on, unit = hr
	// ##	<return>: CT time (hr), < 12 = day, > 12 = night
	double getCtTime(MDateTime* pZt, double hrCtOn) {
		double hrCt = pZt->getClock() / 3600 - hrCtOn;
		return (hrCt < 0 ? (hrCt + 24) : hrCt);
	}

	MDateTime getCurrDateTm() {
		time_t secCurr = time(NULL);
		tm* pTmCurr = localtime(&secCurr);
		return MDateTime(*pTmCurr);
	}
	
	bool cmpTmFile(const string& a, const string& b) {
		MTmFile tmfA(a), tmfB(b);
		MDateTime startA = tmfA.getStartTm();
		MDateTime startB = tmfB.getStartTm();
		return startA < startB;
	}
	
	void sortByDateTm(vector<string>& lFTm) {
		std::sort(lFTm.begin(), lFTm.end(), cmpTmFile);
	}
	
} // end of namespace tooltime

namespace debug_tooltime {
	using namespace tooltime;
	void debug_time() {
		/* test class MDateTime */
		cout << "## Test class MDateTime\n";
		string s1 = "2017-12-29-09.00.00";
		string s2 = "2018-01-01-09.00.00";
		MDateTime dt1(s1);
		MDateTime dt2(s2);
		double diff = dt2 - dt1;
		if (dt1 < dt2) {
			cout << "\tTime " << dt1.getTmStr() << " is earlier than "
				<< dt2.getTmStr() << endl;
		}
		cout << "\tDifference between " << dt1.getTmStr() << " and " << dt2.getTmStr() << " = " << diff / 60 << " min\n";
		MDateTime dt3 = dt2 + diff;
		cout << "\tAnd if we further add the same difference to " << dt2.getTmStr() << ",\n\tthen the result is " << dt3.getTmStr() << endl;

		/* test class MTmFile */
		cout << "\n## Test class MTmFile\n";
		string fTm = "004-2018-09-26-22.00.00-00.00.00.avi";
		MTmFile hTm(fTm);
		cout << "\tFile " << hTm.m_fName << " time range is:\n";
		toolstring::printStringList(hTm.getFileTmRange());
		cout << "\tchannel-name = " << hTm.m_chName << endl;

		/* test func getErrTmPt */
		cout << "\n## Test func getErrTmPt\n";
		string lStr[] = {
			"004-2018-09-26-08.46.48-10.00.00.avi",
			"004-2018-09-26-10.00.00-12.00.00.avi",
			"004-2018-09-26-12.00.00-14.00.00.avi",
			"004-2018-09-26-14.00.00-15.58.00.avi",
			"004-2018-09-26-16.00.00-18.00.00.avi",
			"004-2018-09-26-18.00.00-20.00.00.avi",
			"004-2018-09-26-20.00.00-22.00.00.avi",
			"004-2018-09-26-22.00.00-00.00.00.avi",
			"004-2018-09-27-00.00.00-02.00.00.avi",
			"004-2018-09-27-02.00.00-04.00.00.avi",
			"004-2018-09-27-04.00.00-06.00.00.avi",
			"004-2018-09-27-06.00.00-08.00.00.avi",
			"004-2018-09-27-08.00.00-10.00.00.avi",
			"004-2018-09-27-10.00.00-12.00.00.avi",
			"004-2018-09-27-12.00.00-13.40.19.avi"
		};
		vector<string> lFTm(lStr, lStr + 15);
		vector<int> lErrTmPt = getErrTmPt(lFTm, 60);
		if (lErrTmPt.size() == 0) cout << "\tTime-file series are continous.\n";
		else {
			for (int i = 0; i < (int)lErrTmPt.size(); i++) {
				cout << "\tDetect abnormal time point between "
					<< lFTm[lErrTmPt[i]] << " and " << lFTm[lErrTmPt[i] + 1]
					<< endl;
			}
		}
		
		/* test func getClock, getDate, getCtTime */
		cout << "\n## Test func getClock, getDate, getCtTime\n";
		string sTmCt16 = "2017-12-29-01.00.00";
		string sTmCt0 = "2017-12-29-09.00.00";
		string sTmCt12hf = "2017-12-29-21.15.00";
		MDateTime dtCt16(sTmCt16), dtCt0(sTmCt0), dtCt12hf(sTmCt12hf);
		double hrCtOn = 9.0;
		cout << "\tDate = " << dtCt16.getDate() << endl;
		double hrCt16 = getCtTime(&dtCt16, hrCtOn);
		double hrCt0 = getCtTime(&dtCt0, hrCtOn);
		double hrCt12hf = getCtTime(&dtCt12hf, hrCtOn);
		cout << "\tTime " << sTmCt16 << " = CT" << hrCt16 << endl
			 << "\tTime " << sTmCt0 << " = CT" << hrCt0 << endl
			 << "\tTime " << sTmCt12hf << " = CT" << hrCt12hf << endl;

		/* test MDateTime(tm) and func getCurrDateTm */
		cout << "\n## Test MDateTime(tm) and func getCurrDateTm\n";
		MDateTime dtCurr = getCurrDateTm();
		cout << "\tCurrent date-time: " << dtCurr.getTmStr().c_str() << endl;
		
		/* test func cmpTmFile, sortByDateTm */
		cout << "\n## Test func cmpTmFile, sortByDateTm\n";
		string ulStr[] = {
			"004-2018-09-26-14.00.00-16.00.00.avi",
			"004-2018-09-26-16.00.00-18.00.00.avi",
			"004-2018-09-27-00.00.00-02.00.00.avi",
			"004-2018-09-27-06.00.00-08.00.00.avi",
			"004-2018-09-27-08.00.00-10.00.00.avi",
			"004-2018-09-26-08.46.48-10.00.00.avi",
			"004-2018-09-26-10.00.00-12.00.00.avi",
			"004-2018-09-26-12.00.00-14.00.00.avi",
			"004-2018-09-27-02.00.00-04.00.00.avi",
			"004-2018-09-27-04.00.00-06.00.00.avi",
			"004-2018-09-26-18.00.00-20.00.00.avi",
			"004-2018-09-26-20.00.00-22.00.00.avi",
			"004-2018-09-26-22.00.00-00.00.00.avi",
			"004-2018-09-27-10.00.00-12.00.00.avi",
			"004-2018-09-27-12.00.00-13.40.19.avi"
		};
		vector<string> ulFTm;
		for (int i=0; i < 15; i++)
			ulFTm.push_back(ulStr[i]);
		cout << "\t### before sorting:\n";
		toolstring::printStringList(ulFTm);
		sortByDateTm(ulFTm);
		cout << "\n\t### after sorting:\n";
		toolstring::printStringList(ulFTm);
	}
}

#endif