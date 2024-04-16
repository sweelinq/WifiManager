#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QIcon>
#include "WifiManager.h"

int main(int argc, char *argv[])
{
  QGuiApplication app(argc, argv);
  app.setWindowIcon(QIcon(":/Icons/appicon.svg"));

  WifiManager wifiManager;

  {
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("wifiManager", &wifiManager);
    engine.setObjectOwnership(&wifiManager, QQmlEngine::CppOwnership);
    engine.rootContext()->setContextProperty("wifiScanner", wifiManager.scanner());
    engine.setObjectOwnership(wifiManager.scanner(), QQmlEngine::CppOwnership);

    engine.load("qrc:/WifiManager/main.qml");
    app.exec();
  }

  return 0;
}
