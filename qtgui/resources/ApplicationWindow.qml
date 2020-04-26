import QtQuick 2.12
import QtQuick.Controls 2.14
import QtQuick.Dialogs 1.3

ApplicationWindow {
    id: mainWindow
    visible: true
    width: 800
    height: 600
    title: qsTr("Image Pixelation")

    menuBar: MenuBar {
        Menu {
            title: qsTr("File")
            MenuItem {
                text: qsTr("&Load")
                onTriggered: {
					inputFileGet.open()
				}
            }
            MenuItem {
                text: qsTr("Select Out&path")
                onTriggered: {
					outputFileGet.open()
				}
            }
            MenuItem {
                text: qsTr("Exit")
                onTriggered: Qt.quit();
            }
        }
    }

	InputFileChooser {
		id: inputFileGet
		visible: false
		title: "Select File To Pixelate"
	}
	
	InputFileChooser {
		id: outputFileGet
		visible: false
		selectExisting: false
		title: "Select Store Path"
	}
	
	SplitView {
		id: imagePreview
		anchors.fill: parent

		ItemDelegate {
			id: inputImage
			SplitView.minimumWidth: 200
			SplitView.preferredWidth: 400
			SplitView.maximumWidth: 600
			icon.source: inputFileGet.fileUrl
			icon.width: 400
		}

		ItemDelegate {
			id: outputImage
			SplitView.minimumWidth: 200
			SplitView.preferredWidth: 400
			SplitView.maximumWidth: 600
			icon.source: inputFileGet.fileUrl
			icon.width: 400
		}

		// ...
	}
}