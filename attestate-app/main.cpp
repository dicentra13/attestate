#include <QApplication>

#include "main_window.h"

int main(int argc, char** argv)
{
    QLocale::setDefault(QLocale(QLocale::Russian, QLocale::RussianFederation));

    QApplication app(argc, argv);

    QTranslator qtTranslator;
    qtTranslator.load(
        "qt_" + QLocale::system().name(),
        QLibraryInfo::location(QLibraryInfo::TranslationsPath));

    app.installTranslator(&qtTranslator);

    MainWindow editor;
    editor.show();
    return app.exec();
}
