import QtQuick 2.0
//import ACF 1.0
//import "../qml"

Rectangle {
    id: container
//    anchors.fill: parent
    width: parent.width //200
    height: 100
    color: "transparent"
//    property TreeItemModel contactInfoModel
    property var contactInfoModel
    function updateData() {
        tfName.text = contactInfoModel.GetData("firstName")
        tfLastName.text = contactInfoModel.GetData("lastName")
        tfNicName.text = contactInfoModel.GetData("nickName")
        console.log("tfName.text ", tfName.text)

    }

    onContactInfoModelChanged: {
        container.updateData()
    }

    Connections {
        target: contactInfoModel
        onStateChanged: {
            if(contactInfoModel.state === "Ready"){
                container.updateData()
            }
        }
    }


    Column {
        width: parent.width

        TextFieldCustom {
    	    id: tfName
            width: parent.width
            placeHolderText: qsTr("first name")
            onAccepted: {
                contactInfoModel.SetData("firstName", tfName.text)
            }
        }
        TextFieldCustom {
            id: tfLastName
            width: parent.width
            placeHolderText: qsTr("last name")
        }
       TextFieldCustom {
           id: tfNicName
            width: parent.width
            placeHolderText: qsTr("nic name")
        }
    }
}


