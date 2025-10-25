#include "inc/mainwindow.h"
#include "qsurfaceformat.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QSurfaceFormat format;
    format.setVersion(4, 6);                   // твоя версия OpenGL
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setOption(QSurfaceFormat::DebugContext); // включаем отладочный контекст
    QSurfaceFormat::setDefaultFormat(format);  // задаём как дефолт
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "Test-That_Robot_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    MainWindow w;
    w.show();
    return a.exec();
}
