#include <QApplication>

#include "gui/mainwindow.h"
#include "pl0controller.h"

int main(int argc, char *argv[])
{
     QApplication app(argc, argv);

     PL0Controller *con = new PL0Controller();
     MainWindow *mainWin = new MainWindow(con);
     con->setMainWindow(mainWin);
     mainWin->initWindow();

     mainWin->show();

     return app.exec();
}
