import QtQuick 2.0
//import '../../../imtgui/Qml/imtgui/AuxComponents'
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

Rectangle {
    id: containerContactInfo;
    anchors.fill: parent;
    // width: parent.width;
    width: 500;
    height: 100;
    // color: "transparent";
    color: Style.backgroundColor;
    //    property TreeItemModel contactInfoModel
    property var contactInfoModel;
    property string itemId;

    Component.onCompleted: {
        itemsModel.updateModel()
    }

    function updateData() {
        console.log("contactInfoModel.model", contactInfoModel.model);
        //        tfName.text = contactInfoModel.GetData("firstName");
        //        tfName.isTextChanged = false;
        //        tfLastName.text = contactInfoModel.GetData("lastName");
        //        tfLastName.isTextChanged = false;
        //        tfNicName.text = contactInfoModel.GetData("nickName");
        tfcFirstNameText.text = contactInfoModel.GetData("FirstName");
        tfcFirstNameText.isTextChanged = false;
        tfcLastNameText.text = contactInfoModel.GetData("LastName");
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

    GqlModel {
        id: itemsModel;

        function updateModel() {
            console.log( "updateModel AccountItem");

            var query = Gql.GqlRequest("query", "AccountItem");

            if(containerContactInfo.itemId != ""){
                var inputParams = Gql.GqlObject("input");
                inputParams.InsertField("Id");
                inputParams.InsertFieldArgument("Id", containerContactInfo.itemId);
                query.AddParam(inputParams);
            }

            var queryFields = Gql.GqlObject("item");


            //            itemModel->SetData("Id", accountId);
            //            itemModel->SetData("FirstName", firstName);
            //            itemModel->SetData("LastName", lastName);
            //            itemModel->SetData("NickName", nickName);
            //            itemModel->SetData("Email", mail);
            //            itemModel->SetData("AccountType", accountTypeId);
            //            itemModel->SetData("AccountDescription", accountDescription);

            queryFields.InsertField("Id");
            queryFields.InsertField("FirstName");
            queryFields.InsertField("LastName");
            queryFields.InsertField("NickName");
            queryFields.InsertField("Email");
            queryFields.InsertField("AccountType");
            queryFields.InsertField("AccountDescription");

            queryFields.InsertField("Street");


            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            console.log("AccountEdit query ", gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, itemsModel);
            if (this.state === "Ready"){

                var dataModelLocal = itemsModel.GetData("data");
                if(dataModelLocal.ContainsKey("AccountItem")){
                    dataModelLocal = dataModelLocal.GetData("AccountItem");
                    if(dataModelLocal !== null && dataModelLocal.ContainsKey("item")){
//                        dataModelLocal = dataModelLocal.GetData("item");
                        containerContactInfo.contactInfoModel = dataModelLocal.GetData("item");
                        dataModelLocal.RemoveData("item");
                        //                        tfcAccountName.text = dataModelLocal.GetData("FirstName")
//                        tfcFirstNameText.text = dataModelLocal.GetData("FirstName")
//                        tfcLastName.text = dataModelLocal.GetData("LastName")
//                        //                        tfcNickName.text = dataModelLocal.GetData("NickName")
//                        tfcEmail.text = dataModelLocal.GetData("Email")
//                        tfcAccountDescription.text = dataModelLocal.GetData("AccountDescription")
//                        tfcAccountName.text = dataModelLocal.GetData("AccountName")
                        //                        console.log("items",dataModelLocal);
                        //                        collectionView.table.elementsModel = dataModelLocal;
                    }
                    else if(itemsModel.ContainsKey("errors")){
                        var errorsModel = itemsModel.GetData("errors");
                        if(errorsModel !== null && errorsModel.ContainsKey(containerContactInfo.gqlModelItems)){
                            console.log("message", errorsModel.GetData(containerContactInfo.gqlModelItems).GetData("message"));
                        }
                    }
                }
            }
        }
    }


    //    Rectangle {
    //        id: cbTypeAccount;
    //        width: parent.width;
    //        height: 30;
    //        color: "gray";
    //    }

    ListModel {
        id: typeAccountModel;
        ListElement {
            text: "Personal";
        }

        ListElement {
            text: "Company";
        }
    }

    ListModel {
        id: genderModel;
        ListElement {
            text: "Diverse";
        }

        ListElement {
            text: "Male";
        }

        ListElement {
            text: "Female";
        }
    }

    Flickable {
        anchors.fill: parent;
        contentWidth: container.width;
        contentHeight: container.height + 50;
        boundsBehavior: Flickable.StopAtBounds;
        clip: true;

        Column {
            id: container;
            width: 500;
            spacing: 7;

            Text {
                id: titleAccountType;
                text: qsTr("Account type");
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }

            ComboBox {
                id: cbTypeAccount;
                z: 10;
                width: parent.width;
                height: 20;
                model: typeAccountModel;
                currentIndex: 0;
                textCentered: false;
                backgroundColor: "#d0d0d0";
                borderColor: Style.theme == "Dark" ? "#565757" : "#a4a4a6";
            }

            Text {
                id: titleAccountName;
                z: 5;
                text: qsTr("Account name");
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }

            TextFieldCustom {
                id: tfcAccountName;
                width: container.width;
                height: 23;
                anchors.horizontalCenter: container.horizontalCenter;
            }

            Text {
                id: titleAccountDescription;
                text: qsTr("Account description");
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }

            TextFieldCustom {
                id: tfcAccountDescription;
                width: container.width;
                height: 23;
                anchors.horizontalCenter: container.horizontalCenter;
            }

//            Column {
//                id: companyAddressColumn;
//                //anchors.top: tfcAccountDescription.bottom;
//                width: 500;
//                visible: cbTypeAccount.currentIndex === 1;

//                Text {
//                    id: companyAddressBlockTitle;
//                    text: qsTr("Company address");
//                    color: Style.textColor;
//                    font.family: Style.fontFamily;
//                    font.pixelSize: Style.fontSize_common;
//                    //visible: cbTypeAccount.currentIndex === 1;
//                }

//                Rectangle {
//                    width: 100;
//                    height: 100;
//                    color: "blue";
//                }

//                Rectangle {
//                    id: companyAddressBlock;
//                    anchors.fill: companyAddressColumn;
//                    color: "red";
////                    width: companyAddressColumn.width;
//                   // height: companyAddressColumn.height;
////                    width: container.width;
//                    //height: 285;
//    //                anchors.top: companyAddressBlockTitle.bottom;
//    //                anchors.topMargin: 5;
//                    //color: Style.baseColor;
//                   // color: Style.imagingToolsGradient1;
//                    border.color: Style.theme == "Light" ? "#d0d0d2" : "#3a3b3b" ;
//                   // visible: cbTypeAccount.currentIndex === 1;
//                    Text {
//                        id: titleCountry;
//                        text: qsTr("Country");
//                        color: Style.textColor;
//                        font.family: Style.fontFamily;
//                       // anchors.top: parent.top;
//                        anchors.topMargin: 5;
//                        anchors.left: companyAddressBlock.left;
//                        anchors.leftMargin: 10;
//                        font.pixelSize: Style.fontSize_common;
//                    }

//                    Rectangle {
//                        id: countryBlock;
//                        width: companyAddressBlock.width - 20;
//                        anchors.horizontalCenter: companyAddressBlock.horizontalCenter;
//                        height: 45;
//                       // anchors.top: titleCountry.bottom;

//                        //color: Style.baseColor;
//                        color: Style.imagingToolsGradient1;
//                        border.color: Style.theme == "Light" ? "#d0d0d2" : "#3a3b3b" ;
//                        TextFieldCustom {
//                            id: tfcCountryText;
//                            width: countryBlock.width - 22;
//                            height: 23;
//                            anchors.horizontalCenter: countryBlock.horizontalCenter;
//                            anchors.verticalCenter: countryBlock.verticalCenter;
//                        }
//                    }

//                    Text {
//                        id: titleCity;
//                        text: qsTr("City");
//                        color: Style.textColor;
//                        font.family: Style.fontFamily;
//                       // anchors.top: countryBlock.bottom;
//                        anchors.topMargin: 7;
//                        anchors.left: companyAddressBlock.left;
//                        anchors.leftMargin: 10;
//                        font.pixelSize: Style.fontSize_common;
//                    }

//                    Rectangle {
//                        id: cityBlock;
//                        width: companyAddressBlock.width - 20;
//                        anchors.horizontalCenter: companyAddressBlock.horizontalCenter;
//                        height: 45;
//                        //anchors.top: titleCity.bottom;

//                        //color: Style.baseColor;
//                        color: Style.imagingToolsGradient1;
//                        border.color: Style.theme == "Light" ? "#d0d0d2" : "#3a3b3b" ;
//                        TextFieldCustom {
//                            id: tfcCity;
//                            width: cityBlock.width - 22;
//                            height: 23;
//                            anchors.horizontalCenter: cityBlock.horizontalCenter;
//                            anchors.verticalCenter: cityBlock.verticalCenter;
//                        }
//                    }

//                    Text {
//                        id: titlePostalCode;
//                        text: qsTr("Postal code");
//                        color: Style.textColor;
//                        font.family: Style.fontFamily;
//                        //anchors.top: cityBlock.bottom;
//                        anchors.topMargin: 7;
//                        anchors.left: companyAddressBlock.left;
//                        anchors.leftMargin: 10;
//                        font.pixelSize: Style.fontSize_common;
//                    }

//                    Rectangle {
//                        id: postalCodeBlock;
//                        width: companyAddressBlock.width - 20;
//                        anchors.horizontalCenter: companyAddressBlock.horizontalCenter;
//                        height: 45;
//                       // anchors.top: titlePostalCode.bottom;

//                        //color: Style.baseColor;
//                        color: Style.imagingToolsGradient1;
//                        border.color: Style.theme == "Light" ? "#d0d0d2" : "#3a3b3b" ;
//                        TextFieldCustom {
//                            id: postalCode;
//                            width: postalCodeBlock.width - 22;
//                            height: 23;
//                            anchors.horizontalCenter: postalCodeBlock.horizontalCenter;
//                            anchors.verticalCenter: postalCodeBlock.verticalCenter;
//                        }
//                    }

//                    Text {
//                        id: titleStreet;
//                        text: qsTr("Street");
//                        color: Style.textColor;
//                        font.family: Style.fontFamily;
//                       // anchors.top: postalCodeBlock.bottom;
//                        //anchors.topMargin: 7;
//                        anchors.left: companyAddressBlock.left;
//                        anchors.leftMargin: 10;
//                        font.pixelSize: Style.fontSize_common;
//                    }

//                    Rectangle {
//                        id: streetBlock;
//                        width: companyAddressBlock.width - 20;
//                        anchors.horizontalCenter: companyAddressBlock.horizontalCenter;
//                        height: 45;
//                        //anchors.top: titleStreet.bottom;

//                        //color: Style.baseColor;
//                        color: Style.imagingToolsGradient1;
//                        border.color: Style.theme == "Light" ? "#d0d0d2" : "#3a3b3b" ;
//                        TextFieldCustom {
//                            id: tfcStreet;
//                            width: streetBlock.width - 22;
//                            text: containerContactInfo.contactInfoModel ? containerContactInfo.contactInfoModel.GetData("Street") : "";
//                            height: 23;
//                            anchors.horizontalCenter: streetBlock.horizontalCenter;
//                            anchors.verticalCenter: streetBlock.verticalCenter;
//                        }
//                    }
//                }

//                Text {
//                    id: title;
//                    text: qsTr("TEST");
//                    color: Style.textColor;
//                    font.family: Style.fontFamily;
//                   // anchors.top: postalCodeBlock.bottom;
//                    //anchors.topMargin: 7;
//                    anchors.left: companyAddressBlock.left;
//                    anchors.leftMargin: 10;
//                    font.pixelSize: Style.fontSize_common;
//                }

//                Rectangle {
//                    //anchors.fill: parent;
//                    color: "red";
//                    height: 100;
//                    width: 100;
//                }
     //       }

            Text {
                id: companyAddressBlockTitle;
                text: qsTr("Company address");
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
                visible: cbTypeAccount.currentIndex === 1;
            }

            Rectangle {
                id: companyAddressBlock;
                width: container.width;
                height: 300;
//                anchors.top: companyAddressBlockTitle.bottom;
//                anchors.topMargin: 5;
                //color: Style.baseColor;
                color: Style.imagingToolsGradient1;
                border.color: Style.theme == "Light" ? "#d0d0d2" : "#3a3b3b" ;
                visible: cbTypeAccount.currentIndex === 1;
                Text {
                    id: titleCountry;
                    text: qsTr("Country");
                    color: Style.textColor;
                    font.family: Style.fontFamily;
                    anchors.top: parent.top;
                    anchors.topMargin: 5;
                    anchors.left: companyAddressBlock.left;
                    anchors.leftMargin: 10;
                    font.pixelSize: Style.fontSize_common;
                }

                Rectangle {
                    id: countryBlock;
                    width: companyAddressBlock.width - 20;
                    anchors.horizontalCenter: companyAddressBlock.horizontalCenter;
                    height: 45;
                    anchors.top: titleCountry.bottom;

                    //color: Style.baseColor;
                    color: Style.imagingToolsGradient1;
                    border.color: Style.theme == "Light" ? "#d0d0d2" : "#3a3b3b" ;
                    TextFieldCustom {
                        id: tfcCountryText;
                        width: countryBlock.width - 22;
                        height: 23;
                        anchors.horizontalCenter: countryBlock.horizontalCenter;
                        anchors.verticalCenter: countryBlock.verticalCenter;
                    }
                }

                Text {
                    id: titleCity;
                    text: qsTr("City");
                    color: Style.textColor;
                    font.family: Style.fontFamily;
                    anchors.top: countryBlock.bottom;
                    anchors.topMargin: 7;
                    anchors.left: companyAddressBlock.left;
                    anchors.leftMargin: 10;
                    font.pixelSize: Style.fontSize_common;
                }

                Rectangle {
                    id: cityBlock;
                    width: companyAddressBlock.width - 20;
                    anchors.horizontalCenter: companyAddressBlock.horizontalCenter;
                    height: 45;
                    anchors.top: titleCity.bottom;

                    //color: Style.baseColor;
                    color: Style.imagingToolsGradient1;
                    border.color: Style.theme == "Light" ? "#d0d0d2" : "#3a3b3b" ;
                    TextFieldCustom {
                        id: tfcCity;
                        width: cityBlock.width - 22;
                        height: 23;
                        anchors.horizontalCenter: cityBlock.horizontalCenter;
                        anchors.verticalCenter: cityBlock.verticalCenter;
                    }
                }

                Text {
                    id: titlePostalCode;
                    text: qsTr("Postal code");
                    color: Style.textColor;
                    font.family: Style.fontFamily;
                    anchors.top: cityBlock.bottom;
                    anchors.topMargin: 7;
                    anchors.left: companyAddressBlock.left;
                    anchors.leftMargin: 10;
                    font.pixelSize: Style.fontSize_common;
                }

                Rectangle {
                    id: postalCodeBlock;
                    width: companyAddressBlock.width - 20;
                    anchors.horizontalCenter: companyAddressBlock.horizontalCenter;
                    height: 45;
                    anchors.top: titlePostalCode.bottom;

                    //color: Style.baseColor;
                    color: Style.imagingToolsGradient1;
                    border.color: Style.theme == "Light" ? "#d0d0d2" : "#3a3b3b" ;
                    TextFieldCustom {
                        id: postalCode;
                        width: postalCodeBlock.width - 22;
                        height: 23;
                        anchors.horizontalCenter: postalCodeBlock.horizontalCenter;
                        anchors.verticalCenter: postalCodeBlock.verticalCenter;
                    }
                }

                Text {
                    id: titleStreet;
                    text: qsTr("Street");
                    color: Style.textColor;
                    font.family: Style.fontFamily;
                    anchors.top: postalCodeBlock.bottom;
                    anchors.topMargin: 7;
                    anchors.left: companyAddressBlock.left;
                    anchors.leftMargin: 10;
                    font.pixelSize: Style.fontSize_common;
                }

                Rectangle {
                    id: streetBlock;
                    width: companyAddressBlock.width - 20;
                    anchors.horizontalCenter: companyAddressBlock.horizontalCenter;
                    height: 45;
                    anchors.top: titleStreet.bottom;

                    //color: Style.baseColor;
                    color: Style.imagingToolsGradient1;
                    border.color: Style.theme == "Light" ? "#d0d0d2" : "#3a3b3b" ;
                    TextFieldCustom {
                        id: tfcStreet;
                        width: streetBlock.width - 22;
                        text: containerContactInfo.contactInfoModel ? containerContactInfo.contactInfoModel.GetData("Street") : "";
                        height: 23;
                        anchors.horizontalCenter: streetBlock.horizontalCenter;
                        anchors.verticalCenter: streetBlock.verticalCenter;
                    }
                }
            }

            Text {
                id: accountOwnerBlockTitle;
                text: qsTr("Account Owner");
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }

            Rectangle {
                id: accountOwnerBlock;
                width: container.width;
                height: cbTypeAccount.currentIndex === 0 ? 680 : 250;
                color: Style.imagingToolsGradient1;
                border.color: Style.theme == "Light" ? "#d0d0d2" : "#3a3b3b" ;

                Text {
                    id: titleEmail;
                    text: qsTr("Email");
                    color: Style.textColor;
                    font.family: Style.fontFamily;
                    anchors.top: parent.top;
                    anchors.topMargin: 7;
                    anchors.left: accountOwnerBlock.left;
                    anchors.leftMargin: 10;
                    font.pixelSize: Style.fontSize_common;
                }

                Rectangle {
                    id: emailBlock;
                    width: accountOwnerBlock.width - 20;
                    anchors.horizontalCenter: accountOwnerBlock.horizontalCenter;
                    height: 45;
                    anchors.top: titleEmail.bottom;

                    //color: Style.baseColor;
                    color: Style.imagingToolsGradient1;
                    border.color: Style.theme == "Light" ? "#d0d0d2" : "#3a3b3b" ;
                    TextFieldCustom {
                        id: tfcEmail;
                        width: emailBlock.width - 22;
                        text: containerContactInfo.contactInfoModel ? containerContactInfo.contactInfoModel.GetData("Email") : "";
                        height: 23;
                        anchors.horizontalCenter: emailBlock.horizontalCenter;
                        anchors.verticalCenter: emailBlock.verticalCenter;
                    }
                }

                Text {
                    id: titleBD;
                    text: qsTr("BirthDay");
                    color: Style.textColor;
                    font.family: Style.fontFamily;
                    anchors.top: emailBlock.bottom;
                    anchors.topMargin: 7;
                    anchors.left: accountOwnerBlock.left;
                    anchors.leftMargin: 10;
                    font.pixelSize: Style.fontSize_common;

                    visible: cbTypeAccount.currentIndex === 0;
                }

                Rectangle {
                    id: bdBlock;
                    width: accountOwnerBlock.width - 20;
                    anchors.horizontalCenter: accountOwnerBlock.horizontalCenter;
                    height: 45;
                    anchors.top: titleBD.bottom;
                    visible: cbTypeAccount.currentIndex === 0;
                    //color: Style.baseColor;
                    color: Style.imagingToolsGradient1;
                    border.color: Style.theme == "Light" ? "#d0d0d2" : "#3a3b3b" ;
                    TextFieldCustom {
                        id: tfcBD;
                        width: bdBlock.width - 22;
                        text: containerContactInfo.contactInfoModel ? containerContactInfo.contactInfoModel.GetData("Email") : "";
                        height: 23;
                        anchors.horizontalCenter: bdBlock.horizontalCenter;
                        anchors.verticalCenter: bdBlock.verticalCenter;
                    }
                }

                Text {
                    id: titleGender;
                    text: qsTr("Gender");
                    color: Style.textColor;
                    font.family: Style.fontFamily;
                    anchors.top: bdBlock.bottom;
                    anchors.topMargin: 7;
                    anchors.left: accountOwnerBlock.left;
                    anchors.leftMargin: 10;
                    font.pixelSize: Style.fontSize_common;
                    visible: cbTypeAccount.currentIndex === 0;
                }

                Rectangle {
                    id: genderBlock;
                    z: 10;
                    width: accountOwnerBlock.width - 20;
                    anchors.horizontalCenter: accountOwnerBlock.horizontalCenter;
                    height: 45;
                    anchors.top: titleGender.bottom;
                    visible: cbTypeAccount.currentIndex === 0;
                    //color: Style.baseColor;
                    color: Style.imagingToolsGradient1;
                    border.color: Style.theme == "Light" ? "#d0d0d2" : "#3a3b3b" ;

                    ComboBox {
                        id: genderCB;
                        width: bdBlock.width - 22;
                        height: 20;
                        model: genderModel;
                        anchors.horizontalCenter: genderBlock.horizontalCenter;
                        anchors.verticalCenter: genderBlock.verticalCenter;
                        backgroundColor: "#d0d0d0";
                        borderColor: Style.theme == "Dark" ? "#565757" : "#a4a4a6";
                        textCentered: false;
                    }

//                    TextFieldCustom {
//                        id: tfcGender;
//                        width: bdBlock.width - 22;
//                        text: containerContactInfo.contactInfoModel ? containerContactInfo.contactInfoModel.GetData("Email") : "";
//                        height: 23;
//                        anchors.horizontalCenter: genderBlock.horizontalCenter;
//                        anchors.verticalCenter: genderBlock.verticalCenter;
//                    }
                }

                Text {
                    id: titleFirstName;
                    z: 5;
                    text: qsTr("First name");
                    color: Style.textColor;
                    font.family: Style.fontFamily;
                    anchors.left: accountOwnerBlock.left;
                    anchors.leftMargin: 7;
                    anchors.top: cbTypeAccount.currentIndex === 0 ? genderBlock.bottom : emailBlock.bottom;
                    anchors.topMargin: 10;
                    font.pixelSize: Style.fontSize_common;
                }

                Rectangle {
                    id: firstNameBlock;
                    z: 5;
                    width: accountOwnerBlock.width - 20;
                    anchors.horizontalCenter: accountOwnerBlock.horizontalCenter;
                    height: 45;
                    anchors.top: titleFirstName.bottom;
                    //color: Style.baseColor;
                    color: Style.imagingToolsGradient1;
                    border.color: Style.theme == "Light" ? "#d0d0d2" : "#3a3b3b" ;
                    TextFieldCustom {
                        id: tfcFirstNameText;
                        width: firstNameBlock.width - 22;
                        text: containerContactInfo.contactInfoModel ? containerContactInfo.contactInfoModel.GetData("FirstName") : "";
                        height: 23;
                        anchors.horizontalCenter: firstNameBlock.horizontalCenter;
                        anchors.verticalCenter: firstNameBlock.verticalCenter;
                    }
                }

                Text {
                    id: titleLastName;
                    text: qsTr("Last name");
                    color: Style.textColor;
                    font.family: Style.fontFamily;
                    anchors.left: accountOwnerBlock.left;
                    anchors.leftMargin: 10;
                    anchors.top: firstNameBlock.bottom;
                    anchors.topMargin: 7;
                    font.pixelSize: Style.fontSize_common;
                }

                Rectangle {
                    id: lastNameBlock;
                    width: accountOwnerBlock.width - 20;
                    anchors.horizontalCenter: accountOwnerBlock.horizontalCenter;
                    height: 45;
                    anchors.top: titleLastName.bottom;
                    //color: Style.baseColor;
                    color: Style.imagingToolsGradient1;
                    border.color: Style.theme == "Light" ? "#d0d0d2" : "#3a3b3b" ;
                    TextFieldCustom {
                        id: tfcLastName;
                        width: lastNameBlock.width - 22;
                        text: containerContactInfo.contactInfoModel ? containerContactInfo.contactInfoModel.GetData("LastName") : "";
                        height: 23;
                        anchors.horizontalCenter: lastNameBlock.horizontalCenter;
                        anchors.verticalCenter: lastNameBlock.verticalCenter;
                    }
                }

                Text {
                    id: titleNickName;
                    text: qsTr("Nickname");
                    color: Style.textColor;
                    font.family: Style.fontFamily;
                    anchors.left: accountOwnerBlock.left;
                    anchors.leftMargin: 10;
                    anchors.top: lastNameBlock.bottom;
                    anchors.topMargin: 7;
                    font.pixelSize: Style.fontSize_common;
                    visible: cbTypeAccount.currentIndex === 0;
                }

                Rectangle {
                    id: nickNameBlock;
                    width: accountOwnerBlock.width - 20;
                    anchors.horizontalCenter: accountOwnerBlock.horizontalCenter;
                    height: 45;
                    anchors.top: titleNickName.bottom;
                    //color: Style.baseColor;
                    visible: cbTypeAccount.currentIndex === 0;
                    color: Style.imagingToolsGradient1;
                    border.color: Style.theme == "Light" ? "#d0d0d2" : "#3a3b3b" ;
                    TextFieldCustom {
                        id: tfcNickName;
                        width: nickNameBlock.width - 22;
                        text: containerContactInfo.contactInfoModel ? containerContactInfo.contactInfoModel.GetData("LastName") : "";
                        height: 23;
                        anchors.horizontalCenter: nickNameBlock.horizontalCenter;
                        anchors.verticalCenter: nickNameBlock.verticalCenter;
                    }
                }

                Text {
                    id: titleAddresses;
                    text: qsTr("Addresses");
                    color: Style.textColor;
                    font.family: Style.fontFamily;
                    anchors.top: nickNameBlock.bottom;
                    anchors.topMargin: 7;
                    anchors.left: accountOwnerBlock.left;
                    anchors.leftMargin: 10;
                    font.pixelSize: Style.fontSize_common;
                    visible: cbTypeAccount.currentIndex === 0;
                }

                Rectangle {
                    id: addressesBlock;
                    width: accountOwnerBlock.width - 20;
                    anchors.horizontalCenter: accountOwnerBlock.horizontalCenter;
                    height: 200;
                    anchors.top: titleAddresses.bottom;
                    //color: Style.baseColor;
                    visible: cbTypeAccount.currentIndex === 0;
                    color: Style.imagingToolsGradient1;
                    border.color: Style.theme == "Light" ? "#d0d0d2" : "#3a3b3b" ;

                    Rectangle {
                        id: addressesBlockButtons;
                        width: 50;
                        height: 20;
                        color: "transparent";
                        anchors.top: addressesBlock.top;
                        anchors.topMargin: 10;
                        anchors.left: addressesBlock.left;
                        anchors.leftMargin: 10;

                        AuxButton {
                            id: addButton;
                            anchors.verticalCenter: addressesBlockButtons.verticalCenter;
                            anchors.left: addressesBlockButtons.left;
                            width: 15;
                            height: 15;
                            iconSource: "../../../Icons/" + Style.theme + "/Add_On_Normal.svg";
                        }

                        AuxButton {
                            id: removeButton;
                            anchors.verticalCenter: addressesBlockButtons.verticalCenter;
                            anchors.right: addressesBlockButtons.right;
                            width: 15;
                            height: 15;
                            iconSource: "../../../Icons/" + Style.theme + "/Remove_On_Normal.svg";
                        }
                    }

                    AuxTable {
                        id: addressesTable;
                        anchors.top: addressesBlockButtons.bottom;
                        anchors.topMargin: 5;
                        anchors.bottom: parent.bottom;
                        anchors.left: parent.left;
                        anchors.leftMargin: 10;
                        anchors.right: parent.right;
                        anchors.rightMargin: 10;

//                        anchors.fill: parent;
                        headersModel: headersModel;
                    }

                    ListModel {
                        id: headersModel;
                        ListElement {
                            Name: "Country";
                        }

                        ListElement {
                            Name: "City";
                        }

                        ListElement {
                            Name: "Postal code";
                        }

                        ListElement {
                            Name: "Street";
                        }
                    }


//                    TextFieldCustom {
//                        id: tfcNickName;
//                        width: nickNameBlock.width - 22;
//                        text: containerContactInfo.contactInfoModel ? containerContactInfo.contactInfoModel.GetData("LastName") : "";
//                        height: 23;
//                        anchors.horizontalCenter: nickNameBlock.horizontalCenter;
//                        anchors.verticalCenter: nickNameBlock.verticalCenter;
//                    }
                }


            }
        }
    }
}


