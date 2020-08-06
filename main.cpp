#include "diodialog.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DIODialog w;
    w.show();
    return a.exec();
}
