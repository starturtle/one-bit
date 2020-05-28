import QtQuick 2.12
import QtQuick.Controls 2.14
import QtQuick.Dialogs 1.3
import QtQuick.Layouts 1.14

Dialog {
    title: "Select Colors"
    standardButtons: Dialog.Ok | Dialog.Cancel
	property var color1: color("black")
	property var color2: color("white")
    property var colorToChange: undefined
    property var labelToChange: undefined
    onAccepted: {
        close()
    }
    onRejected: {
        console.log("Canceled")
        close()
    }
	
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
                 colorChooser.currentColor = color1
                 colorChooser.color = color1
                 colorToChange = color1
				 labelToChange = lbcol1
                 colorChooser.open()
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
                 colorChooser.currentColor = color2
                 colorChooser.color = color2
                 colorToChange = color2
				 labelToChange = lbcol2
                 colorChooser.open()
            }
        }
    }
    ColorDialog {
        id: colorChooser
		visible: false
        onAccepted: {
			if (colorToChange != undefined)
			{
				colorToChange = color
				labelToChange.color = color
			}
        }
    }
}