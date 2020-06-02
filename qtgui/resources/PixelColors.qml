import QtQuick 2.12
import QtQuick.Controls 2.14
import QtQuick.Dialogs 1.3
import QtQuick.Layouts 1.14

Frame {
    property var color1: color("black")
    property var color2: color("white")
    property var colorToChange: undefined
    property var labelToChange: undefined
    
    GridLayout {
        columns: 2
        Rectangle {
            id: lbcol1
            color: color1
            width: 200
            height: 50
        }
        Button {
            id: changeColor1
            text: "Change"
            width: 200
            height: 30
            onClicked: {
                 colorChooser1.open()
            }
        }
        Rectangle {
            id: lbcol2
            color: color2
            width: 200
            height: 50
        }
        Button {
            id: changeColor2
            text: "Change"
            width: 200
            height: 30
            onClicked: {
                 colorChooser2.open()
            }
        }
    }
    ColorDialog {
        id: colorChooser1
        visible: false
        onAccepted: {
            color1 = color
        }
    }
    ColorDialog {
        id: colorChooser2
        visible: false
        onAccepted: {
            color2 = color
        }
    }
}