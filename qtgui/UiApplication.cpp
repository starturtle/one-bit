#include "UiApplication.h"
#include <QGuiApplication>
#include <QQmlApplicationEngine>
namespace gui_mode
{
    int run_as_window(int argc, char* argv[], const one_bit::ArgumentParser& in_params)
    {
        QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

        QGuiApplication uiApp(argc, argv);

        QQmlApplicationEngine engine;
        const QUrl url(QStringLiteral("qrc:/ApplicationWindow.qml"));
        QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
            &uiApp, [url](QObject* obj, const QUrl& objUrl) {
                if (!obj && url == objUrl)
                    QCoreApplication::exit(-1);
            }, Qt::QueuedConnection);
        engine.load(url);
        return uiApp.exec();
    }
}