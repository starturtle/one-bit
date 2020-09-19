import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14

Pane {
  width: 400
  GridLayout {
    columns: 2
    Label {
      id: lResWidth
      text: qsTr("Overall Width (cm)")
    }
    TextInput {
      id: resWidth
      color: "white"
      text: "12"
      font.pixelSize: lResWidth.font.pixelSize - 2
      verticalAlignment: TextInput.AlignVCenter
    }
    Label {
      id: lResHeight
      text: qsTr("Overall Height (cm)")
    }
    TextInput {
      id: resHeight
      color: "white"
      text: "12"
      font.pixelSize: lResHeight.font.pixelSize - 2
      verticalAlignment: TextInput.AlignVCenter
    }
    Label {
      id: lStCols
      text: qsTr("Stitches per gauge (10cm)")
    }
    TextInput {
      id: stCols
      color: "white"
      text: "25"
      font.pixelSize: lStCols.font.pixelSize - 2
      verticalAlignment: TextInput.AlignVCenter
    }
    Label {
      id: lStRows
      text: qsTr("Rows per gauge (10cm)")
    }
    TextInput {
      id: stRows
      color: "white"
      text: "22"
      font.pixelSize: lStRows.font.pixelSize - 2
      verticalAlignment: TextInput.AlignVCenter
    }
  }
  property var resultWidth: resWidth.text
  property var resultHeight: resHeight.text
  property var dimensions: resultWidth, resultHeight
  property var stitchRows: stRows.text
  property var stitchColumns: stCols.text
  signal sizesChanged()

  Component.onCompleted: {
    resWidth.editingFinished.connect(sizesChanged)
    resHeight.editingFinished.connect(sizesChanged)
    stCols.editingFinished.connect(sizesChanged)
    stRows.editingFinished.connect(sizesChanged)
  }
}
