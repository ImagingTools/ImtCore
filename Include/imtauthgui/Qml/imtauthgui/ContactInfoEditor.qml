import QtQuick 2.0
//import ACF 1.0

Rectangle {
    id: container;
    width: parent.width;
    height: 100;
    color: "transparent";
//    property TreeItemModel contactInfoModel
    property var contactInfoModel;
    function updateData() {
        console.log("contactInfoModel.model", contactInfoModel.model);
        tfName.text = contactInfoModel.GetData("firstName");
        tfName.isTextChanged = false;
        tfLastName.text = contactInfoModel.GetData("lastName");
        tfLastName.isTextChanged = false;
        tfNicName.text = contactInfoModel.GetData("nickName");
        tfNicName.isTextChanged = false;
    }

    function updateShortData(shortData) {
        var key = shortData["key"];
//        console.log("key", key)
        if (key === "firstName"){
            tfName.text = shortData["newVal"];
            tfName.isTextChanged = false;
        }
        if (key === "lastName"){
            tfLastName.text = shortData["newVal"];
            tfLastName.isTextChanged = false;
        }
        if (key === "nickName"){
            tfNicName.text = shortData["newVal"];
            tfNicName.isTextChanged = false;
        }
    }

    onContactInfoModelChanged: {
        container.updateData();
    }

//    Connections {
//        target: contactInfoModel;
//        onStateChanged: {
//            console.log("ModelState", contactInfoModel.state);
//            if(contactInfoModel.state === "Ready"){
//                container.updateData();
//            }
//        }
//    }

    Column {
        width: parent.width;

        TextFieldCustom {
            id: tfName;
            width: parent.width;
            placeHolderText: qsTr("first name");
            onAccepted: {
                contactInfoModel.SetData("firstName", tfName.text);
            }
        }
        TextFieldCustom {
            id: tfLastName;
            width: parent.width;
            placeHolderText: qsTr("last name");
            onAccepted: {
                contactInfoModel.SetData("lastName", tfLastName.text);
            }
        }
       TextFieldCustom {
           id: tfNicName;
            width: parent.width;
            placeHolderText: qsTr("nic name");
            onAccepted: {
                contactInfoModel.SetData("nickName", tfNicName.text);
            }
        }

    }
}


