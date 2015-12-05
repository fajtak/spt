# include "noisyMask.h"

using namespace std;

NoisyMask::NoisyMask(void)
{
    for (int x = 0 ; x < cxDim ; x++)
        for (int y = 0 ; y < cyDim ; y++)
        {
            m_pixels[x][y] = 0;
            m_pixelsBool[x][y] = true;
            m_pixelsEnergy[x][y] = 0;
        }
    m_threshold = 10000000;
}

void NoisyMask::AddHit(int x, int y, double energy)
{
    m_pixels[x][y]++;
    m_pixelsEnergy[x][y] += energy;
}

void NoisyMask::Clear(void)
{
    for (int x = 0 ; x < cxDim ; x++)
        for (int y = 0 ; y < cyDim ; y++)
        {
            m_pixels[x][y] = 0;
            m_pixelsEnergy[x][y] = 0;
        }
}

void NoisyMask::MaskBorder(void)
{
    for (int x = 0 ; x < cxDim ; x++)
        for (int y = 0 ; y < cyDim ; y++)
        {
            if ((x == 0)||(y == 0)||(x == 255)||(y == 255)||(x == 256)||(x == 511))
                m_pixelsBool[x][y] = false;
        }
}

int NoisyMask::Update(void)
{
    m_nGood = 0;
    for (int x = 0 ; x < cxDim ; x++)
        for (int y = 0 ; y < cyDim ; y++)
        {
            if (m_pixels[x][y] >= m_threshold)
                m_pixelsBool[x][y] = false;
            else
                m_nGood++;
        }
    return m_nGood;
}

void NoisyMask::SaveNoisyMask(void)
{
    ofstream noisyMaskFile;
    noisyMaskFile.open(m_noisyMaskFileName);

    for (int y = 0 ; y < cyDim ; y++)
    {

        for (int x = 0 ; x < cxDim ; x++)
        {
            noisyMaskFile << m_pixelsBool[x][y] << " ";
        }
        noisyMaskFile << endl;
    }

    noisyMaskFile.close();
}

void NoisyMask::LoadNoisyMask(void)
{
    ifstream noisyMaskFile;
    noisyMaskFile.open(m_noisyMaskFileName);

    for (int y = 0 ; y < cyDim ; y++)
    {

        for (int x = 0 ; x < cxDim ; x++)
        {
            noisyMaskFile >> m_pixelsBool[x][y];
        }
    }

    noisyMaskFile.close();
}
