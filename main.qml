import QtQuick
import QtQuick.Window
import QtQuick.Layouts
import QtQuick.Controls
import QtMultimedia

Window {
    width: 720
    height: 360

    visible: true
    title: qsTr("Pilandok")

    ColumnLayout {
        anchors.fill: parent
        width: parent.width - 20
        anchors.topMargin: 10
        anchors.leftMargin: 10
        anchors.rightMargin: 10

        RowLayout {
            width: parent.width

            Text {
                text: qsTr("Address:")
            }

            TextField {
                enabled: !clientController.inputsLocked
                inputMethodHints: Qt.ImhFormattedNumbersOnly
                text: clientController.address
                onFocusChanged: clientController.address = text
            }
        }

        RowLayout {
            id: portRow
            width: parent.width
            Text {
                text: qsTr("Port:")
            }

            SpinBox {
                enabled: !clientController.inputsLocked
                from: 1
                to: 65535
                value: clientController.readPort
                onValueChanged: clientController.readPort = value
                editable: true
            }
        }

        RowLayout {
            id: cameraRow
            width: parent.width
            CheckBox {
                enabled: !clientController.inputsLocked
                id: allowLocalWebcam
                objectName: "allowLocalWebcam"
                text: qsTr("Allow use of webcam")
                onClicked: pilandokOutputManager.allowLocalWebcam = checked
                Component.onCompleted: checked = pilandokOutputManager.allowLocalWebcam
                onStateChanged: allowLocalWebcam.checked = pilandokOutputManager.allowLocalWebcam
            }

            MediaDevices { id: mediaDevices }

            ComboBox {
                id: currentCamera
                enabled: allowLocalWebcam.checked && allowLocalWebcam.enabled
                model: mediaDevices.videoInputs
                displayText: typeof currentValue === 'undefined' ? "Unavailable" : currentValue.description
                textRole: "description"
            }

            ComboBox {
                id: currentMic
                enabled: allowLocalWebcam.checked && allowLocalWebcam.enabled
                model: mediaDevices.audioInputs
                textRole: "description"
                displayText: typeof currentValue === 'undefined' ? "unavailable" : currentValue.description
            }
        }

        RowLayout {
            Button {
                enabled: !clientController.lockConnectButton
                id: connectButton
                text: clientController.showConnected ? qsTr("Disconnect") : qsTr("Connect")
                onClicked: clientController.toggleConnection()
            }

            Button {
                text: "Open logs file"
                onClicked: pilandokLogsUI.openLogsFile()
            }
        }

        ScrollView {
            Layout.fillHeight: true
            Layout.fillWidth: true
            Text {
                text: pilandokLogsUI == null ? "" : pilandokLogsUI.logString
            }
        }
    }
}
