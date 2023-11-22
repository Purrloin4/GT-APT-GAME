#include "mainwindow.h"
#include "qloggingcategory.h"
#include "/controller/worldcontroller.h"
#include "/controller/graphicviewcontroller.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QLoggingCategory::setFilterRules("pathfinderCategory.debug=true");
    MainWindow w;
    w.show();
    return a.exec();
}
