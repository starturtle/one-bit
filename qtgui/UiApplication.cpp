#include "UiApplication.h"
#include "QtPixelator.h"
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "error_codes.h"
#include "logging.h"

namespace gui_mode
{
  int run_as_window(int argc, char* argv[], const one_bit::ArgumentParser& in_params)
  {
    // initialize application
    logging::LogStream::instance().setLogLevel(logging::Level::DEBUG);
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication uiApp(argc, argv);
    uiApp.setOrganizationName("starturtle");
    uiApp.setOrganizationDomain("one-bit");

    // register actual pixelation execution object
    if (-1 == qmlRegisterType<QtPixelator>("starturtle.oneBit", 1, 0, "QtPixelator"))
    {
      return errors::QT_ERROR;
    }
    logging::LogStream::instance().getLogStream(logging::Level::DEBUG) << "UI App set up!" << std::endl;

    // connect to application engine
    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/ApplicationWindow.qml"));
    auto failureSlot = [url](QObject* obj, const QUrl& objUrl) {
      if (!obj && url == objUrl) QCoreApplication::exit(int(errors::QT_ERROR));
    };
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
      &uiApp, failureSlot, Qt::QueuedConnection);
    engine.load(url);
    logging::LogStream::instance().getLogStream(logging::Level::DEBUG) << "UI engine connected!" << std::endl;

    return uiApp.exec();
  }
}