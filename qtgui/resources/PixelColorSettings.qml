import QtQuick 2.12
import QtQuick.Controls 2.14
import QtQuick.Dialogs 1.3
import QtQuick.Layouts 1.14

Pane {
  Layout.alignment: Qt.AlignTop | Qt.AlignLeft
  Layout.margins: 0
  property color pixelColor: color("black")
  
  Button {
    id: changeButton
    text: ""
    Layout.alignment: Qt.AlignTop | Qt.AlignLeft
    Layout.margins: 0
    width: 25
    height: 25
    background: Rectangle {
      implicitWidth: 25
      implicitHeight: 25
      color: pixelColor
      border.color: "white"
      border.width: 1
      radius: 4
    }
    onClicked: {
       colorChooser.open()
    }
  }
  ColorDialog {
    id: colorChooser
    visible: false
    onAccepted: {
      pixelColor = color
      colorChanged()
    }
  }
  property string colorName: changeButton.text
}