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
          pixelator.setStoragePath(imagePreview.storagePath)
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
          var result = pixelator.commit()
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
        pixelator.setStitchSizes(pixelSizes.resultWidth, pixelSizes.resultHeight, pixelSizes.stitchRows, pixelSizes.stitchColumns)
        pixelator.run()
        console.log("Set preview dimensions to " + imagePreview.input.resultWidth + "/" + imagePreview.input.resultHeight)
      }
    }
  
    PixelColors {
      id: pixelColors
      Layout.fillWidth: true
      onColorsChanged: {
        pixelator.setStitchColors(pixelColors.colors)
        pixelator.run()
        console.log("Set colors to " + pixelColors.colors)
      }
    }
  
    FileDisplay {
      Layout.fillWidth: true
      Layout.fillHeight: true
      Layout.columnSpan: 2
      id: imagePreview
      onInputDataChanged:
      {
        pixelator.setInputImage(imagePreview.input.imageBuffer)
        dimensions = pixelSizes.dimensions
        pixelator.run()
      }
      onClippingSizeChanged:
      {
        footer.update
      }
    }
    Component.onCompleted: {
      imagePreview.input.resultWidth = pixelSizes.resultWidth
      imagePreview.input.resultHeight = pixelSizes.resultHeight
    }
    onHeightChanged: {
      imagePreview.height = contentItem.height - pixelColors.height
    }
    onWidthChanged: {
      imagePreview.width = contentItem.width
    }
  }
  QtPixelator {
    id: pixelator
    onPixelationCreated: {
      imagePreview.updatePreview(pixelator.resultImage)
    }
  }
  footer: ToolBar {
    RowLayout {
      anchors.fill: parent
      Label { text: imagePreview.clippingInfo }
    }
  }
  Component.onCompleted: {
    pixelator.setStitchSizes(pixelSizes.resultWidth, pixelSizes.resultHeight, pixelSizes.stitchRows, pixelSizes.stitchColumns)
    console.log("Initialize stitch counts to " + pixelSizes.stitchColumns + "M " + pixelSizes.stitchRows + "R, totaling " + pixelSizes.resultWidth + "x" + pixelSizes.resultHeight + "cm")
    pixelator.setStitchColors(pixelColors.colors)
    console.log("Initialize colors to " + pixelColors.colors)
  }
}