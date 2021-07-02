#include <iostream>
#include <unistd.h>
#include <QVector>
#include <QDebug>

using namespace std;

int main()
{
    int R;  // м
    cout << "Enter start distance: "; cin >> R;
    int R1;
    float dt = 0.1;

    short V; // м/с
    cout << "Enter start velocity: "; cin >> V;

    QVector<int> spisok_R;

    for (R1 = R; R1 < 10000; R1 = R1 + V*dt)
    {
        spisok_R.append(R1);
        cout << "Distance = " << R1 << " m" << endl;
        sleep(dt);
    }

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
