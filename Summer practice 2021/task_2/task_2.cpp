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

    if (fin.is_open())
    {
        cout << "File opened!" << endl; cout << endl;
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

            i += 1;

            if (*max_element(str_float.begin(), str_float.end()) > max)
                max = *max_element(str_float.begin(), str_float.end()); // Максимальный элемент в строке

            short_str = "";
            str_float.clear();
        }


        fin.close();
        fin.open("C:\\Users\\Dmitriy\\Desktop\\zad2\\MDV.txt");

        i = 0;
        float porog = 0.7 * max;
        QVector <short> range;
        QVector <short> velocity;
        QVector <float> amplitude;

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

            len = str_float.length();

            // Цикл для отбора пиков в строке

            for (j = 0; j < len; j += 1)
            {
                if (j + 1 < len)
                {
                    if (str_float[j] > porog)
                    {
                        range.append(i);
                        velocity.append(j);
                        amplitude.append(str_float[j]);
                        if ((str_float[j+1] > porog) && (str_float[j+1] < str_float[j]))
                        {
                            str_float[j+1] = 0.0;
                        }
                    }
                }
            }
            short_str = "";
            str_float.clear();
            i += 1;
        }
        short len = velocity.length();

        //qDebug() << range;
        //qDebug() << velocity;
        //qDebug() << amplitude;

        // Цикл для отсеивания "целей" из соседних каналов с меньшей амплитудой

        i = 0;
        short j;
        while (i < len)
        {
            if (i + 1 == len)
                break;
            j = i + 1;
            while (j < len)
            {
                if ((velocity[i] == velocity[j]) || (velocity[i] == velocity[j] - 1)|| (velocity[i] == velocity[j] + 1))
                {
                    if ((range[i] == range[j]) || (range[i] == range[j] - 1)|| (range[i] == range[j] + 1))
                    {
                        if (amplitude[i] > amplitude[j])
                        {
                            velocity.remove(j);
                            range.remove(j);
                            amplitude.remove(j);
                            i = -1;
                            break;
                        }
                        else
                        {
                            velocity.remove(i);
                            range.remove(i);
                            amplitude.remove(i);
                            i = -1;
                            break;
                        }
                    }
                }
                j += 1;
            }
            len = velocity.length();
            i += 1;
        }

        //cout << endl;
        for (i = 0; i < len; i += 1)
        {
            cout << i << ") Range channel = " << range[i] << ", Velocity channel = " << velocity[i] << endl;
        }
        //cout << endl;
        //qDebug() << range;
        //qDebug() << velocity;
        //qDebug() << amplitude;

    }
    else
        cout << "File not found." << endl;

    fin.close();

    return 0;
}
