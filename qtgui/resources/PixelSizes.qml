import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14

Dialog {
    title: "Pixel Size Setup"
    standardButtons: Dialog.Ok | Dialog.Cancel
	width: 400
    property var resultWidth: 12
    property var resultHeight: 12
    property var stitchRows: 22
    property var stitchColumns: 25
    onAccepted: {
        resultWidth = resWidth.content.text
        resultHeight = resHeight.content.text
        stitchRows = stRows.content.text
        stitchColumns = stCols.content.text
        close()
    }
    onRejected: {
        resWidth.content.text = resultWidth
        resHeight.content.text = resultHeight
        stRows.content.text = stitchRows
        stCols.content.text = stitchColumns
        close()
    }
    GridLayout {
        columns: 2
        Label {
            text: qsTr("Overall Width (cm)")
        }
        TextInput {
            id: resWidth
            color: "white"
            text: resultWidth
        }
        Label {
            text: qsTr("Overall Height (cm)")
        }
        TextInput {
            id: resHeight
            color: "white"
            text: resultHeight
        }
        Label {
            text: qsTr("Stitch Test Columns (10cm)")
        }
        TextInput {
            id: stCols
            color: "white"
            text: stitchColumns
        }
        Label {
            text: qsTr("Stitch Test Rows (10cm)")
        }
        TextInput {
            id: stRows
            color: "white"
            text: stitchRows
        }
    }
}
