import QtQuick 2.12
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import starturtle.stixelator 1.0

ApplicationWindow {
  id: mainWindow
  visible: true
  width: 800
  height: 600
  title: qsTr("Image Pixelation")

  menuBar: MenuBar {
    MenuBarItem {
      text: qsTr("&Open...")
      onTriggered: {
        imagePreview.getInputFile()
      }
    }
    MenuBarItem {
      text: qsTr("&Save as...")
      onTriggered: {
        imagePreview.getOutputFile()
      }
    }
    MenuBarItem {
      text: qsTr("&Quit")
      onTriggered: Qt.quit();
    }
  }

  GridLayout
  {
    columns: 3
    anchors.fill: parent
    PixelSizes {
      Layout.fillWidth: true
      id: pixelSizes
      onSizesChanged:
      {
        imagePreview.input.resultWidth = resultWidth
        imagePreview.input.resultHeight = resultHeight
        pixelator.setStitchSizes(resultWidth, resultHeight, stitchRows, stitchColumns)
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

    GridLines {
      id: gridLines
      Layout.fillWidth: true
      onSettingsChanged: {
        pixelator.setHelperSettings(gridEnabled, primary, secondary, gridCount)
        pixelator.run()
        console.log("Set grid settings to " + gridEnabled + ", " + primary + ", " + secondary + ", " + gridCount)
      }
    }
  
    FileDisplay {
      Layout.fillWidth: true
      Layout.fillHeight: true
      Layout.columnSpan: 3
      id: imagePreview
      onInputDataChanged:
      {
        pixelator.setInputImage(imagePreview.previewData)
        console.log("Updated input image, trigger pixelation")
        pixelator.run()
      }
      onClippingSizeChanged:
      {
        footer.update
      }
      onStoragePathSet:
      {
        pixelator.setStoragePath(storagePath)
        pixelator.commit()
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
      console.log("new pixelation created")
      imagePreview.updatePreview(pixelator.resultBuffer)
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