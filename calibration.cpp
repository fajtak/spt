#include "calibration.h"

using namespace std;

void Calibration::ReadCalibration(string folderName)
{
    string aFileName = folderName + "a.txt";
    string bFileName = folderName + "b.txt";
    string cFileName = folderName + "c.txt";
    string tFileName = folderName + "t.txt";

    ifstream aFile;
    aFile.open(aFileName);

    int x = 0;
    int y = 0;
    double value;

    if (!aFile)
    {
        // Print an error and exit
        cerr << "Uh oh, your file \"" << aFileName << "\" could not be opened for reading!" << endl;
    }else{
        //cout << "File \"" << m_name << "\" is open and ready for reading!" << endl;
        while(aFile)
        {
            aFile >> value;
            if (!aFile) break;
            m_a[x][y] = value;
            x++;
            if (x == 512)
            {
                x = 0;
                y++;
            }
        }
    }

    aFile.close();

    ifstream bFile;
    bFile.open(bFileName);

    x = 0;
    y = 0;

    if (!bFile)
    {
        // Print an error and exit
        cerr << "Uh oh, your file \"" << bFileName << "\" could not be opened for reading!" << endl;
    }else{
        //cout << "File \"" << m_name << "\" is open and ready for reading!" << endl;
        while(bFile)
        {
            bFile >> value;
            if (!bFile) break;
            m_b[x][y] = value;
            x++;
            if (x == 512)
            {
                x = 0;
                y++;
            }
        }
    }

    bFile.close();

    ifstream cFile;
    cFile.open(cFileName);

    x = 0;
    y = 0;

    if (!cFile)
    {
        // Print an error and exit
        cerr << "Uh oh, your file \"" << cFileName << "\" could not be opened for reading!" << endl;
    }else{
        //cout << "File \"" << m_name << "\" is open and ready for reading!" << endl;
        while(cFile)
        {
            cFile >> value;
            if (!cFile) break;
            m_c[x][y] = value;
            x++;
            if (x == 512)
            {
                x = 0;
                y++;
            }
        }
    }

    cFile.close();

    ifstream tFile;
    tFile.open(tFileName);

    x = 0;
    y = 0;

    if (!tFile)
    {
        // Print an error and exit
        cerr << "Uh oh, your file \"" << tFileName << "\" could not be opened for reading!" << endl;
    }else{
        //cout << "File \"" << m_name << "\" is open and ready for reading!" << endl;
        while(tFile)
        {
            tFile >> value;
            if (!tFile) break;
            m_t[x][y] = value;
            x++;
            if (x == 512)
            {
                x = 0;
                y++;
            }
        }
    }

    tFile.close();
}

double Calibration::CountEnergy(int x, int y, int tot)
{
    double energy = (m_t[x][y]*m_a[x][y] + tot - m_b[x][y] + sqrt(pow(m_b[x][y] + m_t[x][y]*m_a[x][y] - tot,2) + 4*m_a[x][y]*m_c[x][y]))/(2*m_a[x][y]);
    if (std::isnan(energy)) return 0;
    return energy;
}

