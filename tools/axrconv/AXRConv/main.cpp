#include "mainwindow.h"

#include <QApplication>
#include <QSurfaceFormat>



int main(int argc, char *argv[]){

    QApplication::setAttribute(Qt::AA_DisableWindowContextHelpButton);
    QApplication a(argc, argv);

    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setVersion(3, 3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(format);

    MainWindow w;
    w.show();

    return a.exec();

}
