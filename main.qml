import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs

Window
{
  id: mainWindow
  width: 640
  height: 480
  minimumWidth: 640
  maximumWidth: 640
  minimumHeight: 480
  maximumHeight: 480
  visible: true
  title: qsTr("WiFi Manager - VERSION_PLACEHOLDER")
  color: "#3e3e3e"

  Popup
  {
    property string ssid: ""

    id: connectSecuredDialog
    modal: true
    closePolicy: Popup.NoAutoClose
    background: Rectangle
    {
      color: "#606060"
      radius: ((mainWindow.width + mainWindow.height) / 2) / 50
    }
    width: mainWindow.width * 0.8
    height: mainWindow.height * 0.45
    x: (mainWindow.width - width) / 2
    y: (mainWindow.height - height) / 2

    Item
    {
      anchors.fill: parent

      Image
      {
        source: "qrc:/Icons/cancel.svg"
        anchors
        {
          top: parent.top
          right: parent.right
        }

        width: parent.width / 15
        height: width

        MouseArea
        {
          anchors.fill: parent
          cursorShape: Qt.PointingHandCursor

          onClicked:
          {
            connectSecuredDialog.close()
          }
        }
      }

      Text
      {
        id: titleText
        text: qsTr("Insert WiFi password for network \n'" + connectSecuredDialog.ssid + "'")
        font.bold: true
        font.pixelSize: 18
        color: "white"
        anchors.top: parent.top
        anchors.topMargin: 20
        horizontalAlignment: Text.AlignHCenter
        anchors.horizontalCenter: parent.horizontalCenter
        height: implicitHeight
      }

      Image
      {
        source: "qrc:/Icons/wifilock.svg"
        anchors.top: parent.top
        anchors.topMargin: 20
        anchors.left: parent.left
        anchors.leftMargin: 10
        width: titleText.height
        height: titleText.height
      }

      TextField
      {
        id: passwordField
        width: parent.width * 0.6
        anchors.top: titleText.bottom
        anchors.topMargin: connectSecuredDialog.height / 15
        anchors.horizontalCenter: parent.horizontalCenter
        placeholderText: qsTr("Password")
        focus: true
        enabled: !connectSecuredDialog.isConnecting
      }

      Rectangle
      {
        id: connectSecureButton
        color: "#464646"
        radius: ((mainWindow.width + mainWindow.height) / 2) / 50
        border.color: "#282828"
        border.width: 1
        width: parent.width / 3
        height: width / 3

        anchors
        {
          top: passwordField.bottom
          topMargin: connectSecuredDialog.height / 10
          horizontalCenter: passwordField.horizontalCenter
        }

        Text
        {
          text: qsTr("Connect")
          font.pixelSize: 18
          color: "white"
          anchors.centerIn: parent
        }

        MouseArea
        {
          anchors.fill: parent
          cursorShape: Qt.PointingHandCursor

          onClicked:
          {
            wifiManager.connectToWifiNetwork(connectSecuredDialog.ssid, passwordField.text)
            connectSecuredDialog.close()
          }
        }
      }
    }
  }

  Popup
  {
    id: manageSavedNetworksDialog
    modal: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
    background: Rectangle
    {
      color: "#606060"
      radius: ((mainWindow.width + mainWindow.height) / 2) / 50
    }
    width: mainWindow.width * 0.8
    height: mainWindow.height * 0.6
    x: (mainWindow.width - width) / 2
    y: (mainWindow.height - height) / 2

    onOpened:
    {
      savedNetworksList.model = wifiManager.getSavedNetworks();
    }

    Item
    {
      anchors.fill: parent

      Image
      {
        source: "qrc:/Icons/cancel.svg"
        anchors.top: parent.top
        anchors.right: parent.right
        width: parent.width / 15
        height: width

        MouseArea
        {
          anchors.fill: parent
          cursorShape: Qt.PointingHandCursor

          onClicked:
          {
            manageSavedNetworksDialog.close()
          }
        }
      }

      Text
      {
        id: savedNetworksDialogTitle
        text: qsTr("Saved Networks")
        font.bold: true
        font.pixelSize: 18
        color: "white"
        anchors.top: parent.top
        anchors.topMargin: 5
        anchors.horizontalCenter: parent.horizontalCenter
      }

      ListView
      {
        id: savedNetworksList
        anchors.top: savedNetworksDialogTitle.bottom
        anchors.topMargin: 15
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20
        clip: true

        delegate: Item
        {
          width: savedNetworksList.width
          height: 40

          Text
          {
            text: modelData
            color: "white"
            font.pixelSize: 16
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.right: forgetButton.left
            anchors.rightMargin: 10
          }

          Rectangle
          {
            id: forgetButton
            color: "#464646"
            radius: 5
            border.color: "#282828"
            border.width: 1
            width: 100
            height: 30
            anchors.right: parent.right
            anchors.rightMargin: 20
            anchors.verticalCenter: parent.verticalCenter

            Text
            {
              text: qsTr("Forget")
              font.pixelSize: 16
              color: "white"
              anchors.centerIn: parent
            }

            MouseArea
            {
              anchors.fill: parent
              cursorShape: Qt.PointingHandCursor

              onClicked:
              {
                wifiManager.removeSavedNetwork(modelData)
                savedNetworksList.model = wifiManager.getSavedNetworks()
              }
            }
          }
        }
      }
    }
  }

  Column
  {
    spacing: mainWindow.height / 40

    Item
    {
      id: headerItem
      height: mainWindow.height / 8
      width: mainWindow.width

      Text
      {
        id: wifiText
        text: "Wi-Fi"
        font.pixelSize: mainWindow.height / 24
        color: "white"
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: mainWindow.width / 50
      }

      Switch
      {
        id: wifiSwitch
        checked: wifiManager.wifiEnabled
        height: parent.height
        width: implicitWidth
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: wifiText.right
        anchors.leftMargin: mainWindow.width / 50

        onClicked:
        {
          if (!checked)
          {
            wifiManager.turnWifiOff()
          } else
          {
            wifiManager.turnWifiOn()
          }
        }
      }

      Image
      {
        source: "qrc:/Icons/managewifi.svg"
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.topMargin: parent.height / 4
        anchors.rightMargin: parent.height / 4
        width: parent.height / 2
        height: parent.height / 2

        MouseArea
        {
          anchors.fill: parent
          cursorShape: Qt.PointingHandCursor
          onClicked:
          {
            manageSavedNetworksDialog.open()
          }
        }
      }
    }

    Item
    {
      id: contentItem
      width: mainWindow.width
      height: mainWindow.height - headerItem.height

      Text
      {
        id: noNetworkManagerText
        text: "Networkmanager not installed"
        font.pixelSize: mainWindow.height / 20
        color: "white"
        anchors.centerIn: parent
        width: implicitWidth
        height: implicitHeight
        visible: !wifiManager.nmcliAvailable
      }

      Text
      {
        id: noHardwareText
        text: "No Wi-Fi hardware available"
        font.pixelSize: mainWindow.height / 20
        color: "white"
        anchors.centerIn: parent
        width: implicitWidth
        height: implicitHeight
        visible: wifiManager.nmcliAvailable  && !wifiManager.wifiHardwareAvailable
      } 

      ListView
      {
        id: wifiNetworksListView
        anchors
        {
          fill: parent
          leftMargin: mainWindow.width / 20
          rightMargin: mainWindow.width / 20
        }
        visible: wifiManager.nmcliAvailable && wifiManager.wifiHardwareAvailable && wifiManager.wifiEnabled

        model: wifiScanner
        clip: true

        delegate: Item
        {
          width: parent !== null ? parent.width : 0
          height: contentItem.height / 8

          Row
          {
            anchors.fill: parent
            spacing: mainWindow.width / 40

            Image
            {
              height: parent.height / 2
              width: height
              anchors.verticalCenter: parent.verticalCenter
              source:
              {
                if (model.strength < 70)
                  return "qrc:/Icons/wifi1bar.svg";
                else if (model.strength < 80)
                  return "qrc:/Icons/wifi2bar.svg";
                else if (model.strength < 90)
                  return "qrc:/Icons/wifi3bar.svg";
                else
                  return "qrc:/Icons/wifi4bar.svg";
              }
            }

            Text
            {
              text: model.ssid
              font.pixelSize: mainWindow.height / 24
              color: "white"
              anchors.verticalCenter: parent.verticalCenter
            }

            Image
            {
              height: parent.height / 2
              width: height
              anchors.verticalCenter: parent.verticalCenter
              source: "qrc:/Icons/check.svg"
              visible: model.connected
            }
          }

          Rectangle
          {
            property bool isEnabled: true

            id: connectButton
            color: "#464646"
            radius: ((mainWindow.width + mainWindow.height) / 2) / 50
            border.color: "#282828"
            border.width: 1

            anchors
            {
              top: parent.top
              bottom: parent.bottom
              right: parent.right

              topMargin: parent.height / 6
              bottomMargin: parent.height / 6
            }

            width: parent.height * 2.5

            Text
            {
              text: model.connected ? qsTr("Disconnect") : qsTr("Connect")
              font.pixelSize: mainWindow.height / 24
              color: "white"
              anchors.centerIn: parent
            }

            MouseArea
            {
              anchors.fill: parent
              cursorShape: Qt.PointingHandCursor

              onClicked:
              {
                if(model.connected)
                {
                  wifiManager.disconnectFromWifiNetwork(model.interface);
                }
                else
                {
                  if(model.secured)
                  {
                    connectSecuredDialog.ssid = model.ssid;
                    connectSecuredDialog.open();
                  }
                  else
                  {
                    wifiManager.connectToWifiNetwork(model.ssid, "");
                  }
                }
              }
            }
          }
        }
      }
    }
  }

  Connections
  {
    target: wifiManager

    function onWifiEnabledChanged(enabled)
    {
      wifiSwitch.checked = enabled
    }
  }
}
