#ifndef FRAME_H
#define FRAME_H

#include <vector>
#include <string>
#include <algorithm>

#include "cluster.h"
#include "noisyMask.h"

using namespace std;

class Frame
{
	public:
        Frame(void);

		void AddCluster(string oneLine, NoisyMask& myNoisyMask);
        Cluster & GetCluster(int which){return m_clusters[which];};
		Cluster & GetFirstGood(void);
		Cluster & GetSecondGood(void);

		void SetTime(double time){m_time = time;};
		void SetID(int ID){m_ID = ID;};
		void SetOID(int oID){m_OID = oID;};
		void SetAcqTime(double acqTime){m_acqTime = acqTime;};
        double GetTime(void){return m_time;};
		int GetID(void){return m_ID;};
		int GetOID(void){return m_OID;};
		double GetAcqTime(void){return m_acqTime;};
		int GetNClusters(void){return m_nClusters;};
		int GetNGoodClusters(void);

	private:
		vector<Cluster> m_clusters;
		double m_time;
		int m_ID;
		int m_OID; //ordered ID
		int m_nClusters;
		double m_acqTime;
};

#endif
