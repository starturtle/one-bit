import QtQuick 2.12
import QtQuick.Controls 2.14
import QtQuick.Dialogs 1.3
import starturtle.oneBit 1.0

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
		Menu {
			title: qsTr("Settings")
			MenuItem {
				text: qsTr("&Pixels...")
				onTriggered: {
					pixelSizes.open()
				}
			}
			MenuItem {
				text: qsTr("Co&lors...")
				onTriggered: {
					pixelColors.open()
				}
			}
		}
		Menu {
			title: qsTr("P&review")
			MenuItem {
				text: qsTr("&Update")
				onTriggered: {
					pixelator.setInputImage(inputFileGet.fileUrl)
					pixelator.setOutputImage(outputFileGet.fileUrl)
					pixelator.setStitchSizes(pixelSizes.resultWidth, pixelSizes.resultHeight, pixelSizes.stitchRows, pixelSizes.stitchColumns)
					pixelator.setStitchColors(pixelColors.color1, pixelColors.color2)
					pixelator.run()
				}
			}
		}
    }

	InputFileChooser {
		id: inputFileGet
		title: "Select File To Pixelate"
		onAccepted: {
		    inputImage.source = inputFileGet.fileUrl
		    outputImage.source = inputFileGet.fileUrl
		}
	}
	
	InputFileChooser {
		id: outputFileGet
		selectExisting: false
		title: "Select Store Path"
	}

	PixelSizes {
		id: pixelSizes
	}
	
	PixelColors {
		id: pixelColors
		color1: "white"
		color2: "black"
	}
	
	SplitView {
		id: imagePreview
		anchors.fill: parent

		Image {
			id: inputImage
			SplitView.minimumWidth: 200
			SplitView.preferredWidth: 400
			SplitView.maximumWidth: 600
			source: inputFileGet.fileUrl
			width: 400
		}

		Image {
			id: outputImage
			SplitView.minimumWidth: 200
			SplitView.preferredWidth: 400
			SplitView.maximumWidth: 600
			source: inputFileGet.fileUrl
			width: 400
		}

		// ...
	}
	QtPixelator {
		id: pixelator
	}
}