import QtQuick 2.12
import QtQuick.Controls 2.14
import QtQuick.Dialogs 1.3
import QtQuick.Layouts 1.14

Pane {
  property color pixelColor: color("black")
  
  GridLayout {
    Button {
      id: changeButton
      text: ""
      width: 30
      height: 30
      background: Rectangle {
        implicitWidth: 30
        implicitHeight: 30
        color: pixelColor
        border.color: "white"
        border.width: 1
        radius: 4
      }
      onClicked: {
         colorChooser.open()
      }
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