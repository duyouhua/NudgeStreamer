#include "Streamer.h"

#include <iostream>
#include <QMessageBox>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Streamer* s = new Streamer;
    int result = 0;
    if (!s)
    {
        QMessageBox(QMessageBox::Critical, "Error", "Fail to initial UI!", QMessageBox::Ok).exec();
        return EXIT_FAILURE;
    }
    s->show();
    s->InitialPlugins();

    try
    {
        std::cout << "Before exec" << std::endl;
        result = a.exec();
        std::cout << "After exec" << std::endl;
    }
    catch (...)
    {
        QMessageBox::warning(0, "System Crashed!", "Hum，it seems that the system is crashed, sorry!");
    }

    return result;
}
