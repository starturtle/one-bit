import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14

Pane {
  width: 400
  GridLayout {
    columns: 2
    Label {
      text: qsTr("Overall Width (cm)")
    }
    TextInput {
      id: resWidth
      color: "white"
      text: "12"
    }
    Label {
      text: qsTr("Overall Height (cm)")
    }
    TextInput {
      id: resHeight
      color: "white"
      text: "12"
    }
    Label {
      text: qsTr("Stitches per gauge (10cm)")
    }
    TextInput {
      id: stCols
      color: "white"
      text: "25"
    }
    Label {
      text: qsTr("Rows per gauge (10cm)")
    }
    TextInput {
      id: stRows
      color: "white"
      text: "22"
    }
  }
  property var resultWidth: resWidth.text
  property var resultHeight: resHeight.text
  property var stitchRows: stRows.text
  property var stitchColumns: stCols.text
}
