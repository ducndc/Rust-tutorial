import QtQuick
import QtQuick.Controls
import Qt.labs.platform
import QtQuick.Layouts
import Backend

ApplicationWindow {
    visible: true
    width: 720
    height: 500
    title: "Anomaly Log Detection"

    LogAnalyzer {
        id: analyzer

        onAnalysisFinished: {
            statusLabel.text = message
        }

        onAnalysisError: {
            statusLabel.text = "Error: " + error
        }
    }

    property string selectedFile: ""

    Rectangle {
        anchors.fill: parent
        color: "#f5f5f5"

        ColumnLayout {
            anchors.centerIn: parent
            spacing: 20

            Label {
                text: "Anomaly Log Detection Tool"
                font.pixelSize: 24
                font.bold: true
                color: "#2c3e50"
                horizontalAlignment: Text.AlignHCenter
                Layout.alignment: Qt.AlignHCenter
            }

            ComboBox {
                id: deviceCombo
                model: ["APw6", "ONTw6", "XGSPONw6", "APw5", "ONTw5"]
                Layout.preferredWidth: 220
                Layout.alignment: Qt.AlignHCenter
            }

            Button {
                text: "Select Log File"
                Layout.preferredWidth: 220
                Layout.alignment: Qt.AlignHCenter
                onClicked: fileDialog.open()
            }

            Label {
                visible: selectedFile !== ""
                text: "File: " + selectedFile.replace("file://", "").split("/").pop()
                font.pixelSize: 14
                color: "#7f8c8d"
                Layout.alignment: Qt.AlignHCenter
            }

            Button {
                Layout.preferredWidth: 220
                Layout.alignment: Qt.AlignHCenter

                background: Rectangle {
                    color: "#3498db"
                    radius: 4
                }

                contentItem: Text {
                    text: "Run Detection"
                    anchors.centerIn: parent
                    color: "white"
                    font.pixelSize: 16
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                onClicked: {
                    if (selectedFile !== "") {
                        statusLabel.text = "Running..."
                        analyzer.analyze(selectedFile, deviceCombo.currentText)
                    } else {
                        statusLabel.text = "No file selected."
                    }
                }
            }

            Label {
                id: statusLabel
                text: "Ready"
                font.pixelSize: 16
                color: "#34495e"
                horizontalAlignment: Text.AlignHCenter
                Layout.alignment: Qt.AlignHCenter
            }

            FileDialog {
                id: fileDialog
                title: "Select Log Archive"
                nameFilters: ["All Files (*)"]
                onAccepted: {
                    selectedFile = fileDialog.file
                    statusLabel.text = "Selected File: " + selectedFile.replace("file://", "").split("/").pop()
                }
            }
        }
    }
}
