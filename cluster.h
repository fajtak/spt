#ifndef CLUSTER_H
#define CLUSTER_H

#include <vector>
#include <string>
#include <iostream>

using namespace std;

struct Pixel
{
	int xPos;
	int yPos;
	int tot;
	double energy;
};

class Cluster
{
	public :
		void AddPixel(string pixelLine);
		Pixel & GetPixel(int which){return m_pixels[which];};

		void SetSize(int size){m_size = size;};
        int GetSize(void){return m_size;};
        double GetEnergy(void);
		double GetXPos(void){return m_xPos;};
		double GetYPos(void){return m_yPos;};
		void SetXPos(double xPos){m_xPos = xPos;};
		void SetYPos(double yPos){m_yPos = yPos;};
        void SetGood(bool isGood){m_good = isGood;};
		bool GetGood(void){return m_good;};

		void ShrinkSize(int which){m_size--;m_pixels.erase(m_pixels.begin() + which);};

		int GetXPos(int which){return m_pixels[which].xPos;};
		int GetYPos(int which){return m_pixels[which].yPos;};
		int GetTot(int which){return m_pixels[which].tot;};
		double GetEnergy(int which){return m_pixels[which].energy;};
		void SetEnergy(int which,double energy){m_pixels[which].energy = energy;};

	private :
		vector<Pixel> m_pixels;
		int m_size;
		double m_xPos;
		double m_yPos;
		bool m_good;
};

#endif
