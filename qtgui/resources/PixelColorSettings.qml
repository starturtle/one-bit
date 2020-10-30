import QtQuick 2.12
import QtQuick.Controls 2.14
import QtQuick.Dialogs 1.3
import QtQuick.Layouts 1.14

Pane {
  property color pixelColor: color("black")
  
  Button {
    Layout.alignment: Qt.AlignTop | Qt.AlignLeft
    Layout.margins: 0
    id: changeButton
    text: ""
    width: 25
    height: 25
    background: Rectangle {
      color: pixelColor
      border.color: "white"
      border.width: 1
    }
    onClicked: {
      colorChooser.currentColor = pixelColor
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