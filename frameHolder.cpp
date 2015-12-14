#include "frameHolder.h"
#include "TCanvas.h"

double FrameHolder::CountDist(Cluster &a, Cluster &b)
{
    return sqrt(pow(a.GetXPos()-b.GetXPos(),2)+pow(a.GetYPos()-b.GetYPos(),2));
}

void FrameHolder::AddFrame(string oneLine)
{
    if ((m_frames.size()>10)&&(m_frames.back().GetNClusters() == 0))
    {
        m_frames.pop_back();
    }
	m_frames.push_back(Frame());
    int pos1 = oneLine.find("(");
    int pos2 = oneLine.find("s");
    int pos3 = oneLine.find(")");
    m_frames.back().SetID(atoi(oneLine.substr(5,pos1-1).c_str()));
    m_frames.back().SetTime(atof(oneLine.substr(pos1+1,pos2-2).c_str()));
    m_frames.back().SetAcqTime(atof(oneLine.substr(pos2+3,pos3-3).c_str()));
    m_frames.back().SetOID(m_nFrames);
    m_nFrames++;
}

void FrameHolder::ReadData(vector<string> &dataFiles)
{
    double time = 0;
    int frame = 0;
    double acqTimeWD = 0;
    m_noisyMask.LoadNoisyMask();
    m_noisyMask.Update();

    for (int idFile = 0 ; idFile < dataFiles.size() ; idFile++)
    {
        m_dataFileName = dataFiles[idFile];
        m_dataFile.open(m_dataFileName);

        string oneLine;

        if (!m_dataFile)
        {
            // Print an error and exit
            cerr << "Uh oh, your file \"" << m_dataFileName << "\" could not be opened for reading!" << endl;
        }else{
            //cout << "File \"" << m_name << "\" is open and ready for reading!" << endl;
            cout << "Start reading file : " << m_dataFileName << endl;
            while(m_dataFile)
            {
                getline(m_dataFile,oneLine);
                if (oneLine[0] == 'F')
                    AddFrame(oneLine);
                else if (oneLine[0] == '[')
                    m_frames.back().AddCluster(oneLine,m_noisyMask);
                else
                    ;
            }
        }
        m_dataFile.close();
        //m_nFrames = m_frames.size();
        time += m_frames[m_frames.size()-1].GetTime() - m_frames[frame].GetTime();
        acqTimeWD += (m_frames[m_frames.size()-1].GetOID()-m_frames[frame].GetOID())*m_frames[m_frames.size()-1].GetAcqTime();
        frame = m_frames.size();
    }
    cout << "Stop reading!" << endl;

    cout << "Data per " << acqTimeWD/3600/24 << " days / " << time/3600/24 << " days" << endl;
}

void FrameHolder::PreMasking(vector<string> &dataFiles, int threshold)
{
    for (int idFile = 0 ; idFile < dataFiles.size() ; idFile++)
    {
        m_dataFileName = dataFiles[idFile];
        m_dataFile.open(m_dataFileName);

        string oneLine;
        string anotherLine;
        int pos1 = 0;

        if (!m_dataFile)
        {
            // Print an error and exit
            cerr << "Uh oh, your file \"" << m_dataFileName << "\" could not be opened for reading!" << endl;
        }else{
            //cout << "File \"" << m_name << "\" is open and ready for reading!" << endl;
            cout << "Start pre-reading file : " << m_dataFileName << endl;
            while(m_dataFile)
            {
                getline(m_dataFile,oneLine);
                if (oneLine[0] == '[')
                {
                    int nPixels = count(oneLine.begin(),oneLine.end(),'[');
                    for (int idP = 0; idP < nPixels ; idP++)
                    {
                        pos1 = oneLine.find("]");
                        anotherLine = oneLine.substr(1,pos1-1);
                        {
                            int pos1 = anotherLine.find(",");
                            int pos2 = anotherLine.rfind(",");
                            m_noisyMask.AddHit(atoi(anotherLine.substr(0,pos1).c_str()),atoi(anotherLine.substr(pos1+1,pos2).c_str()),0);
                        }
                        oneLine = oneLine.substr(pos1+2);
                    }
                }
                else
                    ;
            }
        }
        m_dataFile.close();
    }
    m_noisyMask.SetThreshold(threshold);

    cout << "Threshold set : " << threshold << endl;

    int nMaskedPixels = 512*256 - m_noisyMask.GetNGood();
    cout << "Number of masked pixels : " << nMaskedPixels << endl;

    TCanvas* myCan = new TCanvas("myCan","Results",800,600);
    TH1F* hNCountOnPixel = new TH1F("hNCountOnPixel","How many pixels gave X signals",200,0,200);
    for (int x = 0 ; x < 512 ; x++)
        for (int y = 0 ; y < 256 ; y++)
        {
            if (m_noisyMask.GetNHits(x,y) >= 200)
            {
                hNCountOnPixel->Fill(199);
            }else{
                hNCountOnPixel->Fill(m_noisyMask.GetNHits(x,y));
            }
        }
    hNCountOnPixel->Draw();
    myCan->SetLogy();
    string fileName = m_rootFileName.substr(m_rootFileName.size()-10,5);
    fileName += ".png";
    myCan->SaveAs(fileName.c_str());
    delete hNCountOnPixel;
    delete myCan;
    m_noisyMask.SaveNoisyMask();
}

void FrameHolder::UseCalib(bool useCalib)
{
    for (int idF = 0 ; idF < m_frames.size(); idF++)
    {
        for (int idC = 0 ; idC < m_frames[idF].GetNClusters(); idC++)
        {
            for (int idP = 0 ; idP < m_frames[idF].GetCluster(idC).GetSize(); idP++)
            {
                if (useCalib)
                    m_frames[idF].GetCluster(idC).SetEnergy(idP,m_calib.CountEnergy(m_frames[idF].GetCluster(idC).GetXPos(idP),m_frames[idF].GetCluster(idC).GetYPos(idP),m_frames[idF].GetCluster(idC).GetTot(idP)));
                else
                    m_frames[idF].GetCluster(idC).SetEnergy(idP,m_frames[idF].GetCluster(idC).GetTot(idP));
            }
        }
    }
}

void FrameHolder::ProduceResults(int threshold)
{
    m_rootFile = new TFile(m_rootFileName.c_str(),"Recreate");
    //m_rootFile->mkdir("Raw_Data");
    m_rootFile->mkdir("After_Masking");
    m_rootFile->mkdir("Energy_Cuts");
    m_rootFile->mkdir("Size_Cuts");
    m_rootFile->mkdir("Border_Cuts");
    m_rootFile->mkdir("SSE&DSE_AllSizes");
    m_rootFile->mkdir("SSE&DSE");
    m_rootFile->mkdir("SSE&DSE_BorderCuts");

    if (m_rootFile == NULL) {
        cout << "Unable to open " <<  m_rootFileName << " !!!" << endl;
        exit(1);
    }
    cout << "Output file " <<  m_rootFileName << " opened..." << endl;
    //MaskClusters("After_Masking");
    CreateHist("After_Masking");

    EnergyCuts(6,10000);
    CreateHist("Energy_Cuts");

    SSEandDSE("SSE&DSE_AllSizes");

    SizeCuts(3);
    CreateHist("Size_Cuts");

    SSEandDSE("SSE&DSE");

    m_noisyMask.MaskBorder();
    MaskClusters("Border_Cuts");
    CreateHist("Border_Cuts");
    SSEandDSE("SSE&DSE_BorderCuts");

    m_rootFile->Close();
    delete m_rootFile;
    m_rootFile = 0;

    cout << "Output file properly closed..." << endl;
}

int FrameHolder::CreateHist(string subFolder)
{
    m_rootFile->cd(subFolder.c_str());

    TH1I* hNClusterPerFrame = new TH1I("hNClusterPerFrame","Number of clusters per frame;Number of clusters;Number of frames",100,0,100);
    TH2I* hNCountOnPixelMap = new TH2I("hNCountOnPixelMap","How many times does pixel give a signal",512,0,512,256,0,256);
    TH1F* hNClusterHistory = new TH1F("hNClusterHistory","Time evolution of number of clusters",m_nFrames/10000,0,m_nFrames/10000);
    TH1F* hNClusterHistoryDetail = new TH1F("hNClusterHistoryDetail","Time evolution of number of clusters in detail",m_nFrames/100,0,m_nFrames/100);
    TH1D* hEnergyHistory = new TH1D("hEnergyHistory","Time evolution of energy of clusters",m_nFrames/100000,0,m_nFrames/100000);
    TH1D* hEnergyHistoryDetail = new TH1D("hEnergyHistoryDetail","Time evolution of energy of clusters in detail",m_nFrames/1000,0,m_nFrames/1000);
    TH1F* hNCountOnPixel = new TH1F("hNCountOnPixel","How many pixels gave X signals",100,0,100);
    TH1F* hEnergySpectrum = new TH1F("hEnergySpectrum","Energy spectrum of all clusters",10000,0,10000);
    TH1F* hClusterSize = new TH1F("hClusterSize","Sizes of clusters",100,0,100);
    TH1D* hEnergyOnPixel = new TH1D("hEnergyOnPixel","Energy deposited per pixel",10000,0,1000);

    m_noisyMask.Clear();

    int nClusters = 0;

    for (int idF = 0 ; idF < m_frames.size(); idF++)
    {
        hNClusterPerFrame->Fill(m_frames[idF].GetNGoodClusters());
        hNClusterHistory->Fill(m_frames[idF].GetOID()/10000,m_frames[idF].GetNGoodClusters());
        hNClusterHistoryDetail->Fill(m_frames[idF].GetOID()/100,m_frames[idF].GetNGoodClusters());
        nClusters += m_frames[idF].GetNGoodClusters();
        for (int idC = 0 ; idC < m_frames[idF].GetNClusters(); idC++)
        {
            if (m_frames[idF].GetCluster(idC).GetGood())
            {
                hEnergyHistory->Fill(m_frames[idF].GetOID()/100000,m_frames[idF].GetCluster(idC).GetEnergy());
                hEnergyHistoryDetail->Fill(m_frames[idF].GetOID()/1000,m_frames[idF].GetCluster(idC).GetEnergy());
                hClusterSize->Fill(m_frames[idF].GetCluster(idC).GetSize());
                hEnergySpectrum->Fill(m_frames[idF].GetCluster(idC).GetEnergy());
                for (int idP = 0 ; idP < m_frames[idF].GetCluster(idC).GetSize(); idP++)
                {
                    hNCountOnPixelMap->Fill(m_frames[idF].GetCluster(idC).GetXPos(idP),m_frames[idF].GetCluster(idC).GetYPos(idP));
                    m_noisyMask.AddHit(m_frames[idF].GetCluster(idC).GetXPos(idP),m_frames[idF].GetCluster(idC).GetYPos(idP),m_frames[idF].GetCluster(idC).GetEnergy(idP));
                }
            }
        }
    }
    hNClusterPerFrame->Fill(0.0,m_nFrames-m_frames.size());
    for (int x = 0 ; x < 512; x++)
        for (int y = 0 ; y < 256; y++)
        {
            if (m_noisyMask.GetNHits(x,y) >= 100)
            {
                hNCountOnPixel->Fill(99);
            }else{
                hNCountOnPixel->Fill(m_noisyMask.GetNHits(x,y));
            }
            hEnergyOnPixel->Fill(m_noisyMask.GetEnergy(x,y));
        }
    TH1D* hCountOnPixelRows = hNCountOnPixelMap->ProjectionY("hCountOnPixelRows");
    TH1D* hCountOnPixelColumns = hNCountOnPixelMap->ProjectionX("hCountOnPixelColumns");

    int threshold = 0;
    /*double mean = hNCountOnPixel->GetMean();
    for (int i = 0 ; i < 100 ; i++)
    {
        if (TMath::Poisson(i,mean)*256*512 < 1)
        {
            threshold = i;
            break;
        }
    }*/

    hNCountOnPixelMap->Write();
    hNClusterPerFrame->Write();
    hNClusterHistory->Write();
    hNClusterHistoryDetail->Write();
    hNCountOnPixel->Write();
    hEnergySpectrum->Write();
    hClusterSize->Write();
    hCountOnPixelRows->Write();
    hCountOnPixelColumns->Write();
    hEnergyHistory->Write();
    hEnergyHistoryDetail->Write();
    hEnergyOnPixel->Write();

    hNCountOnPixelMap->Delete();
    hNClusterPerFrame->Delete();
    hNClusterHistory->Delete();
    hNClusterHistoryDetail->Delete();
    hNCountOnPixel->Delete();
    hEnergySpectrum->Delete();
    hClusterSize->Delete();
    hCountOnPixelRows->Delete();
    hCountOnPixelColumns->Delete();
    hEnergyHistory->Delete();
    hEnergyHistoryDetail->Delete();
    hEnergyOnPixel->Delete();

    cout << "Number of frames : " << m_nFrames << " and number of clusters : " << nClusters << endl;
    m_rootFile->cd();
    return threshold;
}

void FrameHolder::MaskClusters(string subFolder)
{
    m_rootFile->cd(subFolder.c_str());

    cout << "Masking : " << endl;

    TH1I* hSizeMaskedClusters = new TH1I("hSizeMaskedClusters","Size of masked clusters",100,0,100);

    for (int idF = 0 ; idF < m_frames.size(); idF++)
    {
        for (int idC = 0 ; idC < m_frames[idF].GetNClusters(); idC++)
        {
            if (m_frames[idF].GetCluster(idC).GetGood())
            {
                for (int idP = 0 ; idP < m_frames[idF].GetCluster(idC).GetSize(); idP++)
                {
                    if (!m_frames[idF].GetCluster(idC).GetGood())
                        break;
                    if(!m_noisyMask.IsGood(m_frames[idF].GetCluster(idC).GetXPos(idP),m_frames[idF].GetCluster(idC).GetYPos(idP)))
                    {
                        m_frames[idF].GetCluster(idC).SetGood(false);
                        hSizeMaskedClusters->Fill(m_frames[idF].GetCluster(idC).GetSize());
                    }
                }
            }

        }
    }

    hSizeMaskedClusters->Write();
    hSizeMaskedClusters->Delete();
    m_noisyMask.SaveNoisyMask();
    m_rootFile->cd();
}

void FrameHolder::EnergyCuts(double lowerLimit, double upperLimit)
{
    cout << "Energy cuts : " << endl;
    for (int idF = 0 ; idF < m_frames.size(); idF++)
    {
        for (int idC = 0 ; idC < m_frames[idF].GetNClusters(); idC++)
        {
            if (m_frames[idF].GetCluster(idC).GetGood())
                if ((m_frames[idF].GetCluster(idC).GetEnergy() < lowerLimit)||(m_frames[idF].GetCluster(idC).GetEnergy() > upperLimit))
                {
                    m_frames[idF].GetCluster(idC).SetGood(false);
                }
        }
    }
}

void FrameHolder::SizeCuts(int howMany)
{
    cout << "Size cuts : " << endl;

    for (int idF = 0 ; idF < m_frames.size(); idF++)
    {
        if (m_frames[idF].GetNGoodClusters() > 2)
        {
            for (int idC = 0 ; idC < m_frames[idF].GetNClusters(); idC++)
            {
                m_frames[idF].GetCluster(idC).SetGood(false);
            }
            continue;
        }
        for (int idC = 0 ; idC < m_frames[idF].GetNClusters(); idC++)
        {
            if (m_frames[idF].GetCluster(idC).GetSize() > howMany)
            {
                m_frames[idF].GetCluster(idC).SetGood(false);
            }
        }
    }
}



void FrameHolder::SSEandDSE(string subfolder)
{
    m_rootFile->cd(subfolder.c_str());

    TH2F* hSSEEnergyScatter = new TH2F("hSSEEnergyScatter", "Energy scatter plot of SSE", 1000,0,1000,1000,0,1000);
    TH2F* hSSEEnergyScatterDist = new TH2F("hSSEEnergyScatterDist", "Energy scatter plot of SSE + Distance cut", 1000,0,1000,1000,0,1000);
    TH2F* hDSEEnergyScatter = new TH2F("hDSEEnergyScatter", "Energy scatter plot of DSE", 1000,0,1000,1000,0,1000);
    TH2F* hSSEEnergyScatterZoom = new TH2F("hSSEEnergyScatterZoom", "ROI Energy scatter plot of SSE", 4,19,23,4,19,23);
    TH2F* hSSEEnergyScatterZoomDist = new TH2F("hSSEEnergyScatterZoomDist", "ROI Energy scatter plot of SSE + Distance cut", 4,19,23,4,19,23);
    TH2F* hDSEEnergyScatterZoom = new TH2F("hDSEEnergyScatterZoom", "ROI Energy scatter plot of DSE", 4,19,23,4,19,23);
    TH1F* hSSEDist = new TH1F("hSSEDist","Distance between SSE in pixels",600,0,600);
    TH1F* hSSEDistDist = new TH1F("hSSEDistDist","Distance between SSE in pixels + Dist cut",600,0,600);
    TH1F* hDSEDist = new TH1F("hDSEDist","Distance between DSE in pixels",600,0,600);
    TH1F* hSSEEne = new TH1F("hSSEEne","Energy spectrum of SSE",1000,0,1000);
    TH1F* hSSEEneDist = new TH1F("hSSEEneDist","Energy spectrum of SSE + Distance cut",1000,0,1000);
    TH1F* hDSEEne = new TH1F("hDSEEne","Energy spectrum of DSE",1000,0,1000);
    TH2F* hSSEPos = new TH2F("hSSEPos","Positions of SSE",512,0,512,256,0,256);
    TH2F* hSSEPosDist = new TH2F("hSSEPosDist","Positions of SSE + Distance cut",512,0,512,256,0,256);
    TH2F* hDSEPos = new TH2F("hDSEPos","Positions of DSE",512,0,512,256,0,256);
    TH1F* hSSESumEnergy = new TH1F("hSSESumEnergy","Summary energy of SSE",1000,0,1000);
    TH1F* hSSESumEnergyDist = new TH1F("hSSESumEnergyDist","Summary energy of SSE + Distance Cut",1000,0,1000);
    TH1F* hDSESumEnergy = new TH1F("hDSESumEnergy","Summary energy of DSE",1000,0,1000);
    TH1F* hSSETime = new TH1F("hSSETime","Time distribution of SSE",100,0,100);
    TH1F* hSSETimeDist = new TH1F("hSSETimeDist","Time distribution of SSE",100,0,100);
    TH1F* hDSETime = new TH1F("hDSETime","Time distribution of DSE",100,0,100);

    int nSSE = 0;
    int nSSEDistCut = 0;
    int nDSE = 0;

    for (int idF = 0 ; idF < m_frames.size() ; idF++)
    {
        if (m_frames[idF].GetNGoodClusters() == 2)
        {
            Cluster first = m_frames[idF].GetFirstGood();
            Cluster second = m_frames[idF].GetSecondGood();
            if (((first.GetXPos(0) < 256)&&(second.GetXPos(0) < 256))||((first.GetXPos(0) > 255)&&(second.GetXPos(0) > 255)))
            {
                if (CountDist(first,second) > 14)
                {
                    nSSEDistCut++;
                    hSSEEnergyScatterDist->Fill(first.GetEnergy(),second.GetEnergy());
                    hSSEEnergyScatterZoomDist->Fill(first.GetEnergy(),second.GetEnergy());
                    hSSEDistDist->Fill(CountDist(first,second));
                    hSSEEneDist->Fill(first.GetEnergy());
                    hSSEEneDist->Fill(second.GetEnergy());
                    hSSEPosDist->Fill(first.GetXPos(),first.GetYPos());
                    hSSEPosDist->Fill(second.GetXPos(),second.GetYPos());
                    hSSESumEnergyDist->Fill(first.GetEnergy()+second.GetEnergy());
                    hSSETimeDist->Fill((double)idF/m_frames.size()*100.0);
                }
                nSSE++;
                hSSEEnergyScatter->Fill(first.GetEnergy(),second.GetEnergy());
                hSSEEnergyScatterZoom->Fill(first.GetEnergy(),second.GetEnergy());
                hSSEDist->Fill(CountDist(first,second));
                hSSEEne->Fill(first.GetEnergy());
                hSSEEne->Fill(second.GetEnergy());
                hSSEPos->Fill(first.GetXPos(),first.GetYPos());
                hSSEPos->Fill(second.GetXPos(),second.GetYPos());
                hSSESumEnergy->Fill(first.GetEnergy()+second.GetEnergy());
                hSSETime->Fill((double)idF/m_frames.size()*100.0);
            }
            else
            {
                nDSE++;
                hDSEEnergyScatter->Fill(first.GetEnergy(),second.GetEnergy());
                hDSEEnergyScatterZoom->Fill(first.GetEnergy(),second.GetEnergy());
                hDSEDist->Fill(CountDist(first,second));
                hDSEEne->Fill(first.GetEnergy());
                hDSEEne->Fill(second.GetEnergy());
                hDSEPos->Fill(first.GetXPos(),first.GetYPos());
                hDSEPos->Fill(second.GetXPos(),second.GetYPos());
                hDSESumEnergy->Fill(first.GetEnergy()+second.GetEnergy());
                hDSETime->Fill((double)idF/m_frames.size()*100.0);
            }
        }
    }

    hSSEEnergyScatter->Write();
    hSSEEnergyScatterDist->Write();
    hDSEEnergyScatter->Write();
    hSSEEnergyScatterZoom->Write();
    hSSEEnergyScatterZoomDist->Write();
    hDSEEnergyScatterZoom->Write();
    hSSEDist->Write();
    hSSEDistDist->Write();
    hDSEDist->Write();
    hSSEEne->Write();
    hSSEEneDist->Write();
    hDSEEne->Write();
    hSSEPos->Write();
    hSSEPosDist->Write();
    hDSEPos->Write();
    hSSESumEnergy->Write();
    hSSESumEnergyDist->Write();
    hDSESumEnergy->Write();
    hSSETime->Write();
    hSSETimeDist->Write();
    hDSETime->Write();

    hSSEEnergyScatter->Delete();
    hSSEEnergyScatterDist->Delete();
    hDSEEnergyScatter->Delete();
    hSSEEnergyScatterZoom->Delete();
    hSSEEnergyScatterZoomDist->Delete();
    hDSEEnergyScatterZoom->Delete();
    hSSEDist->Delete();
    hSSEDistDist->Delete();
    hDSEDist->Delete();
    hSSEEne->Delete();
    hSSEEneDist->Delete();
    hDSEEne->Delete();
    hSSEPos->Delete();
    hSSEPosDist->Delete();
    hDSEPos->Delete();
    hSSESumEnergy->Delete();
    hSSESumEnergyDist->Delete();
    hDSESumEnergy->Delete();
    hSSETime->Delete();
    hSSETimeDist->Delete();
    hDSETime->Delete();

    cout << "Number of SSE : " << nSSE << " (" << nSSEDistCut << ")" << " number of DSE : " << nDSE << endl;
    m_rootFile->cd();
}
