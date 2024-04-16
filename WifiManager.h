#pragma once

#include <QtCore>
#include <QTimer>
#include <QRunnable>
#include <QThreadPool>
#include "WifiScanner.h"
#include "nmcli.h"

class WifiStatusUpdateRunnable : public QObject, public QRunnable
{
  Q_OBJECT

public:
  WifiStatusUpdateRunnable(QObject *parent = nullptr)
      : QObject(parent)
  {
  }

signals:
  void wifiStatusUpdated(bool wifiHardwareAvailable, bool wifiEnabled);

protected:
  void run() override
  {
    QStringList parameters;
    QString output;

    parameters = {"-terse", "-fields", "type", "device"};
    output = nmcli::exec(parameters);
    QStringList devices = output.split("\n");
    bool wifiHardwareAvailable = devices.contains("wifi");

    parameters = {"radio", "wifi"};
    output = nmcli::exec(parameters).trimmed();
    bool wifiEnabled = (output == "enabled");

    emit wifiStatusUpdated(wifiHardwareAvailable, wifiEnabled);
  }
};

class WifiManager : public QObject
{
  Q_OBJECT

  Q_PROPERTY(bool nmcliAvailable READ isNmcliAvailable CONSTANT)
  Q_PROPERTY(bool wifiHardwareAvailable READ isWifiHardwareAvailable NOTIFY wifiHardwareAvailableChanged)
  Q_PROPERTY(bool wifiEnabled READ isWifiEnabled NOTIFY wifiEnabledChanged)

public:
  WifiManager(QObject *parent = nullptr)
      : QObject(parent),
      m_scanner(),
      m_nmcliAvailable(nmcli::available()),
      m_wifiHardwareAvailable(true),
      m_wifiEnabled(false),
      m_statusCheckTimer()
  {
    m_scanner.updateWiFiNetworks();

    QObject::connect(&m_statusCheckTimer, &QTimer::timeout, this, &WifiManager::updateWifiStatus);
    m_statusCheckTimer.start(5000);
    updateWifiStatus();
  }

  WifiScanner* scanner()
  {
    return &m_scanner;
  }

  Q_INVOKABLE QStringList getSavedNetworks()
  {
    QStringList networks;

    QStringList parameters = {"-terse", "-fields", "name,type", "connection"};
    QString output = nmcli::exec(parameters);
    QStringList lines = output.split("\n");

    for(const QString &line : lines)
    {
      QStringList fields = nmcli::getFields(line);
      if(fields.size() == 2 && !fields[0].isEmpty() && fields[1] == "802-11-wireless")
      {
        networks.append(fields[0]);
      }
    }

    return networks;
  }

signals:
  void wifiHardwareAvailableChanged(bool available);
  void wifiEnabledChanged(bool enabled);

public slots:
  void turnWifiOn()
  {
    QStringList parameters = {"-wait", "0", "radio", "wifi", "on"};
    nmcli::exec(parameters);
    m_wifiEnabled = true;
    emit wifiEnabledChanged(m_wifiEnabled);
    m_scanner.updateWiFiNetworks();
  }

  void turnWifiOff()
  {
    QStringList parameters = {"-wait", "0", "radio", "wifi", "off", };
    nmcli::exec(parameters);
    m_wifiEnabled = false;
    emit wifiEnabledChanged(m_wifiEnabled);
    m_scanner.updateWiFiNetworks();
  }

  void connectToWifiNetwork(const QString &ssid, const QString &password)
  {
    QStringList parameters;
    if (password.isEmpty())
    {
      parameters = {"-wait", "0", "device", "wifi", "connect", ssid};
    }
    else
    {
      parameters = {"-wait", "0", "device", "wifi", "connect", ssid, "password", password};
    }
    nmcli::exec(parameters);
    m_scanner.updateWiFiNetworks();
  }

  void disconnectFromWifiNetwork(const QString &iface)
  {
    QStringList parameters = {"-wait", "0", "device", "disconnect", iface};
    nmcli::exec(parameters);
    m_scanner.updateWiFiNetworks();
  }

  void removeSavedNetwork(const QString &ssid)
  {
    QStringList parameters = {"connection", "delete", ssid};
    nmcli::exec(parameters);
    m_scanner.updateWiFiNetworks();
  }

private slots:
  void updateWifiStatus()
  {
    WifiStatusUpdateRunnable* task = new WifiStatusUpdateRunnable();
    QObject::connect(task, &WifiStatusUpdateRunnable::wifiStatusUpdated, this, &WifiManager::onWifiStatusUpdated, Qt::QueuedConnection);
    QThreadPool::globalInstance()->start(task);
  }

  void onWifiStatusUpdated(bool wifiHardwareAvailable, bool wifiEnabled)
  {
    if(m_wifiHardwareAvailable != wifiHardwareAvailable)
    {
      m_wifiHardwareAvailable = wifiHardwareAvailable;
      emit wifiEnabledChanged(m_wifiHardwareAvailable);
    }

    if(m_wifiEnabled != wifiEnabled)
    {
      m_wifiEnabled = wifiEnabled;
      emit wifiEnabledChanged(m_wifiEnabled);
    }
  }

private:
  WifiScanner m_scanner;
  bool m_nmcliAvailable;
  bool m_wifiHardwareAvailable;
  bool m_wifiEnabled;
  QTimer m_statusCheckTimer;

  bool isNmcliAvailable() const
  {
    return m_nmcliAvailable;
  }

  bool isWifiHardwareAvailable() const
  {
    return m_wifiHardwareAvailable;
  }

  bool isWifiEnabled() const
  {
    return m_wifiEnabled;
  }
};
