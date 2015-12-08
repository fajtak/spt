#ifndef NOISYMASK_H
#define NOISYMASK_H

#include <iostream>
#include <fstream>
#include <string>

const int cxDim = 512;
const int cyDim = 256;

using namespace std;

class NoisyMask
{
    public:
        NoisyMask(void);

        void AddHit(int x, int y, double energy);

        int Update();

        void MaskBorder();

        int SetThreshold(int thres){m_threshold = thres;return Update();};
        void SetName(string name){m_noisyMaskFileName = name;};

        int GetNGood(void){return m_nGood;};
        bool IsGood(int x, int y){return m_pixelsBool[x][y];};
        int GetNHits(int x, int y){return m_pixels[x][y];};
        double GetEnergy (int x, int y){return m_pixelsEnergy[x][y];};
        void CoutNoisyMask (void);
        void CoutNoisyMaskRow (int myRow);

        void SaveNoisyMask(void);
        void LoadNoisyMask(void);

        void Clear(void);

    private:
        int m_pixels[cxDim][cyDim];
        bool m_pixelsBool[cxDim][cyDim];
        double m_pixelsEnergy[cxDim][cyDim];
        int m_threshold;
        int m_nGood;
        string m_noisyMaskFileName;
};


#endif
