import QtQuick 2.12
import QtQuick.Controls 2.5

Page {
    width: 600
    height: 400

    title: qsTr("Page 2")

    Label {
        text: qsTr("A preview of the output should be displayed here")
        anchors.centerIn: parent
    }
}
