#include "MainWindow.h"
#include <QApplication>

#include "common.h"
#include "input.h"
#include "localSearch.h"
#include "memetic.h"
using namespace std;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
