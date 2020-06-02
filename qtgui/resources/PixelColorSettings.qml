import QtQuick 2.12
import QtQuick.Controls 2.14
import QtQuick.Dialogs 1.3
import QtQuick.Layouts 1.14

Pane {
  property color pixelColor: color("black")
  
  GridLayout {
    columns: 2
    Rectangle {
      id: colorLabel
      color: pixelColor
      width: 100
      height: 30
      border.color: "white"
    }
    Button {
      id: changeButton
      text: "Change"
      width: 100
      height: 30
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
    }
  }
}