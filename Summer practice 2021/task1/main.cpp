#include <iostream>
#include <unistd.h>
#include <QVector>
#include <QDebug>
#include <math.h>

#define C 3*pow(10,8)
#define _USE_MATH_DEFINES // для C++
#include <cmath>

QVector<float> spisok_Tp;
QVector<float> spisok_ti;
QVector<double> spisok_Ampl;
int A = 1;
float dt = 0.1;
float Fpr = 63000000;
float Fd = 84000000;
float lambda = 0.03;
int s = 10;

using namespace std;

void parametrs(float Fmin = 40000, float Fmax = 200000)
{
    for (int F = Fmin; F < Fmax; F += 500)
    {
        float Tp = 1.0/F;           // Период повторения импульсов
        spisok_Tp.append(Tp);
        float ti = Tp/s;            // Длительность импульса
        spisok_ti.append(ti);
     }
    cout << "Done!" << endl;
}


int main()
{
    int R;  // м
    cout << "Enter start distance: "; cin >> R;
    int R1;

    short V; // м/с
    cout << "Enter start velocity: "; cin >> V;

    QVector<int> spisok_R;

    // Цикл расчета дальности каждый дискрет времени dt
    for (R1 = R; R1 < 10000; R1 = R1 + V*dt)
    {
        spisok_R.append(R1);
        sleep(dt);
    }

    float F = 40005;                             //Частота следования импульсов
    float w = 2*M_PI*dt*(Fpr-(-2*V/lambda));     //Циклическая частота заполнения импульса
    float x1 = 0;                                //x1, y1, x2, y2 - координаты точек соответственно в импульсе, без него (в пустом пространстве)
    float y1 = 0;
    float x2 = 0;
    float y2 = 0;
    float Tost1 = 0;            //Время между концом импульса и последней дискретой этого импульса
    float Tost2 = 0;            //Время между началом импульса и последней дискретой пустого пространства

    float j = 0;                //j = Tp - описывает одну итерацию цикла (импульс + пространство)

    float Tp = 1.0/F;           // Период повторения импульсов
    float ti = Tp/s;            // Длительность импульса

    while (j <= 5*Tp+ti){
        float i = 0.0;                          //Счетчик для шага дискретной последовательности i/Fd, i - для импульса, ii - для пустого пр.
        while (x1 < ti + j){
            x1 = j + i/Fd + (1.0/Fd - Tost2);   //Расчет координаты Х дискреты в импульсе; (1.0/Fd - Tost2) - учет нахождения предыдущей дискреты
            y1 = sin(w*x1);                     //Расчет амплитуды Y дискреты в импульсе
            if (x1 < ti + j){                   //Если координата Х еще лежит в импулье, то добавляем в вектор амплитуд
                i = i + 1.0;
                Tost1 = ti + j - x1;            //Оставшийся интервал между концом импульса и последней его дискретой
                spisok_Ampl.append(y1);
            }
        }
        Tost2 = 0.0;

        float ii = 0.0;
        while (x2 <= Tp + j){
            x2 = ti + j + ii/Fd + (1.0/Fd - Tost1);     //Расчет координаы Х дискреты в пустом пространстве; (1.0/Fd - Tost1) - учет предыдущей дис.
            y2 = 0;                                     //Импульса нет, следовательно амплитуда = 0
            if (x2 < Tp + j){
                Tost2 = Tp + j + Tost1 - x2;            //Оставшийся интервал между началом импульса и последней дискретой перед ним
                ii = ii + 1.0;
                spisok_Ampl.append(y2);
            }
        }
        Tost1 = 0.0;
        j = j + Tp;

    }
    qDebug() << spisok_Ampl;                //Вывод вектора амплитуд

    parametrs(V);
    return 0;
}
