import QtQuick 2.12
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14

Pane {
  GridLayout {
    columns: 2
    CheckBox {
      id: enableButton
      Layout.columnSpan: 2
      text: "Enable grid"
      checked: true
    }
    PixelColorSettings {
      id: priColor
      Layout.columnSpan: 2
      colorName: "primary"
      pixelColor: "red"
    }
    PixelColorSettings {
      id: secColor
      Layout.columnSpan: 2
      colorName: "secondary"
      pixelColor: "darkgray"
    }
    Label {
      id: lbPriWidth
      text: "Stixels per primary grid line"
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
    priColor.colorChanged.connect(settingsChanged)
    secColor.colorChanged.connect(settingsChanged)
    priWidth.editingFinished.connect(settingsChanged)
  }
  property bool gridEnabled: enableButton.checked
  property color primary: priColor.pixelColor
  property color secondary: secColor.pixelColor
  property var gridCount: priWidth.text
}