import QtQuick 2.12
import QtQuick.Controls 2.14
import QtQuick.Dialogs 1.3

Dialog {
    title: "Select Colors"
    standardButtons: Dialog.Ok | Dialog.Cancel
    property var colorToChange: color1
    onAccepted: {
        close()
    }
    onRejected: {
        console.log("Canceled")
        close()
    }
    Column {
        Rectangle {
            id: color1
            color: "black"
            width: 200
            height: 50
        }
        Button {
            id: changeColor1
            text: "Color 1"
            width: 200
            height: 30
            onClicked: {
                 colorChooser.currentColor = color1.color
                 colorChooser.color = color1.color
                 colorToChange = color1
                 colorChooser.open()
            }
        }
        Rectangle {
            id: color2
            color: "white"
            width: 200
            height: 50
        }
        Button {
            id: changeColor2
            text: "Color 2"
            width: 200
            height: 30
            onClicked: {
                 colorChooser.currentColor = color2.color
                 colorChooser.color = color2.color
                 colorToChange = color2
                 colorChooser.open()
            }
        }
    }
    ColorDialog {
        id: colorChooser
        modality: Qt.modal
        onAccepted: {
            colorToChange.color = color
        }
    }
}