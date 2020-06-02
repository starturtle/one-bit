import QtQuick 2.12
import QtQuick.Controls 2.14
import QtQuick.Dialogs 1.3

FileDialog {
    title: "Please choose a file"
    folder: shortcuts.pictures
    nameFilters: [ "Image files (*.jpg *.png)", "All files (*)" ]
    visible: false
    selectFolder: false
    onAccepted: {
        close()
    }
    onRejected: {
        console.log("Canceled")
        close()
    }
}