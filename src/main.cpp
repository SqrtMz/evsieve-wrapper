#include "gui/home.hpp"
#include <QApplication>
#include <fcntl.h>
#include <libevdev/libevdev.h>
#include <string>

int main(int argc, char* argv[]) {
    
    QApplication app(argc, argv);
    Home w;
    w.show();

    return app.exec();
}