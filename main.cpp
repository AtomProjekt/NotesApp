#include <QApplication>
#include <QIcon>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Метаданные приложения
    app.setApplicationName("NotesApp");
    app.setOrganizationName("NotesApp");
    app.setApplicationVersion("1.0.0");

    // Иконка из ресурсов
    app.setWindowIcon(QIcon(":/logo/myicon.ico"));

    // Сглаживание шрифтов
    app.setAttribute(Qt::AA_UseHighDpiPixmaps);

    MainWindow w;
    w.show();

    return app.exec();
}
