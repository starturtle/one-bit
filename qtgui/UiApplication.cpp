#include "UiApplication.h"
#include "QtPixelator.h"
#include "SourceImage.h"
#include "ResultImage.h"
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "error_codes.h"
#include "logging.h"

namespace gui_mode
{
  int run_as_window(int argc, char* argv[], const one_bit::ArgumentParser& in_params)
  {
    // initialize application
    logging::logger().setLogLevel(logging::Level::DEBUG);
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication uiApp(argc, argv);
    uiApp.setOrganizationName("starturtle");
    uiApp.setOrganizationDomain("org");
    uiApp.setApplicationName("one-bit");
    uiApp.setApplicationVersion("1.0");

    // register actual pixelation execution object
    if (-1 == qmlRegisterType<QtPixelator>("starturtle.oneBit", 1, 0, "QtPixelator"))
    {
      return errors::QT_ERROR;
    }

    if (-1 == qmlRegisterType<SourceImage>("starturtle.oneBit", 1, 0, "SourceImage"))
    {
      return errors::QT_ERROR;
    }

    if (-1 == qmlRegisterType<ResultImage>("starturtle.oneBit", 1, 0, "ResultImage"))
    {
      return errors::QT_ERROR;
    }

    if (-1 == qRegisterMetaType<std::vector<QColor>>()) // for color array in QtPixelator
    {
      return errors::QT_ERROR;
    }

    logging::logger() << logging::Level::DEBUG << "UI App set up!" << logging::Level::OFF;

    // connect to application engine
    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/ApplicationWindow.qml"));
    auto failureSlot = [url](QObject* obj, const QUrl& objUrl) {
      if (!obj && url == objUrl) QCoreApplication::exit(int(errors::QT_ERROR));
    };
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
      &uiApp, failureSlot, Qt::QueuedConnection);
    engine.load(url);
    logging::logger() << logging::Level::DEBUG << "UI engine connected!" << logging::Level::OFF;

    return uiApp.exec();
  }
}