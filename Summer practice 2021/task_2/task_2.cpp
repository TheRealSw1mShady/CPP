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
    QVector <float> str_float;          // Вектор строки с числами
    QVector <float> vector_max;         // Вектор с максимальными значениями из каждой строки
    QVector <float> vector_max_vel;     // Вектор с индексами максимальных значений каждой строки
    ifstream fin;
    fin.open("C:\\Users\\Dmitriy\\Desktop\\zad2\\MDV.txt");

    int i = 0;
    string str;                         // Строка
    string short_str;                   // Строка для записи одного числа
    int pos;
    float max = 0;                          // Переменная под максимально число в строке
    float max_channel;                  // Переменная под максимальное число в векторе vector_max
    int len;
    double sum = 0;
    int max_vel;

    if (fin.is_open())
    {
        cout << "File opened!" << endl;
        i = 0;

        // Чтение файла построчно:

        while (!fin.eof())
        {
            str = "";
            getline(fin, str);

            // Разделение строки на числа и запись в вектор

            int j;
            int len = str.length();
            for (j = 0; j < len; j += 1)
            {
                pos = str.find("	");
                if (pos == -1)
                    break;
                while (j != pos)
                {
                    short_str += str[j];
                    j += 1;
                }
                j -= 1;
                str_float.append(stof(short_str));
                short_str = "";
                str.erase(pos, 1);
            }

            //qDebug() << str_float; cout << endl;

            i += 1;

            if (*max_element(str_float.begin(), str_float.end()) > max)
                max = *max_element(str_float.begin(), str_float.end()); // Максимальный элемент в строке
//            max = *max_element(str_float.begin(), str_float.end());
//            vector_max.append(max);

            // Поиск индекса для каждого максимума

            for (j = 0; j < str_float.length(); j += 1)
            {
                if (str_float[j] == max)
                    max_vel = j;
                    //vector_max_vel.append(j);
            }

            short_str = "";
            str_float.clear();
        }

        cout << max << endl;
        cout << "Velocity channel of MAX = " << max_vel << endl;

//        cout << endl;
//        qDebug() << vector_max; cout << endl;
//        cout << endl;
//        qDebug() << vector_max_vel; cout << endl;

//        max_channel = *max_element(vector_max.begin(), vector_max.end());   // Максимальный элемент матрицы
//        len = vector_max.length();

//        // Среднее арифметическое всех максимумов

//        for (i = 0; i < len; i += 1)
//            sum += vector_max[i];
//        float SA = sum/i;

//        cout << "SA = " << SA << endl;
//        cout << "Porog = " << SA - 0.045*max_channel << endl;
//        cout << endl;

        // Вывод каналов

        fin.close();
        fin.open("C:\\Users\\Dmitriy\\Desktop\\zad2\\MDV.txt");

        i = 0;

        while (!fin.eof())
        {
            str = "";
            getline(fin, str);

            // Разделение строки на числа и запись в вектор

            int j;
            int len = str.length();
            for (j = 0; j < len; j += 1)
            {
                pos = str.find("	");
                if (pos == -1)
                    break;
                while (j != pos)
                {
                    short_str += str[j];
                    j += 1;
                }
                j -= 1;
                str_float.append(stof(short_str));
                short_str = "";
                str.erase(pos, 1);
            }

            //qDebug() << str_float; cout << endl;


            len = str_float.length();

            float a = 0;
            for (j = 0; j < len; j += 1)
            {
                max_channel = *max_element(str_float.begin(), str_float.end());
                if (j == 0)
                    a = str_float[j];
                else
                    if ((max_channel >= 0.7*max) && (a < str_float[j]) && (str_float[j] < max_channel))
                    {
                        str_float[j] = 0;
                    }

                    if ((max_channel >= 0.7*max) && (str_float[j] == max_channel))
                    {
                            cout << "Range channel = " << i << ", Velocity channel = " << j << ", Amplitude = " << str_float[j] << endl;
                            str_float[j] = 0;
                            if (j != len - 1)
                                str_float[j+1] = 0;
                            j = 0;
                    }
                    if ((max_channel >= 0.7*max) && (max_channel > a >  str_float[j]))
                            str_float[j] = 0;
                    a = str_float[j];

            }

            short_str = "";
            str_float.clear();
            i += 1;
        }
    }
    else
        cout << "File not found." << endl;

    fin.close();

    return 0;
}
