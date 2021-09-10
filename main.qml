import QtQuick
import QtQuick.Window
import QtQuick.Layouts
import QtQuick.Controls

Window {
    width: 320
    height: 240

    maximumWidth: 320
    maximumHeight: 240

    minimumWidth: 320
    minimumHeight: 240

    visible: true
    title: qsTr("Pilanduk")

    ColumnLayout {
        width: parent.width
        height: parent.height

        RowLayout {
            id: ipRow
            width: parent.width

            Text {
                text: qsTr("IP Address:")
            }

            TextField {
                validator: RegularExpressionValidator {
                    // TODO: This only works on ipv4 addresses
                    // Its fine for now as a POC, but this needs to be changed in the future to adapt to ipv6
                    regularExpression: /^(([01]?[0-9]?[0-9]|2([0-4][0-9]|5[0-5]))\.){3}([01]?[0-9]?[0-9]|2([0-4][0-9]|5[0-5]))$/
                }
                inputMethodHints: Qt.ImhFormattedNumbersOnly
                text: "192.168.1.5"
            }
        }

        RowLayout {
            id: portRow
            width: parent.width
            Text {
                text: qsTr("Port:")
            }

            SpinBox {
                from: 1
                to: 65535
                value: 4318
                editable: true
            }
        }

        CheckBox {
            id: allowLocalWebcam
            objectName: "allowLocalWebcam"
            text: qsTr("Allow use of webcam")
        }

        Button {
            id: connectButton
            text: qsTr("Connect to Kijang")
        }

        ScrollView {
            Layout.fillHeight: true
            width: parent.width
            Text {
                id: kijangStatus
                objectName: "kijangStatus"
            }
        }
    }
}
