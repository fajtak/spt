#include "cluster.h"

using namespace std;

void Cluster::AddPixel(string pixelLine)
{
    m_pixels.push_back(Pixel());

    int pos1 = pixelLine.find(",");
    int pos2 = pixelLine.rfind(",");

    m_pixels.back().xPos = atoi(pixelLine.substr(0,pos1).c_str());
    m_pixels.back().yPos = atoi(pixelLine.substr(pos1+1,pos2).c_str());
    m_pixels.back().tot = atoi(pixelLine.substr(pos2+1,pixelLine.size()-1).c_str());
}

double Cluster::GetEnergy(void)
{
    double energy = 0;
    for (int idP = 0 ; idP < m_size ; idP++)
    {
        energy += m_pixels[idP].energy;
    }
    return energy;
}
