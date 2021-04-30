import QtQuick 2.0
//import ACF 1.0
//import "../qml"

Rectangle {
//    anchors.fill: parent
    width: parent.width //200
    height: 100
    color: "transparent"
//    property TreeItemModel contactInfoModel
    property var contactInfoModel

    Connections {
        target: contactInfoModel
        onStateChanged: {
           // if(contactInfoModel.state === "Ready"){
                tfName.text = contactInfoModel.getData("firstName")
                tfLastName.text = contactInfoModel.getData("lastName")
                tfNicName.text = contactInfoModel.getData("nickName")
            console.log("tfName.text ", tfName.text)
            //}
        }
    }

//    required contactInfoModel

    Column {
        width: parent.width
//        spacing: 10

        TextFieldCustom {
    	    id: tfName
            width: parent.width
            placeHolderText: qsTr("first name")
//            text: contactInfoModel.getData("firstName")
        }
        TextFieldCustom {
            id: tfLastName
            width: parent.width
            placeHolderText: qsTr("last name")
//            text: contactInfoModel.getData("secondName")
        }
       TextFieldCustom {
           id: tfNicName
            width: parent.width
            placeHolderText: qsTr("nic name")
//            text: contactInfoModel.getData("nickName")
        }
    }
}


