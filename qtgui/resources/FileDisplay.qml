import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Dialogs 1.3
import starturtle.oneBit 1.0

Frame {
  InputFileChooser {
    id: inputFileGet
    title: "Select File To Pixelate"
    onAccepted: {
      inputImage.path = inputFileGet.fileUrl
    }
  }

  InputFileChooser {
    id: outputFileGet
    selectExisting: false
    title: "Select Store Path"
  }

  SplitView {
    anchors.fill: parent
    SourceImage {
      id: inputImage
      path: inputFileGet.fileUrl
      SplitView.minimumWidth: 200
      SplitView.preferredWidth: 400
      SplitView.maximumWidth: 600
      onDataChanged: {
        outputImage.setData(imageBuffer)
      }
    }

    ResultImage {
      id: outputImage
      SplitView.minimumWidth: 200
      SplitView.preferredWidth: 400
      SplitView.maximumWidth: 600
    }
  }
  property var sourcePath: inputFileGet.fileUrl
  property var storagePath: outputFileGet.fileUrl
  function getInputFile() {inputFileGet.open()}
  function getOutputFile() {outputFileGet.open()}
  function updatePreview(image) {outputImage.setData(image)}
  property var input: inputImage
  signal inputDataChanged()

  Component.onCompleted:
  {
    inputImage.pathChanged.connect(inputDataChanged)
  }
}