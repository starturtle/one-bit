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
      inputImage.source = inputFileGet.fileUrl
      outputImage.source = inputFileGet.fileUrl
    }
  }

  InputFileChooser {
    id: outputFileGet
    selectExisting: false
    title: "Select Store Path"
  }

  SplitView {
    anchors.fill: parent
    Image {
      id: inputImage
      SplitView.minimumWidth: 200
      SplitView.preferredWidth: 400
      SplitView.maximumWidth: 600
      source: inputFileGet.fileUrl
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
  property Image result: outputImage.data
  function getInputFile() {inputFileGet.open()}
  function getOutputFile() {outputFileGet.open()}
}
