import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14

Dialog {
    title: "Pixel Size Setup"
    standardButtons: Dialog.Ok | Dialog.Cancel
    property var pixelWidth: 5
    property var pixelHeight: 5
    property var pixelRows: 45
    property var pixelCols: 45
    onAccepted: {
        pixelWidth = pixWidth.content.text
        pixelHeight = pixHeight.content.text
        pixelRows = pixRows.content.text
        pixelCols = pixCols.content.text
        close()
    }
    onRejected: {
        pixWidth.content.text = pixelWidth
        pixHeight.content.text = pixelHeight
        pixRows.content.text = pixelRows
        pixCols.content.text = pixelCols
        close()
    }
    GridLayout {
        columns: 2
        Label {
            text: qsTr("Pixel Width")
        }
        TextInput {
            id: pixWidth
            text: pixelWidth
        }
        Label {
            text: qsTr("Pixel Height")
        }
        TextInput {
            id: pixHeight
            text: pixelHeight
        }
        Label {
            text: qsTr("Pixel Count Horizontal")
        }
        TextInput {
            id: pixCols
            text: pixelCols
        }
        Label {
            text: qsTr("Pixel Count Vertical")
        }
        TextInput {
            id: pixRows
            text: pixelRows
        }
    }
}
