#pragma once

#include <QtCore>
#include <QtConcurrent>
#include "nmcli.h"

struct WiFiNetwork
{
  QString ssid;
  bool secured;
  int strength;
  bool connected;
  QString iface;
};

struct ConnectionInfo
{
  QString ssid;
  QString iface;
};

class ListWifiNetworksTask : public QObject, public QRunnable
{
  Q_OBJECT
public:
  ListWifiNetworksTask() : QRunnable()
  {
  }

  void run() override
  {
    QList<WiFiNetwork> networks = scanNetworks(connectionInfo());
    emit WiFiNetworksUpdated(networks);
  }

private:
  ConnectionInfo connectionInfo()
  {
    QStringList parameters = {"-terse", "device", "status"};
    QString output = nmcli::exec(parameters);
    QStringList lines = output.split('\n');
    for (const QString &line : lines)
    {
      QStringList fields = nmcli::getFields(line);
      if (fields.size() >= 4 && fields[1] == "wifi" && fields[2] == "connected")
      {
        ConnectionInfo info;
        info.ssid = fields[3];
        info.iface = fields[0];
        return info;
      }
    }

    return ConnectionInfo();
  }

  QList<WiFiNetwork> scanNetworks(const ConnectionInfo& connectionInfo)
  {
    auto getNetwork = [](const WiFiNetwork &network, QList<WiFiNetwork> &networks)
    {
      for (auto i = networks.begin(); i != networks.end(); ++i)
      {
        if (i->ssid == network.ssid && i->secured == network.secured)
        {
          return i;
        }
      }

      return networks.end();
    };

    QList<WiFiNetwork> networks;
    QStringList parameters = {"-terse", "-fields", "ssid,signal,security", "dev", "wifi", "list"};
    QString output = nmcli::exec(parameters);
    QStringList lines = output.split('\n');

    foreach (const QString &line, lines)
    {
      if(!line.isEmpty())
      {
        QStringList fields = nmcli::getFields(line);
        if (fields.size() >= 2)
        {
          QString ssid = fields[0];
          QString security = (fields.size() == 3) ? fields[2] : QString();
          QString strengthStr = fields[1];

          if(!ssid.isEmpty())
          {
            WiFiNetwork network;
            network.ssid = ssid;
            network.secured = !security.isEmpty();
            bool strengthOk = false;
            network.strength = strengthStr.toInt(&strengthOk);
            if (!strengthOk)
              network.strength = 0;
            network.connected = (network.ssid == connectionInfo.ssid);
            network.iface = connectionInfo.iface;

            auto i = getNetwork(network, networks);
            if (i == networks.end())
            {
              networks.append(network);
            }
            else
            {
              // Update strength
              if (network.strength > i->strength)
              {
                i->strength = network.strength;
              }
            }
          }
        }
      }
    }

    // Sort the networks based on strength
    std::sort(networks.begin(), networks.end(), [](const WiFiNetwork &a, const WiFiNetwork &b)
              { return a.strength > b.strength; });

    return networks;
  }

signals:
  void WiFiNetworksUpdated(const QList<WiFiNetwork> &networks);
};

class WifiScanner : public QAbstractListModel
{
  Q_OBJECT

public:
  enum WiFiRoles
  {
    SSIDRole = Qt::UserRole + 1,
    SecurityRole,
    StrengthRole,
    ConnectedRole,
    InterfaceRole
  };

  WifiScanner(QObject *parent = nullptr) : QAbstractListModel(parent)
  {
    QObject::connect(&m_updateTimer, &QTimer::timeout, this, &WifiScanner::updateWiFiNetworks);
    m_updateTimer.start(5000);
    updateWiFiNetworks();
  }

  int rowCount(const QModelIndex &parent = QModelIndex()) const override
  {
    Q_UNUSED(parent)
    return m_networks.count();
  }

  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override
  {
    if (!index.isValid())
      return QVariant();

    if (index.row() >= m_networks.size() || index.row() < 0)
      return QVariant();

    if (role == SSIDRole)
      return m_networks.at(index.row()).ssid;
    else if (role == SecurityRole)
      return m_networks.at(index.row()).secured;
    else if (role == StrengthRole)
      return m_networks.at(index.row()).strength;
    else if (role == ConnectedRole)
      return m_networks.at(index.row()).connected;
    else if (role == InterfaceRole)
      return m_networks.at(index.row()).iface;

    return QVariant();
  }

  QHash<int, QByteArray> roleNames() const override
  {
    QHash<int, QByteArray> roles;
    roles[SSIDRole] = "ssid";
    roles[SecurityRole] = "secured";
    roles[StrengthRole] = "strength";
    roles[ConnectedRole] = "connected";
    roles[InterfaceRole] = "interface";
    return roles;
  }

public slots:
  void updateWiFiNetworks()
  {
    ListWifiNetworksTask* task = new ListWifiNetworksTask();
    QObject::connect(task, &ListWifiNetworksTask::WiFiNetworksUpdated, this, &WifiScanner::onWifiNetworksUpdated, Qt::QueuedConnection);
    QThreadPool::globalInstance()->start(task);
  }

private slots:
  void onWifiNetworksUpdated(const QList<WiFiNetwork> networks)
  {
    beginResetModel();
    m_networks = networks;
    endResetModel();
  }

private:
  QList<WiFiNetwork> m_networks;
  QTimer m_updateTimer;
};

