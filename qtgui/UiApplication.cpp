#include "UiApplication.h"
#include "QtPixelator.h"
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "error_codes.h"

namespace gui_mode
{
    int run_as_window(int argc, char* argv[], const one_bit::ArgumentParser& in_params)
    {
        // initialize application
        QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

        QGuiApplication uiApp(argc, argv);

        // register actual pixelation execution object
        if (-1 == qmlRegisterType<QtPixelator>("starturtle.oneBit", 1, 0, "QtPixelator"))
        {
            return errors::QT_ERROR;
        }

        // connect to application engine
        QQmlApplicationEngine engine;
        const QUrl url(QStringLiteral("qrc:/ApplicationWindow.qml"));
        auto failureSlot = [url](QObject* obj, const QUrl& objUrl) {
            if (!obj && url == objUrl) QCoreApplication::exit(errors::QT_ERROR);
        };
        QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
            &uiApp, failureSlot, Qt::QueuedConnection);
        engine.load(url);

        return uiApp.exec();
    }
}