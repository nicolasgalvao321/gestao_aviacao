#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow window;
    window.setWindowTitle("AeroGestão - Gestão de Aviação");
    window.setWindowIcon(QIcon(":/icons/plane.png"));
    window.resize(900, 700);
    window.show();

    return app.exec();
}
