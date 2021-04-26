import QtQuick 2.0
import ACF 1.0
//import "../qml"

Rectangle {
    anchors.fill: parent
    width: 200
    height: 100
    color: "transparent"
    property TreeItemModel contactInfoModel

    required contactInfoModel

    Column {
        width: parent.width
//        spacing: 10

        TextFieldCustom {
            width: parent.width
            placeHolderText: qsTr("first name")
            text: contactInfoModel.getData("firstName")
        }
        TextFieldCustom {
            width: parent.width
            placeHolderText: qsTr("last name")
            text: contactInfoModel.getData("lastName")
        }
        TextFieldCustom {
            width: parent.width
            placeHolderText: qsTr("nic name")
            text: contactInfoModel.getData("nicName")
        }
    }
}


