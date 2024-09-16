import QtQuick 2.12

        Rectangle {
            Rectangle {
                height: 1
                width: parent.width
                color: Style.borderColor
            }
            Rectangle {
                anchors.bottom: parent.bottom
                height: 1
                width: parent.width
                color: Style.borderColor
            }
            Rectangle {
                anchors.horizontalCenter: parent.horizontalCenter
                height: parent.height
                width: 1
                color: Style.borderColor
            }
        }
