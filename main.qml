import QtQuick
import QtQuick.Window
import QtQuick.Layouts
import QtQuick.Controls

Window {
    width: 640
    height: 480

    maximumWidth: 640
    maximumHeight: 480

    minimumWidth: 640
    minimumHeight: 480

    visible: true
    title: qsTr("Pilanduk")

    ColumnLayout {
        width: parent.width
        height: parent.height

        RowLayout {
            width: parent.width

            Text {
                text: qsTr("Address:")
            }

            TextField {
                enabled: !(kijangClient.connected || kijangClient.attemptingConnection)
                inputMethodHints: Qt.ImhFormattedNumbersOnly
                text: kijangClient.address
                onFocusChanged: kijangClient.address = text
            }
        }

        RowLayout {
            id: portRow
            width: parent.width
            Text {
                text: qsTr("Port:")
            }

            SpinBox {
                enabled: !(kijangClient.connected || kijangClient.attemptingConnection)
                from: 1
                to: 65535
                value: kijangClient.port
                onValueChanged: kijangClient.port = value
                editable: true
            }
        }

        CheckBox {
            enabled: !(kijangClient.connected || kijangClient.attemptingConnection)
            id: allowLocalWebcam
            objectName: "allowLocalWebcam"
            text: qsTr("Allow use of webcam")
            onClicked: pilandukOutputManager.allowLocalWebcam = checked
            Component.onCompleted: checked = pilandukOutputManager.allowLocalWebcam
            onStateChanged: allowLocalWebcam.checked = pilandukOutputManager.allowLocalWebcam
        }

        Button {
            enabled: !kijangClient.attemptingConnection
            id: connectButton
            text: kijangClient.buttonString
            onClicked: kijangClient.toggleConnection()
        }

        ScrollView {
            Layout.fillHeight: true
            width: parent.width
            Text {
                text: pilandukLogsUI.logString
            }
        }
    }
}
