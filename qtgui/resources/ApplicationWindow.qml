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
          pixelator.setInputImage(imagePreview.input.imageBuffer)
          pixelator.setOutputImage(imagePreview.storagePath)
          pixelator.setStitchSizes(pixelSizes.resultWidth, pixelSizes.resultHeight, pixelSizes.stitchRows, pixelSizes.stitchColumns)
          pixelator.setStitchColors(pixelColors.colors)
          var result = pixelator.run()
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
      onSizesChanged:
      {
        imagePreview.input.resultWidth = resultWidth
        imagePreview.input.resultHeight = resultHeight
        console.log("Set preview dimensions to " + imagePreview.input.resultWidth + "/" + imagePreview.input.resultHeight)
      }
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
        pixelator.setInputImage(imagePreview.input.imageBuffer)
        pixelator.setOutputImage(imagePreview.storagePath)
        dimensions = pixelSizes.dimensions
        pixelator.setStitchSizes(pixelSizes.resultWidth, pixelSizes.resultHeight, pixelSizes.stitchRows, pixelSizes.stitchColumns)
        pixelator.setStitchColors(pixelColors.colors)
        pixelator.run()
        outputImage.setData(pixelator.resultImage)
      }
    }
    Component.onCompleted:
    {
      imagePreview.input.resultWidth = pixelSizes.resultWidth
      imagePreview.input.resultHeight = pixelSizes.resultHeight
    }
  }
  QtPixelator {
    id: pixelator
    onPixelationCreated:
    {
      imagePreview.outputImage.setData(pixelator.resultImage)
    }
  }
}