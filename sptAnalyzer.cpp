#include <iostream>
#include <string>
#include <vector>

#include "frameHolder.h"
#include "TApplication.h"

using namespace std;

void SetSPT(char* argv[], vector<string> &dataFiles, string &calibFolder, string &noisyMaskFile, string &resultsFile, int &threshold, bool &useCalib)
{
    if (strcmp(argv[1], "1") == 0)
    {
        for (int idFile = 0 ; idFile < dataFiles.size(); idFile++)
        {
            dataFiles[idFile] += "SPT01.txt";
        }
        calibFolder += "SPT001/";
        threshold = 18;
        noisyMaskFile += "spt01.txt";
        resultsFile += "spt01.root";
    }
    if (strcmp(argv[1], "2") == 0)
    {
        for (int idFile = 0 ; idFile < dataFiles.size(); idFile++)
        {
            dataFiles[idFile] += "SPT02.txt";
        }
        calibFolder += "SPT002/";
        threshold = 1000;
        noisyMaskFile += "spt02.txt";
        resultsFile += "spt02.root";
        useCalib = false;
    }
    if (strcmp(argv[1], "3") == 0)
    {
        for (int idFile = 0 ; idFile < dataFiles.size(); idFile++)
        {
            dataFiles[idFile] += "SPT03.txt";
        }
        calibFolder += "SPT003/";
        threshold = 40;
        noisyMaskFile += "spt03.txt";
        resultsFile += "spt03.root";
    }
    if (strcmp(argv[1], "4") == 0)
    {
        for (int idFile = 0 ; idFile < dataFiles.size(); idFile++)
        {
            dataFiles[idFile] += "SPT04.txt";
        }
        calibFolder += "SPT004/";
        threshold = 16;
        noisyMaskFile += "spt04.txt";
        resultsFile += "spt04.root";
    }
}

int main(int argc, char* argv[])
{
    FrameHolder myFrameHolder;

    vector<string> dataFiles;
    dataFiles.push_back("/windows/sptData/201410100955/");
    dataFiles.push_back("/windows/sptData/201411191500/");
    dataFiles.push_back("/windows/sptData/201502071500/");
    //dataFiles.push_back("/windows/sptData/201502241704/");
    //dataFiles.push_back("/windows/sptData/201503121110/");
    //dataFiles.push_back("/windows/sptData/201503121110/");
    dataFiles.push_back("/windows/sptData/201503191800/"); // 1s acquisition
    //dataFiles.push_back("/windows/sptData/201504021600/"); // 5s acquisition
    //dataFiles.push_back("/windows/sptData/201505181632/");
    //dataFiles.push_back("/windows/sptData/201507071508/");
    //dataFiles.push_back("/windows/sptData/201508182024/");
    //dataFiles.push_back("/windows/sptData/201510091500/");

    string calibFolder = "/windows/sptData/SPTS/Horizontaly/";
    string noisyMaskFile = "results/noisyMask";
    //string resultsFile = "results/";
    string resultsFile = "results/check/";
    int threshold = 0;
    bool useCalib = true;

    SetSPT(argv,dataFiles,calibFolder,noisyMaskFile,resultsFile,threshold,useCalib);

    myFrameHolder.SetName(resultsFile);
    myFrameHolder.SetNoisyMaskFileName(noisyMaskFile);
    if (strcmp(argv[2], "1") == 0)
    {
        if (argc == 4)
            threshold = atoi(argv[3]);
        myFrameHolder.PreMasking(dataFiles,threshold);
    }else
    {
        myFrameHolder.ReadData(dataFiles);
        myFrameHolder.LoadCalib(calibFolder,useCalib);
        myFrameHolder.ProduceResults(threshold);
    }
	return 0;
}
