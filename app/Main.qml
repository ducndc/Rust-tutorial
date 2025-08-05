import QtQuick 2.15
import QtQuick.Controls 2.15
import Qt.labs.platform 1.1


ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: "Anomaly Log Detection"

    Column {
        anchors.centerIn: parent
        spacing: 12

        ComboBox {
            id: deviceCombo
            model: ["APw6", "ONTw6"]
            width: 200
        }

        Button {
            text: "Select Log File"
            onClicked: fileDialog.open()
        }

        Button {
            text: "Run Detection"
            onClicked: statusLabel.text = "Running..."
        }

        Label {
            id: statusLabel
            text: "Ready"
            font.pixelSize: 16
            color: "gray"
            horizontalAlignment: Text.AlignHCenter
            width: parent.width
        }

        FileDialog {
            id: fileDialog
            title: "Select Log File"
            nameFilters: ["All Files (*)"]
            onAccepted: statusLabel.text = "File Selected: " + fileDialog.fileUrl
        }
    }
}
