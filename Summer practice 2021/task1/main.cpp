#include <iostream>
#include <unistd.h>
#include <QVector>
#include <QDebug>
#include <cmath>

#define C 3*pow(10,8)

using namespace std;

void parametrs(float Fmin = 40000, float Fmax = 200000, int s = 10)
{

    // N - количество импульсов, t - длина импульса, Tp - период повторения

    for (int F = Fmin; F < Fmax; F += 500)
    {
        float Tp = 1.0/F;           // Период следования импульсов
        float ti = Tp/s;            // Длительность импульса

        char str[200];
        sprintf(str, "At frequency = %d repeat period is %.10f sec of pulses with duration = %.10f sec", F, Tp, ti);
        cout << str << endl;
     }
}


int main()
{
    float lambda = 0.03;        //лямбда заполнения импульса
    float Fimp = C/lambda;      //частота заполнения импульса


    int R;  // м
    cout << "Enter start distance: "; cin >> R;
    int R1;
    float dt = 0.1;

    short V; // м/с
    cout << "Enter start velocity: "; cin >> V;

    QVector<int> spisok_R;


    // Цикл расчета дальности каждый дискрет времени dt

    for (R1 = R; R1 < 10000;)
    {
        int Rone = R1;

        spisok_R.append(R1);
        cout << "Distance = " << R1 << " m" << endl;

        R1 = R1 + V*dt;

        int Rtwo = R1;
        float Fsdvig = -2*((Rtwo-Rone)/dt)/lambda;        //доплеровский сдвиг
        float Lnew = C/(Fimp + Fsdvig);                   //новая длина волны заполнения импульса

        char str1[200];
        sprintf(str1, "New wavelength = %.10f m. ", Lnew);
        cout << str1;

        sleep(dt);
    }

    cout << endl; cout << "Pause 5 sec" << endl;
    sleep(5);
    parametrs();

    // Вывод списка с дальностью + вывод дальностей из списка
    cout << endl;
    qDebug() << spisok_R;
    int i;
    cout << endl;

    for (i = 1; i < spisok_R.length(); i++)
    {
        cout << spisok_R[i] << "; ";
    }

    return 0;
}
