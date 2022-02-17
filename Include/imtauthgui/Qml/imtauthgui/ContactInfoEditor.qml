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
    property TreeItemModel contactInfoModel;
//    property var contactInfoModel;
    property TreeItemModel model;
    property Item multiDocViewItem;

    property string gqlModelQueryType;
    property string gqlModelQueryTypeNotification;

    property Item rootItem;
    property string accountType;
    property string itemId;
    property string itemName;

    property bool wasChanged: false;

    property string typeOperation: multiDocViewItem.typeOperation;

    onWasChangedChanged: {
        console.log("ContactInfoEditor onWasChangedChanged", containerContactInfo.wasChanged);

        containerContactInfo.commandsChanged("AccountEdit");
//        if (containerContactInfo.wasChanged){
//            containerContactInfo.setStateButton("Save", "Normal");
//        }
//        else {
//            containerContactInfo.setStateButton("Save", "Disabled");
//        }
    }

    function openMessageDialog(nameDialog, message) {

        var source = "AuxComponents/MessageDialog.qml";
        var parameters = {};
        parameters["resultItem"] = containerContactInfo;
        parameters["noButtonVisible"] = false;
        parameters["textOkButton"] = "OK";
        parameters["message"] = message;
        parameters["nameDialog"] = nameDialog;

        thubnailDecoratorContainer.openDialog(source, parameters);
    }

    function dialogResult(parameters){
    }

    function updateData() {
        console.log("containerContactInfo updateData");
        tfcEmail.text = containerContactInfo.contactInfoModel.GetData("Email");
        tfcFirstNameText.text = containerContactInfo.contactInfoModel.GetData("FirstName");
        tfcLastName.text = containerContactInfo.contactInfoModel.GetData("LastName");
        tfcNickName.text = containerContactInfo.contactInfoModel.GetData("NickName");
        tfcAccountName.text = containerContactInfo.contactInfoModel.GetData("AccountName");
        tfcAccountDescription.text = containerContactInfo.contactInfoModel.GetData("AccountDescription");

        cbTypeAccount.currentIndex = containerContactInfo.contactInfoModel.GetData("AccountTypeComboBoxIndex");
        console.log("AccountIndex =", cbTypeAccount.currentIndex);
        var accountType = containerContactInfo.contactInfoModel.GetData("AccountType");
        console.log("AccountType =", accountType);
        containerContactInfo.accountType = accountType;

//        if (accountType === "private") {
//            cbTypeAccount.currentIndex = 0;
//            containerContactInfo.accountType = "private";
//        } else {
//            cbTypeAccount.currentIndex = 1;
//            containerContactInfo.accountType = "company";
//        }

        //tfcBD.text = containerContactInfo.contactInfoModel.GetData("BirthDay");

//        if (containerContactInfo.accountType === "company")
//        {
//            var addresses = containerContactInfo.contactInfoModel.GetData("Addresses")
//            tfcCountryText.text = addresses.GetData("Country");
//            tfcCity.text = addresses.GetData("City");
//            postalCode.text = addresses.GetData("PostalCode");
//            tfcStreet.text = addresses.GetData("Street");
//        }

        //wasChanged = false;
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

    function menuActivated(menuId) {
        console.log("ContactInfoEditor menuActivated", menuId);
        if (menuId  === "New"){
//            collectionViewContainer.selectItem("", "")
        }
        else if (menuId  === "Save") {
            saveModel.updateModel();
        }
        else if (menuId === "Close") {
            containerContactInfo.rootItem.closeTab();
        }
    }

    function activateSaveButton(){
        console.log("ContactInfoEditor activateSaveButton");
        if (containerContactInfo.wasChanged) {
            containerContactInfo.rootItem.setModeMenuButton("Save", "Normal");
        }
    }

    function setStateButton(commandId, state){
        console.log("ContactInfoEditor setStateButton", commandId, state);
        containerContactInfo.rootItem.setModeMenuButton(commandId, state);
    }

    function refresh(){
        console.log("ContactInfoEditor refresh()", containerContactInfo.wasChanged);
    }

    function commandsChanged(commandsId){
        console.log("ContactInfoEditor commandsChanged", commandsId);
        if (commandsId !== "AccountEdit") {
            return;
        }

        containerContactInfo.rootItem.setModeMenuButton("New", "Normal");
        containerContactInfo.rootItem.setModeMenuButton("Close", "Normal");

        if (containerContactInfo.wasChanged){
            containerContactInfo.rootItem.setModeMenuButton("Save", "Normal");
        } else {
            containerContactInfo.rootItem.setModeMenuButton("Save", "Disabled");
        }
    }

    onModelChanged: {
        console.log("ContactInfoEditor onModelChanged", containerContactInfo.typeOperation);
        if (containerContactInfo.model.ContainsKey("data")){
            containerContactInfo.contactInfoModel = containerContactInfo.model.GetData('data');
            containerContactInfo.updateData();
        }
        else {
            if(containerContactInfo.typeOperation === "New"){
                //containerContactInfo.accountType = "company";
                cbTypeAccount.currentIndex = 0;
                containerContactInfo.contactInfoModel = model.AddTreeModel("data");
                containerContactInfo.contactInfoModel.SetData("Email","")
                containerContactInfo.contactInfoModel.SetData("FirstName","")
                containerContactInfo.contactInfoModel.SetData("LastName","")
                containerContactInfo.contactInfoModel.SetData("NickName","")
                containerContactInfo.contactInfoModel.SetData("AccountName","")
                containerContactInfo.contactInfoModel.SetData("AccountDescription","")
                containerContactInfo.contactInfoModel.SetData("AccountType", "company")

                containerContactInfo.contactInfoModel.SetData("AccountTypeComboBoxIndex", cbTypeAccount.currentIndex)

                var addresses = containerContactInfo.contactInfoModel.AddTreeModel("Addresses")
                addresses.SetData("Country","");
                addresses.SetData("City","");
                addresses.SetData("PostalCode","");
                addresses.SetData("Street","");
                //containerContactInfo.updateData();
            }
            else{
                accountItemModel.updateModel();
            }
        }
    }

    onContactInfoModelChanged: {
        console.log("ContactInfoEditor onContactInfoModelChanged");
        if (containerContactInfo.typeOperation != "New"){
            containerContactInfo.updateData();
        }
    }

    GqlModel {
        id: accountItemModel;

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

            queryFields.InsertField("Id");
            queryFields.InsertField("FirstName");
            queryFields.InsertField("LastName");
            queryFields.InsertField("NickName");
            queryFields.InsertField("Email");
            queryFields.InsertField("AccountType");
            queryFields.InsertField("AccountDescription");
            queryFields.InsertField("Addresses");


            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            console.log("AccountEdit query ", gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, accountItemModel);
            if (this.state === "Ready"){

                var dataModelLocal = accountItemModel.GetData("data");
                if(dataModelLocal.ContainsKey("AccountItem")){
                    dataModelLocal = dataModelLocal.GetData("AccountItem");
                    if(dataModelLocal.ContainsKey("item")){
                        //containerContactInfo.accountType = dataModelLocal.GetData("item").GetData("AccountType");

                        dataModelLocal = dataModelLocal.GetData("item");

                        var type = dataModelLocal.GetData("AccountType");

                        if (type === "private"){
                            dataModelLocal.SetData("AccountTypeComboBoxIndex", 0);
                        } else {
                            dataModelLocal.SetData("AccountTypeComboBoxIndex", 1);
                        }

                        containerContactInfo.contactInfoModel = dataModelLocal;

//                        console.log("Email", containerContactInfo.contactInfoModel.GetData("Email"));
//                        console.log("AccountName", containerContactInfo.contactInfoModel.GetData("AccountName"));
//                        console.log("FirstName", containerContactInfo.contactInfoModel.GetData("FirstName"));
//                        console.log("LastName", containerContactInfo.contactInfoModel.GetData("LastName"));
//                        console.log("Addresses", addressesTable.elements = containerContactInfo.contactInfoModel.GetData("Addresses"))

                        containerContactInfo.model.SetExternTreeModel('data', containerContactInfo.contactInfoModel)
                        //dataModelLocal.RemoveData("item");
                    }
                    else if(accountItemModel.ContainsKey("errors")){
                        var errorsModel = accountItemModel.GetData("errors");
                        if(errorsModel !== null && errorsModel.ContainsKey(containerContactInfo.gqlModelItems)){
                            console.log("message", errorsModel.GetData(containerContactInfo.gqlModelItems).GetData("message"));
                        }
                    }
                }
            }
        }
    }

    GqlModel {
        id: saveModel;

        function updateModel() {
            console.log( "updateModel saveModel");

            var query;
            var queryFields;
            var inputParams = Gql.GqlObject("input");

            if(containerContactInfo.typeOperation === "Open"){

                containerContactInfo.gqlModelQueryType = "AccountUpdate";
                containerContactInfo.gqlModelQueryTypeNotification = "updatedNotification";

                query = Gql.GqlRequest("query", "AccountUpdate");
                inputParams.InsertField("Id");
                inputParams.InsertFieldArgument("Id", containerContactInfo.itemId);
                queryFields = Gql.GqlObject("updatedNotification");
            }
            else{
                containerContactInfo.gqlModelQueryType = "AccountAdd";
                containerContactInfo.gqlModelQueryTypeNotification = "addedNotification";

                query = Gql.GqlRequest("query", "AccountAdd");
                inputParams.InsertField("Id");
                inputParams.InsertFieldArgument("Id", containerContactInfo.contactInfoModel.GetData("AccountName"));
                queryFields = Gql.GqlObject("addedNotification");
            }
            query.AddParam(inputParams);

            containerContactInfo.contactInfoModel.SetIsArray(false);
            var jsonString = containerContactInfo.contactInfoModel.toJSON();
            console.log("jsonString", jsonString)
            jsonString = jsonString.replace(/\"/g,"\\\\\\\"")
            console.log("jsonString", jsonString)

            inputParams.InsertField("Item");
            inputParams.InsertFieldArgument ("Item", jsonString);

            queryFields.InsertField("Id");
            queryFields.InsertField("Successed");

            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            console.log("AccountEdit query ", gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, saveModel);
            if (this.state === "Ready"){

                var dataModelLocal;

                if (saveModel.ContainsKey("errors")){
                    dataModelLocal = saveModel.GetData("errors");

                    if (dataModelLocal.ContainsKey(containerContactInfo.gqlModelQueryType)){
                        dataModelLocal = dataModelLocal.GetData(containerContactInfo.gqlModelQueryType);
                        var messageError = dataModelLocal.GetData("message");

                        containerContactInfo.openMessageDialog("Error Dialog", messageError);
                    }

                    return;
                }

                dataModelLocal = saveModel.GetData("data");


                containerContactInfo.multiDocViewItem.activeCollectionItem.refresh();
                if (dataModelLocal.ContainsKey(containerContactInfo.gqlModelQueryType)) {
                    dataModelLocal = dataModelLocal.GetData(containerContactInfo.gqlModelQueryType);

                    if (dataModelLocal.ContainsKey(containerContactInfo.gqlModelQueryTypeNotification)) {
                        dataModelLocal = dataModelLocal.GetData(containerContactInfo.gqlModelQueryTypeNotification);

                        if (dataModelLocal.ContainsKey("Id")) {
                            var oldId = containerContactInfo.itemId;
                            containerContactInfo.itemId = dataModelLocal.GetData("Id");
//                            containerContactInfo.rootItem.updateTitleTab(oldId, containerContactInfo.itemId);


//                            containerContactInfo.multiDocViewItem.activeCollectionItem.updateMetaInfoById(containerContactInfo.itemId, containerContactInfo.contactInfoModel);
                        }
                    }
                }
            }
        }
    }

    ListModel {
        id: typeAccountModel;
        ListElement {
            text: "Private";
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

            Rectangle {
                id: buttonsAvatar;
                anchors.horizontalCenter: parent.horizontalCenter;

                width: 125;
                height: 30;
                color: "transparent";

                AuxButton {
                    id: loadAvatarButton;

                    anchors.left: buttonsAvatar.left;
                    anchors.verticalCenter: buttonsAvatar.verticalCenter;

                    width: 50;
                    height:  20;

                    fontPixelSize: 12;

                  //  hasIcon: true;
                    hasText: true;
                    textButton: "Load";

                }

                AuxButton {
                    id: removeAvatarButton;

                    anchors.left: loadAvatarButton.right;
                    anchors.leftMargin: 5;
                    anchors.verticalCenter: buttonsAvatar.verticalCenter;

                    width: 50;
                    height:  20;
                    fontPixelSize: 12;

                  //  hasIcon: true;
                    hasText: true;

                    textButton: "Remove";
                }
            }

            Rectangle {
                id: avatarRect;
                anchors.horizontalCenter: parent.horizontalCenter;

                width: 150;
                height: 150;

                color: "transparent";

                Image {
                    id: avatarIcon;

                    height: 150;
                    width: 150;

                    fillMode: Image.PreserveAspectFit;
                    source: "../../../" + "Icons/Light/Account_On_Normal.svg";
                    sourceSize.width: width;
                    sourceSize.height: height;
                }
            }

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
                height: 23;

                radius: 3;
                model: typeAccountModel;
                //currentIndex: containerContactInfo.accountType == "private" ? 0 : 1;
//                currentText: containerContactInfo.accountType == "company" ? "Company" : "Private";
                currentText: cbTypeAccount.currentIndex === 0 ? "Private" : "Company";
                textCentered: false;
                backgroundColor: "#d0d0d0";
                borderColor: Style.theme == "Dark" ? "#565757" : "#a4a4a6";

                property bool wasFocus: false;

                onCurrentIndexChanged: {
                    var accountType = typeAccountModel.get(cbTypeAccount.currentIndex).text.toLowerCase();
                    console.log("ContactInfoEditor ComboBox onCurrentIndexChanged", accountType);

                    if (!containerContactInfo.contactInfoModel){
                        return;
                    }

                    containerContactInfo.contactInfoModel.SetData("AccountType", accountType);
                    containerContactInfo.contactInfoModel.SetData("AccountTypeComboBoxIndex", cbTypeAccount.currentIndex)

                    if (containerContactInfo.typeOperation !== "New" && !cbTypeAccount.wasFocus){
                        cbTypeAccount.wasFocus = true;
                        return;
                    }
                    containerContactInfo.wasChanged = true;
                }
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

                anchors.horizontalCenter: container.horizontalCenter;

                width: container.width;
                height: 23;

                onInputTextChanged: {
                    console.log("ContactInfoEditor TextFieldCustom AccountName onInputTextChanged");
                    containerContactInfo.contactInfoModel.SetData("AccountName", tfcAccountName.text);
                    containerContactInfo.wasChanged = true;
//                    containerContactInfo.activateSaveButton();
                }
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

                anchors.horizontalCenter: container.horizontalCenter;

                width: container.width;
                height: 23;

                onInputTextChanged: {
                    console.log("ContactInfoEditor TextFieldCustom AccountDescription onInputTextChanged");
                    containerContactInfo.contactInfoModel.SetData("AccountDescription", tfcAccountDescription.text);
                    containerContactInfo.wasChanged = true;
//                    containerContactInfo.activateSaveButton();
                }
            }

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

                color: Style.imagingToolsGradient1;
                border.color: Style.theme == "Light" ? "#d0d0d2" : "#3a3b3b" ;
                visible: cbTypeAccount.currentIndex === 1;

                Text {
                    id: titleCountry;

                    anchors.top: parent.top;
                    anchors.topMargin: 5;
                    anchors.left: companyAddressBlock.left;
                    anchors.leftMargin: 10;

                    text: qsTr("Country");
                    color: Style.textColor;
                    font.family: Style.fontFamily;
                    font.pixelSize: Style.fontSize_common;
                }

                Rectangle {
                    id: countryBlock;

                    anchors.horizontalCenter: companyAddressBlock.horizontalCenter;
                    anchors.top: titleCountry.bottom;

                    height: 45;
                    width: companyAddressBlock.width - 20;

                    //color: Style.baseColor;
                    color: Style.imagingToolsGradient1;
                    border.color: Style.theme == "Light" ? "#d0d0d2" : "#3a3b3b" ;

                    TextFieldCustom {
                        id: tfcCountryText;

                        anchors.horizontalCenter: countryBlock.horizontalCenter;
                        anchors.verticalCenter: countryBlock.verticalCenter;

                        width: countryBlock.width - 22;
                        height: 23;

                        onInputTextChanged: {
                            console.log("ContactInfoEditor TextFieldCustom Country onInputTextChanged");
                            containerContactInfo.contactInfoModel.SetData("Country", tfcCountryText.text);
                            containerContactInfo.wasChanged = true;
//                            containerContactInfo.activateSaveButton();
                        }
                    }
                }

                Text {
                    id: titleCity;

                    anchors.top: countryBlock.bottom;
                    anchors.topMargin: 7;
                    anchors.left: companyAddressBlock.left;
                    anchors.leftMargin: 10;

                    text: qsTr("City");
                    color: Style.textColor;
                    font.family: Style.fontFamily;

                    font.pixelSize: Style.fontSize_common;
                }

                Rectangle {
                    id: cityBlock;

                    anchors.horizontalCenter: companyAddressBlock.horizontalCenter;
                    anchors.top: titleCity.bottom;

                    width: companyAddressBlock.width - 20;
                    height: 45;

                    //color: Style.baseColor;
                    color: Style.imagingToolsGradient1;
                    border.color: Style.theme == "Light" ? "#d0d0d2" : "#3a3b3b" ;

                    TextFieldCustom {
                        id: tfcCity;

                        anchors.horizontalCenter: cityBlock.horizontalCenter;
                        anchors.verticalCenter: cityBlock.verticalCenter;

                        width: cityBlock.width - 22;
                        height: 23;

                        onInputTextChanged: {
                            console.log("ContactInfoEditor TextFieldCustom City onInputTextChanged");
                            containerContactInfo.contactInfoModel.SetData("City", tfcCity.text);
                            containerContactInfo.wasChanged = true;
//                            containerContactInfo.activateSaveButton();
                        }
                    }
                }

                Text {
                    id: titlePostalCode;

                    anchors.top: cityBlock.bottom;
                    anchors.topMargin: 7;
                    anchors.left: companyAddressBlock.left;
                    anchors.leftMargin: 10;

                    text: qsTr("Postal code");
                    color: Style.textColor;
                    font.family: Style.fontFamily;
                    font.pixelSize: Style.fontSize_common;
                }

                Rectangle {
                    id: postalCodeBlock;

                    anchors.horizontalCenter: companyAddressBlock.horizontalCenter;
                    anchors.top: titlePostalCode.bottom;

                    height: 45;
                    width: companyAddressBlock.width - 20;

                    //color: Style.baseColor;
                    color: Style.imagingToolsGradient1;
                    border.color: Style.theme == "Light" ? "#d0d0d2" : "#3a3b3b" ;

                    TextFieldCustom {
                        id: postalCode;

                        //text: containerContactInfo.accountType === "company" ? containerContactInfo.contactInfoModel.GetData("Addresses").GetData("PostalCode") : "";
                        anchors.horizontalCenter: postalCodeBlock.horizontalCenter;
                        anchors.verticalCenter: postalCodeBlock.verticalCenter;

                        width: postalCodeBlock.width - 22;
                        height: 23;

                        text: "5";

                        onInputTextChanged: {
                            console.log("ContactInfoEditor TextFieldCustom PostalCode onInputTextChanged");
                            containerContactInfo.contactInfoModel.SetData("PostalCode", postalCode.text);
                            containerContactInfo.wasChanged = true;
//                            containerContactInfo.activateSaveButton();
                        }
                    }
                }

                Text {
                    id: titleStreet;

                    anchors.top: postalCodeBlock.bottom;
                    anchors.topMargin: 7;
                    anchors.left: companyAddressBlock.left;
                    anchors.leftMargin: 10;

                    text: qsTr("Street");
                    color: Style.textColor;
                    font.family: Style.fontFamily;
                    font.pixelSize: Style.fontSize_common;
                }

                Rectangle {
                    id: streetBlock;
                    width: companyAddressBlock.width - 20;
                    anchors.horizontalCenter: companyAddressBlock.horizontalCenter;
                    height: 45;
                    anchors.top: titleStreet.bottom;
                    color: Style.imagingToolsGradient1;
                    border.color: Style.theme == "Light" ? "#d0d0d2" : "#3a3b3b" ;

                    TextFieldCustom {
                        id: tfcStreet;
                        width: streetBlock.width - 22;

                        height: 23;
                        anchors.horizontalCenter: streetBlock.horizontalCenter;
                        anchors.verticalCenter: streetBlock.verticalCenter;

                        onInputTextChanged: {
                            console.log("ContactInfoEditor TextFieldCustom Street onInputTextChanged");
                            containerContactInfo.contactInfoModel.SetData("Street", tfcStreet.text);
                            containerContactInfo.wasChanged = true;
//                            containerContactInfo.activateSaveButton();
                        }
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
//                        text: containerContactInfo.contactInfoModel ? containerContactInfo.contactInfoModel.GetData("Email") : "";
                        height: 23;
                        anchors.horizontalCenter: emailBlock.horizontalCenter;
                        anchors.verticalCenter: emailBlock.verticalCenter;

                        onInputTextChanged: {
                            console.log("ContactInfoEditor TextFieldCustom Email onInputTextChanged");
                            containerContactInfo.contactInfoModel.SetData("Email", tfcEmail.text);
                            containerContactInfo.wasChanged = true;
//                            containerContactInfo.activateSaveButton();
                        }
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
//                        text: containerContactInfo.contactInfoModel ? containerContactInfo.contactInfoModel.GetData("BirthDay") : "";
                        height: 23;
                        anchors.horizontalCenter: bdBlock.horizontalCenter;
                        anchors.verticalCenter: bdBlock.verticalCenter;

                        onInputTextChanged: {
                            console.log("ContactInfoEditor TextFieldCustom BirthDay onInputTextChanged");
                            containerContactInfo.contactInfoModel.SetData("BirthDay", tfcBD.text);
                            containerContactInfo.wasChanged = true;
//                            containerContactInfo.activateSaveButton();
                        }
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
                        height: 23;
                        radius: 3;
                        model: genderModel;
                        anchors.horizontalCenter: genderBlock.horizontalCenter;
                        anchors.verticalCenter: genderBlock.verticalCenter;
                        backgroundColor: "#d0d0d0";
                        borderColor: Style.theme == "Dark" ? "#565757" : "#a4a4a6";
                        textCentered: false;

                        onCurrentIndexChanged: {
                            console.log("ContactInfoEditor TextFieldCustom Gender onInputTextChanged");
                            var gender = genderModel.get(genderCB.currentIndex).text.toLowerCase();
                            containerContactInfo.contactInfoModel.SetData("Gender", gender);
                            containerContactInfo.wasChanged = true;
//                            containerContactInfo.activateSaveButton();
                        }
                    }
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
//                        text: containerContactInfo.contactInfoModel ? containerContactInfo.contactInfoModel.GetData("FirstName") : "";
                        height: 23;
                        anchors.horizontalCenter: firstNameBlock.horizontalCenter;
                        anchors.verticalCenter: firstNameBlock.verticalCenter;

                        onInputTextChanged: {
                            console.log("ContactInfoEditor TextFieldCustom FirstName onInputTextChanged");
                            containerContactInfo.contactInfoModel.SetData("FirstName", tfcFirstNameText.text);
                            containerContactInfo.wasChanged = true;
//                            containerContactInfo.activateSaveButton();
                        }
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
//                        text: containerContactInfo.contactInfoModel ? containerContactInfo.contactInfoModel.GetData("LastName") : "";
                        height: 23;
                        anchors.horizontalCenter: lastNameBlock.horizontalCenter;
                        anchors.verticalCenter: lastNameBlock.verticalCenter;

                        onInputTextChanged: {
                            console.log("ContactInfoEditor TextFieldCustom LastName onInputTextChanged");
                            containerContactInfo.contactInfoModel.SetData("LastName", tfcLastName.text);
                            containerContactInfo.wasChanged = true;
//                            containerContactInfo.activateSaveButton();
                        }
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
//                        text: containerContactInfo.contactInfoModel ? containerContactInfo.contactInfoModel.GetData("NickName") : "";
                        height: 23;
                        anchors.horizontalCenter: nickNameBlock.horizontalCenter;
                        anchors.verticalCenter: nickNameBlock.verticalCenter;

                        onInputTextChanged: {
                            console.log("ContactInfoEditor TextFieldCustom NickName onInputTextChanged");
                            containerContactInfo.contactInfoModel.SetData("NickName", tfcNickName.text);
                            containerContactInfo.wasChanged = true;
//                            containerContactInfo.activateSaveButton();
                        }
                    }
                }

                Text {
                    id: titleAddresses;

                    anchors.top: nickNameBlock.bottom;
                    anchors.topMargin: 7;
                    anchors.left: accountOwnerBlock.left;
                    anchors.leftMargin: 10;

                    text: qsTr("Addresses");
                    color: Style.textColor;
                    font.family: Style.fontFamily;
                    font.pixelSize: Style.fontSize_common;
                    visible: cbTypeAccount.currentIndex === 0;
                }

                Rectangle {
                    id: addressesBlock;

                    anchors.horizontalCenter: accountOwnerBlock.horizontalCenter;
                    anchors.top: titleAddresses.bottom;

                    height: 200;
                    width: accountOwnerBlock.width - 20;

                    //color: Style.baseColor;
                    visible: cbTypeAccount.currentIndex === 0;
                    color: Style.imagingToolsGradient1;
                    border.color: Style.theme == "Light" ? "#d0d0d2" : "#3a3b3b" ;

                    Rectangle {
                        id: addressesBlockButtons;

                        anchors.top: addressesBlock.top;
                        anchors.topMargin: 10;
                        anchors.left: addressesBlock.left;
                        anchors.leftMargin: 10;

                        width: 50;
                        height: 20;

                        color: "transparent";

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

                        headers: headersModel;
                    }

                    TreeItemModel {
                        id: headersModel;

                        Component.onCompleted: {
                            headersModel.SetData("Name", "Country", 0)
                            headersModel.InsertNewItem()
                            headersModel.SetData("Name", "City", 1)
                            headersModel.InsertNewItem()
                            headersModel.SetData("Name", "Postal code", 2)
                            headersModel.InsertNewItem()
                            headersModel.SetData("Name", "Street", 3)
                            headersModel.Refresh()
                        }
                    }

                }


            }
        }
    }
}


