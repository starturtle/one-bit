import QtQuick 2.12
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14

Pane {
  GridLayout {
    columns: 4
    CheckBox {
      id: enableButton
      Layout.columnSpan: 2
      text: "Enable grid"
      checked: true
    }
    Label {
      text: "primary color"
    }
    PixelColorSettings {
      id: priColor
      pixelColor: "red"
    }
    
    Label {
      text: "secondary color"
    }
    PixelColorSettings {
      id: secColor
      pixelColor: "darkgray"
    }
    
    Label {
      id: lbPriWidth
      text: "St. per primary grid line"
    }
    TextInput {
      id: priWidth
      color: "white"
      text: "5"
      font.pixelSize: lbPriWidth.font.pixelSize - 2
      verticalAlignment: TextInput.AlignVCenter
    }
  }
  
  signal settingsChanged()
  Component.onCompleted: {
    enableButton.toggled.connect(settingsChanged)
    priColor.pixelColorChanged.connect(settingsChanged)
    secColor.pixelColorChanged.connect(settingsChanged)
    priWidth.editingFinished.connect(settingsChanged)
  }
  
  property bool gridEnabled: enableButton.checked
  property color primary: priColor.pixelColor
  property color secondary: secColor.pixelColor
  property var gridCount: priWidth.text
}