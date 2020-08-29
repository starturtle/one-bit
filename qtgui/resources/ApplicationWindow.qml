import QtQuick 2.12
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
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
          imagePreview.getInputFile()
        }
      }
      MenuItem {
        text: qsTr("Select Out&path")
        onTriggered: {
          imagePreview.getOutputFile()
        }
      }
      MenuItem {
        text: qsTr("Exit")
        onTriggered: Qt.quit();
      }
    }
    Menu {
      title: qsTr("&Pixelation")
      MenuItem {
        text: qsTr("&Run")
        onTriggered: {
          pixelator.setInputImage(imagePreview.sourcePath)
          pixelator.setOutputImage(imagePreview.storagePath)
          pixelator.setStitchSizes(pixelSizes.resultWidth, pixelSizes.resultHeight, pixelSizes.stitchRows, pixelSizes.stitchColumns)
          pixelator.setStitchColors(pixelColors.colors)
          var result = pixelator.run()
          imagePreview.outputImage.data = pixelator.result
        }
      }
    }
  }

  GridLayout
  {
    columns: 2
    anchors.fill: parent
    PixelSizes {
      Layout.fillWidth: true
      id: pixelSizes
    }
  
    PixelColors {
      id: pixelColors
      Layout.fillWidth: true
    }
  
    FileDisplay {
      Layout.fillWidth: true
      Layout.fillHeight: true
      Layout.columnSpan: 2
      id: imagePreview
      onInputDataChanged:
      {
        pixelator.setInputImage(imagePreview.sourcePath)
        pixelator.setOutputImage(imagePreview.storagePath)
        pixelator.setStitchSizes(pixelSizes.resultWidth, pixelSizes.resultHeight, pixelSizes.stitchRows, pixelSizes.stitchColumns)
        pixelator.setStitchColors(pixelColors.colors)
        pixelator.run()
        imagePreview.outputImage.data = pixelator.result
      }
    }
  }
  QtPixelator {
    id: pixelator
  }
}