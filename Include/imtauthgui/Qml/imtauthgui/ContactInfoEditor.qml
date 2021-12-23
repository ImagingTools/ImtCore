import QtQuick 2.0
//import '../../../imtgui/Qml/imtgui/AuxComponents'
//import acf 1.0

Rectangle {
    id: container;
   // width: parent.width;
    width: 300;
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

//    ComboBox {
//        id: cbTypeAccount;
//        width: parent.width;
//        height: 30;
//        model: typeAccountModel;
//    }

//    Rectangle {
//        id: cbTypeAccount;
//        width: parent.width;
//        height: 30;
//        color: "gray";
//    }

//    ListModel {
//        id: typeAccountModel;
//        ListElement {
//            text: "Personal";
//        }

//        ListElement {
//            text: "Company";
//        }
//    }

    Column {
        id: accountInfo;
        width: parent.width;

        anchors.top: cbTypeAccount.bottom;

        Text {
            id: accountNameTitle;
            text: qsTr("Account name");
        }

        TextFieldCustom {
            id: tfAccountName;
            width: parent.width;
            anchors.top: accountNameTitle.bottom;
          //  placeHolderText: qsTr("Account name");
            onAccepted: {
                contactInfoModel.SetData("accountName", tfAccountName.text);
            }
        }

        Text {
            id: accountDescriptionTitle;

            anchors.top: tfAccountName.bottom;
            text: qsTr("Account description");
        }

        TextFieldCustom {
            id: tfAccountDecription;
            width: parent.width;
            anchors.top: accountDescriptionTitle.bottom;
           // placeHolderText: qsTr("Account description");
            onAccepted: {
                contactInfoModel.SetData("accountDescription", tfAccountDecription.text);
            }
        }
    }



    Rectangle {
        id: rect;
        anchors.top: tfAccountDecription.bottom;
        Text {
            id: accountOwnerTitle;
            text: qsTr("Account Owner");

        }
    }

//    Rectangle {
//        id: accountOwnerBlock;

//        anchors.top: accountOwnerTitle.bottom;
//        Text {
//            id: accountOwnerBlockTitle;
//            text: qsTr("Account Owner");
//            anchors.bottom: accountOwnerBlock.top;
//        }

//        Rectangle {
//            id: containerAccountOwner;
//            anchors.fill: accountOwnerBlock;
//            border.color: "black";

//            Column {
//                width: parent.width;

//                Rectangle {
//                    id: emailBlock;

//                    Text {
//                        id: emailBlockTitle;
//                        text: qsTr("Email");
//                    }

//                    TextFieldCustom {
//                        id: tfEmail;
//                        width: parent.width;
//                        //placeHolderText: qsTr("Email");
//                        anchors.top: emailBlockTitle.bottom;
//                        onAccepted: {
//                            contactInfoModel.SetData("email", tfEmail.text);
//                        }
//                    }
//                }
//            }
//        }
//    }

//    Column {
//        width: parent.width;



//        TextFieldCustom {
//            id: tfName;
//            width: parent.width;
//            placeHolderText: qsTr("First name");
//            onAccepted: {
//                contactInfoModel.SetData("firstName", tfName.text);
//            }
//        }

//        TextFieldCustom {
//            id: tfLastName;
//            width: parent.width;
//            placeHolderText: qsTr("Last name");
//            onAccepted: {
//                contactInfoModel.SetData("lastName", tfLastName.text);
//            }
//        }

//       TextFieldCustom {
//           id: tfNicName;
//            width: parent.width;
//            placeHolderText: qsTr("Nickname");
//            onAccepted: {
//                contactInfoModel.SetData("nickName", tfNicName.text);
//            }
//        }

//    }
}


