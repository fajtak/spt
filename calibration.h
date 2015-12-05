#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include "math.h"

using namespace std;

class Calibration
{
    public:
        void ReadCalibration(string folderName);
        double CountEnergy(int x,int y, int tot);
    private:
        double m_a[512][256];
        double m_b[512][256];
        double m_c[512][256];
        double m_t[512][256];
};

#endif
