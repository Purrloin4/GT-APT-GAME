#include "mainwindow.h"
#include "qloggingcategory.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QLoggingCategory pathfinderCategory("main", QtDebugMsg);
    QApplication a(argc, argv);
    QLoggingCategory::setFilterRules("pathfinderCategory.debug=true");
    MainWindow w; // ctor - full screen
//    MainWindow w(916, 600); // ctor - specified dimensions
    //qCDebug(main) << w.getView().scene()->items().count();
    w.show();
    return a.exec();
}
