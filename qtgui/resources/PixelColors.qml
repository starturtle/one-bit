import QtQuick 2.12
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14

Pane {
  GridLayout {
    columns: 2
    Button {
      id: add
      text: "Add"
      onClicked: {
        if (! cols3.visible)
        {
          cols3.visible = true
          remove.enabled = true
        }
        else if (! cols4.visible)
        {
          cols4.visible = true
          enabled = false
        }
      }
    }
    Button {
      id: remove
      text: "Remove"
      onClicked: {
        if (cols4.visible)
        {
          cols4.visible = false
          add.enabled = true
        }
        else if (cols3.visible)
        {
          cols3.visible = false
          enabled = false
        }
      }
    }
    PixelColorSettings {
      id: cols1
      pixelColor: "black"
    }
    PixelColorSettings {
      id: cols2
      pixelColor: "white"
    }
    PixelColorSettings {
      id: cols3
      visible: false
      pixelColor: "red"
    }
    PixelColorSettings {
      id: cols4
      visible: false
      pixelColor: "green"
    }
  }
  property variant colors: {
    if (cols4.visible) {
      return [cols1.pixelColor, cols2.pixelColor, cols3.pixelColor, cols4.pixelColor]
    }
    else if (cols3.visible) {
      return [cols1.pixelColor, cols2.pixelColor, cols3.pixelColor]
    }
    else {
      return [cols1.pixelColor, cols2.pixelColor]
    }
  }
}