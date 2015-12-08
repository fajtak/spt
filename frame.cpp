#include "frame.h"

using namespace std;

Frame::Frame(void)
{
    m_nClusters = 0;
}

void Frame::AddCluster(string oneLine, NoisyMask& myNoisyMask)
{
    int nPixels = count(oneLine.begin(),oneLine.end(),'[');

    int pos1 = 0;
    double xPos = 0;
    double yPos = 0;

    m_clusters.push_back(Cluster());
    m_clusters.back().SetSize(nPixels);
    m_clusters.back().SetGood(true);

    for (int idP = 0; idP < nPixels ; idP++)
    {
        pos1 = oneLine.find("]");
        m_clusters.back().AddPixel(oneLine.substr(1,pos1-1));
        oneLine = oneLine.substr(pos1+2);
        xPos += m_clusters.back().GetXPos(idP);
        yPos += m_clusters.back().GetYPos(idP);
        if (!myNoisyMask.IsGood(m_clusters.back().GetXPos(idP),m_clusters.back().GetYPos(idP)))
        {
            m_clusters.pop_back();
            return;
        }
    }
    xPos /= m_clusters.back().GetSize();
    yPos /= m_clusters.back().GetSize();
    m_clusters.back().SetXPos(xPos);
    m_clusters.back().SetYPos(yPos);
    m_nClusters++;
}

int Frame::GetNGoodClusters(void)
{
    int nGoodClusters = 0;
    for (int idC = 0 ; idC < m_clusters.size(); idC++)
    {
        if (m_clusters[idC].GetGood())
            nGoodClusters++;
    }
    return nGoodClusters;
}

Cluster & Frame::GetFirstGood(void)
{
    for (int idC = 0 ; idC < m_clusters.size() ; idC++)
    {
        if (m_clusters[idC].GetGood())
            return m_clusters[idC];
    }
}

Cluster & Frame::GetSecondGood(void)
{
    bool first = false;
    for (int idC = 0 ; idC < m_clusters.size() ; idC++)
    {
        if (m_clusters[idC].GetGood())
        {
            if (first)
                return m_clusters[idC];
            first = true;
        }
    }
}
