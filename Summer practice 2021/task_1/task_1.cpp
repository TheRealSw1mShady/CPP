#include <iostream>
#include <unistd.h>
#include <QVector>
#include <QDebug>
#include <math.h>

#define C (3*pow(10,8))
#define _USE_MATH_DEFINES // для C++
#include <cmath>

QVector<float> spisok_Tp;
QVector<float> spisok_ti;
QVector<double> spisok_Ampl;
QVector<int> spisok_n;
QVector<int> spisok_N;
QVector<float> spisok_F;

int A = 1;
float dt = 0.1;
float Fpr = 63000000;
float Fd = 84000000;
float lambda = 0.03;
int s = 10;

using namespace std;

void parametrs(float Fmin = 40000, float Fmax = 200000)
{

    float delta = 1/Fd;                                   //длительность дискрета
    float Tpmax = 1.0/Fmin;                               // Период повторения импульсов
    float Tpmin = 1.0/Fmax;                               // Период повторения импульсов

    int Nmin = Tpmin/delta;                               //количество отсчетов на свободное пространство
    int Nmax = Tpmax/delta;                               //количество отсчетов на свободное пространство

    for (int ot = Nmax; ot > Nmin; ot -= 20)
    {
        float Tp = ot*delta;                              // Период повторения импульсов
        spisok_Tp.append(Tp);
        float ti = Tp/s;                                  // Длительность импульса
        spisok_ti.append(ti);
        float F = 1.0/Tp;
        spisok_F.append(F);

        int N = ot;                                       //количество отсчетов на свободное пространство
        int n = N/s;                                      //количество отсчетов в импульсе

        spisok_n.append(n);
        spisok_N.append(N);

     }
    cout << "Parametric function is completed!" << endl;
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
    for (R1 = R; R1 < 30000; R1 = R1 + V*dt)
    {
        spisok_R.append(R1);
        sleep(dt);
    }

    parametrs();

    int index = 10;
    float F = spisok_F[index];
    float Fdop = fabs(-2 * V/lambda);

    if (Fdop >= F)
    {
        while (Fdop >= F)
        {
            index = index + 1;
            F = spisok_F[index];
        }
    }

    float Tp = spisok_Tp[index];                            // Период повторения импульсов
    float ti = spisok_ti[index];                            // Длительность импульса

    //Цикл расчета задержки-------------------------

    int i;
    float t;
    QVector<float> tzad;

    for (i = 0; i < spisok_R.length(); i = i + 1)
    {
        t = 2*spisok_R[i]/C;
        tzad.append(t);
    }

    cout << endl;
    qDebug() << tzad;
    cout << endl;
    //----------------------------------------------

    float w = 2*M_PI*dt*(Fpr-(-2*V/lambda));            //Циклическая частота заполнения импульса

    float constTzad = tzad[50];
    float constTzadNew = 0;
    // Отделение целого числа импульсов
    if (constTzad >= Tp)
    {
        constTzadNew = fmod(constTzad, Tp);
    }

    float t1 = 0;                                       //x1, y1, x2, y2 - координаты точек соответственно в импульсе, без него (в пустом пространстве)
    float y1 = 0;
    float t2 = 0;
    float y2 = 0;

    float j = constTzadNew;                             //j = Tp - описывает одну итерацию цикла (импульс + пространство)
    float k = constTzadNew;

    //Цикл задержки
    float ost = 0;
    if (k == constTzadNew)
    {
        float x = 0;
        int i = 0;
        while (x < k)
        {
            x = i/Fd;
            short y = 0;
            if (x < k)
            {
                spisok_Ampl.append(y);
                i = i + 1;
                ost = k - x;
            }
        }
        k = x;
        j = x;
    }

    //Цикл импульсов
    float fi = 0;

    while (j < 15*Tp){
        float i = 0.0;                                  //Счетчик для шага дискретной последовательности i/Fd, i - для импульса, ii - для пустого пр.
        while (t1 < ti + k){
            t1 = k + i/Fd;                              //Расчет координаты Х дискреты в импульсе;
            if (i == 0)
                fi = fmod((w*t1), (2*M_PI));
            y1 = sin(w*(t1+ost) - fi);                             //Расчет амплитуды Y дискреты в импульсе
            if (t1 < ti + k){                           //Если координата Х еще лежит в импулье, то добавляем в вектор амплитуд
                i = i + 1.0;
                spisok_Ampl.append(y1);
            }
            else break;
        }

        float ii = 0.0;
        while (t2 <= Tp + k){
            t2 = ti + k + ii/Fd;                        //Расчет координаты Х дискреты в пустом пространстве
            y2 = 0;                                     //Импульса нет, следовательно амплитуда = 0
            if (t2 < Tp + j){
                ii = ii + 1.0;
                spisok_Ampl.append(y2);
            }
            else break;
        }
        j = j + Tp;
        k = k + Tp;
    }

    qDebug() << spisok_Ampl;
    cout << endl; cout << "First group of pulses has processed!" << endl; cout << endl;

    constTzad = tzad[51];

    //Перерасчет параметров пачки импульсов

    for (i = index; spisok_Tp.length(); i = i + 1)
    {
        Tp = spisok_Tp[i];
        ti = spisok_ti[i];
        float coeff = fmod(constTzad, Tp);
        if (((Tp/2 - ti/2) <= coeff) && (coeff <= (Tp/2 + ti/2)))
        {
            cout << "OD = " << constTzad << ", ND = " << coeff << ", Tp = " << Tp << ", ti = " << ti << endl;
            break;
        }
    }

    QVector<float> spisok_Ampl_1;

    // Отделение целого числа импульсов
    if (constTzad >= Tp)
        constTzadNew = fmod(constTzad, Tp);

    t1 = 0;                                       //x1, y1, x2, y2 - координаты точек соответственно в импульсе, без него (в пустом пространстве)
    t2 = 0;
    j = constTzadNew;                             //j = Tp - описывает одну итерацию цикла (импульс + пространство)
    k = constTzadNew;

    //Цикл задержки
    ost = 0;
    if (k == constTzadNew)
    {
        float x = 0;
        int i = 0;
        while (x < k)
        {
            x = i/Fd;
            short y = 0;
            if (x < k)
            {
                spisok_Ampl_1.append(y);
                i = i + 1;
                ost = k - x;
            }
        }
        k = x;
        j = x;
    }

    //Цикл импульсов
    fi = 0;

    while (j < 15*Tp){
        float i = 0.0;                                  //Счетчик для шага дискретной последовательности i/Fd, i - для импульса, ii - для пустого пр.
        while (t1 < ti + k){
            t1 = k + i/Fd;                              //Расчет координаты Х дискреты в импульсе;
            if (i == 0)
                fi = fmod((w*t1), (2*M_PI));
            y1 = sin(w*(t1+ost) - fi);                             //Расчет амплитуды Y дискреты в импульсе
            if (t1 < ti + k){                           //Если координата Х еще лежит в импулье, то добавляем в вектор амплитуд
                i = i + 1.0;
                spisok_Ampl_1.append(y1);
            }
            else break;
        }

        float ii = 0.0;
        while (t2 <= Tp + k){
            t2 = ti + k + ii/Fd;                        //Расчет координаты Х дискреты в пустом пространстве
            y2 = 0;                                     //Импульса нет, следовательно амплитуда = 0
            if (t2 < Tp + j){
                ii = ii + 1.0;
                spisok_Ampl_1.append(y2);
            }
            else break;
        }
        j = j + Tp;
        k = k + Tp;
    }

    qDebug() << spisok_Ampl_1;
    cout << endl; cout << "Second group of pulses has processed!" << endl; cout << endl;

    for (i = 52; i < tzad.length(); i = i + 1)
    {
        constTzad = tzad[i];
        if (constTzad >= Tp)
        {
            constTzadNew = fmod(constTzad, Tp);
            cout << i << ". OD = " << constTzad << ", ND = " << constTzadNew << ", Tp = " << Tp << ", ti = " << ti << endl;
        }
    }

    return 0;
}
