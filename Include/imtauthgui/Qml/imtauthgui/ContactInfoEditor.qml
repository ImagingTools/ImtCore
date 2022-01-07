import QtQuick 2.0
//import '../../../imtgui/Qml/imtgui/AuxComponents'
import Acf 1.0
import imtqml 1.0

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

            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            console.log("PackageView query ");
            console.log("PackageView query ", gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, itemsModel);
            if (this.state === "Ready"){
                var dataModelLocal = itemsModel.GetData("data");
                if(dataModelLocal.ContainsKey("AccountItem")){
                    dataModelLocal = dataModelLocal.GetData("AccountItem");
                    if(dataModelLocal !== null && dataModelLocal.ContainsKey("item")){
                        dataModelLocal = dataModelLocal.GetData("item");
                        //                        tfcAccountName.text = dataModelLocal.GetData("FirstName")
                        tfcFirstName.text = dataModelLocal.GetData("FirstName")
                        tfcLastName.text = dataModelLocal.GetData("LastName")
                        //                        tfcNickName.text = dataModelLocal.GetData("NickName")
                        tfcEmail.text = dataModelLocal.GetData("FirstName")
                        tfcAccountDescription.text = dataModelLocal.GetData("AccountDescription")
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
                id: titleAccountName;
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

            Text {
                id: companyAddressBlockTitle;
                text: qsTr("Company address");
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }

            Rectangle {
                id: companyAddressBlock;
                width: container.width;
                height: 285;
//                anchors.top: companyAddressBlockTitle.bottom;
//                anchors.topMargin: 5;
                //color: Style.baseColor;
                color: Style.imagingToolsGradient1;
                border.color: Style.theme == "Light" ? "#d0d0d2" : "#3a3b3b" ;

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
                height: 220;
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
                        height: 23;
                        anchors.horizontalCenter: emailBlock.horizontalCenter;
                        anchors.verticalCenter: emailBlock.verticalCenter;
                    }
                }

                Text {
                    id: titleFirstName;
                    text: qsTr("First name");
                    color: Style.textColor;
                    font.family: Style.fontFamily;
                    anchors.left: accountOwnerBlock.left;
                    anchors.leftMargin: 7;
                    anchors.top: emailBlock.bottom;
                    anchors.topMargin: 10;
                    font.pixelSize: Style.fontSize_common;
                }

                Rectangle {
                    id: firstNameBlock;
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
                        height: 23;
                        anchors.horizontalCenter: lastNameBlock.horizontalCenter;
                        anchors.verticalCenter: lastNameBlock.verticalCenter;
                    }
                }
            }
        }
    }
}


