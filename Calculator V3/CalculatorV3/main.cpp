#include <QtGui/QApplication>
#include "calculator3.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Calculator w;
    w.show();
    
    return a.exec();
}
