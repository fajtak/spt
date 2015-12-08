#ifndef FRAMEHOLDER_H
#define FRAMEHOLDER_H

#include <vector>
#include <iostream>
#include <fstream>
#include <iomanip>

#include "frame.h"
#include "noisyMask.h"
#include "calibration.h"
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH1I.h"
#include "TCanvas.h"
#include "TMath.h"

using namespace std;

class FrameHolder
{
	public :
		void AddFrame(string oneLine);
		int GetNFrames(void){return m_nFrames;};

        void SetNoisyMaskFileName(string noisyMaskFile){m_noisyMask.SetName(noisyMaskFile);};
		void SetName(string fileName){m_rootFileName = fileName;};

        void LoadCalib(string folderName, bool useCalib){m_calib.ReadCalibration(folderName);UseCalib(useCalib);};
		void UseCalib(bool useCalib);

		void ReadData(vector<string> &dataFiles);

        double CountDist(Cluster &a, Cluster &b);

        void PreMasking(vector<string> &dataFiles, int threshold);

		void ProduceResults(int threshold);

		int CreateHist(string subfolder);
		void MaskClusters(string subfolder);
		void EnergyCuts(double lowerLimit, double upperLimit);
        void SizeCuts(int howMany);
		void SSEandDSE(string subfolder);

	private:
		vector<Frame> m_frames;

		string m_dataFileName;
		string m_rootFileName;

		ifstream m_dataFile;

		int m_nFrames;

		NoisyMask m_noisyMask;
		Calibration m_calib;

		TFile* m_rootFile;
};

#endif
