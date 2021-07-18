#include <iostream>
#include <unistd.h>
#include <iomanip>
#include <fstream>
#include <QVector>
#include <QDebug>
#include <math.h>

#define C (3*pow(10,8))
#define _USE_MATH_DEFINES // для C++
#include <cmath>

using namespace std;

int main()
{
    QVector <float> str_float;
    ifstream fin;
    fin.open("C:\\Users\\Dmitriy\\Desktop\\zad2\\MDV.txt");

    int i = 0;
    string str;
    string short_str;
    int pos;

    if (fin.is_open())
    {
        cout << "File opened!" << endl;
        i = 0;
        while (i < 1)//(!fin.eof())
        {
            str = "";
            getline(fin, str);
            while (true)
            {
                pos = str.find("	");
                if (pos == -1)
                    break;
                else
                    str.erase(pos, 1);
            }
            cout << str << endl;

            int j;
            int k = 0;
            int len = str.length();
            for (j = 0; j < len; j += 1)
            {
                short_str += str[j];
                if ((k == 7) && (j == 7))
                {
                    cout << short_str << endl;
                    str_float.append(stof(short_str));
                    short_str = "";
                    k = 0;
                }
                if ((k == 8) && (j != 7))
                {
                    cout << short_str << endl;
                    str_float.append(stof(short_str));
                    short_str = "";
                    k = 0;
                }
                k += 1;
            }
            qDebug() << str_float;
            //qDebug() << qSetRealNumberPrecision(9) << str_float;
            cout << str_float[0];
            i += 1;
        }
    }
    else
        cout << "File not found." << endl;
    cout << "...EOF." << endl;

    fin.close();
    cout << i << endl;
    //cout << str[7];

    return 0;
}
