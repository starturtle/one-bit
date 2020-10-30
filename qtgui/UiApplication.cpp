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
    const unsigned majorVersion{ 1 };
    const unsigned minorVersion{ 0 };
    const std::string versionString{ std::to_string(majorVersion) + "." + std::to_string(minorVersion) };
    const std::string orgName{ "starturtle" };
    const std::string appName{ "stixelator" };
    const std::string appUri{ orgName + "." + appName };

    // initialize application
    logging::logger().setLogLevel(logging::Level::DEBUG);
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication uiApp(argc, argv);
    uiApp.setOrganizationName(orgName.c_str());
    uiApp.setOrganizationDomain("org");
    uiApp.setApplicationName(appName.c_str());
    uiApp.setApplicationVersion(versionString.c_str());

    // register actual pixelation execution object
    if (-1 == qmlRegisterType<QtPixelator>(appUri.c_str(), majorVersion, minorVersion, "QtPixelator"))
    {
      return errors::QT_ERROR;
    }

    if (-1 == qmlRegisterType<SourceImage>(appUri.c_str(), majorVersion, minorVersion, "SourceImage"))
    {
      return errors::QT_ERROR;
    }

    if (-1 == qmlRegisterType<ResultImage>(appUri.c_str(), majorVersion, minorVersion, "ResultImage"))
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