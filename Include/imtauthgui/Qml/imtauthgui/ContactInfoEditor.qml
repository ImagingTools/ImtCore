import QtQuick 2.0
//import '../../../imtgui/Qml/imtgui/AuxComponents'
import Acf 1.0

Rectangle {
    id: containerContactInfo;
   // width: parent.width;
    width: 500;
    height: 100;
   // color: "transparent";
    color: Style.backgroundColor;
//    property TreeItemModel contactInfoModel
    property var contactInfoModel;
    function updateData() {
        console.log("contactInfoModel.model", contactInfoModel.model);
//        tfName.text = contactInfoModel.GetData("firstName");
//        tfName.isTextChanged = false;
//        tfLastName.text = contactInfoModel.GetData("lastName");
//        tfLastName.isTextChanged = false;
//        tfNicName.text = contactInfoModel.GetData("nickName");
        tfcFirstNameText.text = contactInfoModel.GetData("firstName");
        tfcFirstNameText.isTextChanged = false;
        tfcLastNameText.text = contactInfoModel.GetData("lastName");
        tfcLastNameText.isTextChanged = false;
//        tfNicName.isTextChanged = false;
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
    Rectangle {
        id: container;
        width: 500;
        height: parent.height;
        color: parent.color;
        Text {
            id: titleAccountName;
            text: qsTr("Account name");
            color: Style.textColor;
            font.family: Style.fontFamily;
            anchors.top: container.top;
            anchors.topMargin: 10;
            anchors.left: container.left;
           // anchors.leftMargin: 10;
            font.pixelSize: Style.fontSize_subtitle;
        }

        TextFieldCustom {
            id: tfcAccountName;
            width: container.width;
            height: 30;
            anchors.top: titleAccountName.bottom;
            anchors.topMargin: 10;
            anchors.horizontalCenter: container.horizontalCenter;
            //anchors.verticalCenter: tfcEmail.verticalCenter;
        }

        Text {
            id: titleAccountDescription;
            text: qsTr("Account description");
            color: Style.textColor;
            font.family: Style.fontFamily;
            anchors.top: tfcAccountName.bottom;
            anchors.topMargin: 10;
            anchors.left: container.left;
           // anchors.leftMargin: 10;
            font.pixelSize: Style.fontSize_subtitle;
        }

        TextFieldCustom {
            id: tfcAccountDescription;
            width: container.width;
            height: 30;
            anchors.top: titleAccountDescription.bottom;
            anchors.topMargin: 10;
            anchors.horizontalCenter: container.horizontalCenter;
            //anchors.verticalCenter: tfcEmail.verticalCenter;
        }

        Text {
            id: companyAddressBlockTitle;
            text: qsTr("Company address");
            color: Style.textColor;
            //font.pixelSize: Style.fontSize_subtitle;
            font.family: Style.fontFamily;
            anchors.top: tfcAccountDescription.bottom;
            anchors.topMargin: 10;
            font.pixelSize: Style.fontSize_subtitle;
            //anchors.bottom: accountOwnerBlock.top;
        }

        Rectangle {
            id: companyAddressBlock;
            width: container.width;
            height: 400;
            anchors.top: companyAddressBlockTitle.bottom;
            anchors.topMargin: 5;
            //color: Style.baseColor;
            color: Style.imagingToolsGradient1;
            border.color: Style.theme == "Light" ? "#d0d0d2" : "#3a3b3b" ;

            Text {
                id: titleCountry;
                text: qsTr("Country");
                color: Style.textColor;
                font.family: Style.fontFamily;
                anchors.top: companyAddressBlock.top;
                anchors.topMargin: 10;
                anchors.left: companyAddressBlock.left;
                anchors.leftMargin: 10;
                font.pixelSize: Style.fontSize_subtitle;
            }

            Rectangle {
                id: tfcCountry;
                width: companyAddressBlock.width - 20;
                anchors.horizontalCenter: companyAddressBlock.horizontalCenter;
                height: 60;
                anchors.top: titleCountry.bottom;

                //color: Style.baseColor;
                color: Style.imagingToolsGradient1;
                border.color: Style.theme == "Light" ? "#d0d0d2" : "#3a3b3b" ;
                TextFieldCustom {
                    width: tfcCountry.width - 50;
                    height: 30;
                    anchors.horizontalCenter: tfcCountry.horizontalCenter;
                    anchors.verticalCenter: tfcCountry.verticalCenter;
                }
            }

            Text {
                id: titleCity;
                text: qsTr("City");
                color: Style.textColor;
                font.family: Style.fontFamily;
                anchors.top: tfcCountry.bottom;
                anchors.topMargin: 10;
                anchors.left: companyAddressBlock.left;
                anchors.leftMargin: 10;
                font.pixelSize: Style.fontSize_subtitle;
            }

            Rectangle {
                id: tfcCity;
                width: companyAddressBlock.width - 20;
                anchors.horizontalCenter: companyAddressBlock.horizontalCenter;
                height: 60;
                anchors.top: titleCity.bottom;

                //color: Style.baseColor;
                color: Style.imagingToolsGradient1;
                border.color: Style.theme == "Light" ? "#d0d0d2" : "#3a3b3b" ;
                TextFieldCustom {
                    width: tfcCity.width - 50;
                    height: 30;
                    anchors.horizontalCenter: tfcCity.horizontalCenter;
                    anchors.verticalCenter: tfcCity.verticalCenter;
                }
            }

            Text {
                id: titlePostalCode;
                text: qsTr("Postal code");
                color: Style.textColor;
                font.family: Style.fontFamily;
                anchors.top: tfcCity.bottom;
                anchors.topMargin: 10;
                anchors.left: companyAddressBlock.left;
                anchors.leftMargin: 10;
                font.pixelSize: Style.fontSize_subtitle;
            }

            Rectangle {
                id: tfcPostalCode;
                width: companyAddressBlock.width - 20;
                anchors.horizontalCenter: companyAddressBlock.horizontalCenter;
                height: 60;
                anchors.top: titlePostalCode.bottom;

                //color: Style.baseColor;
                color: Style.imagingToolsGradient1;
                border.color: Style.theme == "Light" ? "#d0d0d2" : "#3a3b3b" ;
                TextFieldCustom {
                    width: tfcPostalCode.width - 50;
                    height: 30;
                    anchors.horizontalCenter: tfcPostalCode.horizontalCenter;
                    anchors.verticalCenter: tfcPostalCode.verticalCenter;
                }
            }

            Text {
                id: titleStreet;
                text: qsTr("Street");
                color: Style.textColor;
                font.family: Style.fontFamily;
                anchors.top: tfcPostalCode.bottom;
                anchors.topMargin: 10;
                anchors.left: companyAddressBlock.left;
                anchors.leftMargin: 10;
                font.pixelSize: Style.fontSize_subtitle;
            }

            Rectangle {
                id: tfcStreet;
                width: companyAddressBlock.width - 20;
                anchors.horizontalCenter: companyAddressBlock.horizontalCenter;
                height: 60;
                anchors.top: titleStreet.bottom;

                //color: Style.baseColor;
                color: Style.imagingToolsGradient1;
                border.color: Style.theme == "Light" ? "#d0d0d2" : "#3a3b3b" ;
                TextFieldCustom {
                    width: tfcStreet.width - 50;
                    height: 30;
                    anchors.horizontalCenter: tfcStreet.horizontalCenter;
                    anchors.verticalCenter: tfcStreet.verticalCenter;
                }
            }
        }

        Text {
            id: accountOwnerBlockTitle;
            text: qsTr("Account Owner");
            color: Style.textColor;
            //font.pixelSize: Style.fontSize_subtitle;
            font.family: Style.fontFamily;
            anchors.top: companyAddressBlock.bottom;
            anchors.topMargin: 10;
            font.pixelSize: Style.fontSize_subtitle;
            //anchors.bottom: accountOwnerBlock.top;
        }

        Rectangle {
            id: accountOwnerBlock;
            width: container.width;
            height: 400;
            anchors.top: accountOwnerBlockTitle.bottom;
            anchors.topMargin: 5;
           // color: Style.baseColor;
            color: Style.imagingToolsGradient1;
            border.color: Style.theme == "Light" ? "#d0d0d2" : "#3a3b3b" ;

            Text {
                id: titleEmail;
                text: qsTr("Email");
                color: Style.textColor;
                font.family: Style.fontFamily;
                anchors.top: accountOwnerBlock.top;
                anchors.topMargin: 10;
                anchors.left: accountOwnerBlock.left;
                anchors.leftMargin: 10;
                font.pixelSize: Style.fontSize_subtitle;
            }

            Rectangle {
                id: tfcEmail;
                width: accountOwnerBlock.width - 20;
                anchors.horizontalCenter: accountOwnerBlock.horizontalCenter;
                height: 60;
                anchors.top: titleEmail.bottom;

                //color: Style.baseColor;
                color: Style.imagingToolsGradient1;
                border.color: Style.theme == "Light" ? "#d0d0d2" : "#3a3b3b" ;
                TextFieldCustom {
                    width: tfcEmail.width - 50;
                    height: 30;
                    anchors.horizontalCenter: tfcEmail.horizontalCenter;
                    anchors.verticalCenter: tfcEmail.verticalCenter;
                }
            }

            Text {
                id: titleFirstName;
                text: qsTr("First name");
                color: Style.textColor;
                font.family: Style.fontFamily;
                anchors.left: accountOwnerBlock.left;
                anchors.leftMargin: 10;
                anchors.top: tfcEmail.bottom;
                anchors.topMargin: 10;
                font.pixelSize: Style.fontSize_subtitle;
            }

            Rectangle {
                id: tfcFirstName;
                width: accountOwnerBlock.width - 20;
                anchors.horizontalCenter: accountOwnerBlock.horizontalCenter;
                height: 60;
                anchors.top: titleFirstName.bottom;
                //color: Style.baseColor;
                color: Style.imagingToolsGradient1;
                border.color: Style.theme == "Light" ? "#d0d0d2" : "#3a3b3b" ;
                TextFieldCustom {
                    id: tfcFirstNameText;
                    width: tfcFirstName.width - 50;
                    height: 30;
                    anchors.horizontalCenter: tfcFirstName.horizontalCenter;
                    anchors.verticalCenter: tfcFirstName.verticalCenter;
                }
            }

            Text {
                id: titleLastName;
                text: qsTr("Last name");
                color: Style.textColor;
                font.family: Style.fontFamily;
                anchors.left: accountOwnerBlock.left;
                anchors.leftMargin: 10;
                anchors.top: tfcFirstName.bottom;
                anchors.topMargin: 10;
                font.pixelSize: Style.fontSize_subtitle;
            }

            Rectangle {
                id: tfcLastName;
                width: accountOwnerBlock.width - 20;
                anchors.horizontalCenter: accountOwnerBlock.horizontalCenter;
                height: 60;
                anchors.top: titleLastName.bottom;
                //color: Style.baseColor;
                color: Style.imagingToolsGradient1;
                border.color: Style.theme == "Light" ? "#d0d0d2" : "#3a3b3b" ;
                TextFieldCustom {
                    id: tfcLastNameText;
                    width: tfcLastName.width - 50;
                    height: 30;
                    anchors.horizontalCenter: tfcLastName.horizontalCenter;
                    anchors.verticalCenter: tfcLastName.verticalCenter;
                }
            }
        }
    }
}


