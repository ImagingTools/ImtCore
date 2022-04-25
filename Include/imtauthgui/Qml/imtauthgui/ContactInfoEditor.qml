import QtQuick 2.0
//import '../../../imtgui/Qml/imtgui/AuxComponents'
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

Rectangle {
    id: containerContactInfo;

    anchors.fill: parent;

    width: 500;
    height: 100;

    color: Style.backgroundColor;

    property TreeItemModel contactInfoModel;
    property TreeItemModel model;

    property Item multiDocViewItem;
    property Item rootItem;

    property string gqlModelQueryType;
    property string gqlModelQueryTypeNotification;

    property string accountType;
    property string itemId;
    property string itemName;
    property string operation;

    property bool wasChanged: false;

    Component.onCompleted: {
        rectTfcAccountName.forceActiveFocus();
    }

    onWasChangedChanged: {
        console.log("ContactInfoEditor onWasChangedChanged", containerContactInfo.wasChanged);
        containerContactInfo.commandsChanged("AccountEdit");
    }

    onContactInfoModelChanged: {
        console.log("ContactInfoEditor onContactInfoModelChanged");
        if (containerContactInfo.operation != "New"){
            containerContactInfo.updateData();
        }
    }

    onModelChanged: {
        console.log("ContactInfoEditor onModelChanged", containerContactInfo.operation);
        if (containerContactInfo.model.ContainsKey("data")){
            containerContactInfo.contactInfoModel = containerContactInfo.model.GetData('data');
            containerContactInfo.updateData();
        }
        else {
            if(containerContactInfo.operation === "New"){
                cbTypeAccount.currentIndex = 0;
                containerContactInfo.accountType = "private";

                containerContactInfo.contactInfoModel = model.AddTreeModel("data");
                containerContactInfo.contactInfoModel.SetData("Email","")
                containerContactInfo.contactInfoModel.SetData("FirstName","")
                containerContactInfo.contactInfoModel.SetData("LastName","")
                containerContactInfo.contactInfoModel.SetData("NickName","")
                containerContactInfo.contactInfoModel.SetData("AccountName","")
                containerContactInfo.contactInfoModel.SetData("AccountDescription","")
                containerContactInfo.contactInfoModel.SetData("AccountType", "private")

                containerContactInfo.contactInfoModel.SetData("AccountTypeComboBoxIndex", cbTypeAccount.currentIndex)

                var addresses = containerContactInfo.contactInfoModel.AddTreeModel("Addresses")
                addresses.SetData("Country","");
                addresses.SetData("City","");
                addresses.SetData("PostalCode","");
                addresses.SetData("Street","");
            }
            else{
                accountItemModel.updateModel();
            }
        }
    }

    function dialogResult(parameters){}

    function openMessageDialog(nameDialog, message, type) {
        var source = "AuxComponents/MessageDialog.qml";
        var parameters = {};
        parameters["resultItem"] = containerContactInfo;
        parameters["noButtonVisible"] = false;
        parameters["textOkButton"] = "OK";
        parameters["message"] = message;
        parameters["nameDialog"] = nameDialog;
        parameters["dialogId"] = type;
        thubnailDecoratorContainer.openDialog(source, parameters);
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
        var accountType = containerContactInfo.contactInfoModel.GetData("AccountType");
        containerContactInfo.accountType = accountType;
    }

    function menuActivated(menuId) {
        console.log("ContactInfoEditor menuActivated", menuId);
        if (menuId  === "New"){
//            collectionViewContainer.selectItem("", "")
        }
        else if (menuId  === "Save") {

            var name = containerContactInfo.contactInfoModel.GetData("AccountName");
            if (name == ""){
                var message = "Account name can't be empty!";
                containerContactInfo.openMessageDialog("Error Dialog", message, "ErrorDialog");
                return;
            }

            contactInfoSaveQuery.updateModel();
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

        containerContactInfo.rootItem.setModeMenuButton("Save", "Normal");
//        if (containerContactInfo.wasChanged){
//            containerContactInfo.rootItem.setModeMenuButton("Save", "Normal");
//        }
//        else {
//            containerContactInfo.rootItem.setModeMenuButton("Save", "Disabled");
//        }
    }

    ListModel {
        id: typeAccountModel;

        ListElement {
            name: "Private";
        }

        ListElement {
            name: "Company";
        }
    }

    ListModel {
        id: genderModel;

        ListElement {
            name: "Diverse";
        }

        ListElement {
            name: "Male";
        }

        ListElement {
            name: "Female";
        }
    }

    Flickable {
        anchors.fill: parent;

        contentWidth: contactInfoColumn.width;
        contentHeight: contactInfoColumn.height + 50;
        boundsBehavior: Flickable.StopAtBounds;

        clip: true;

        Column {
            id: contactInfoColumn;

            width: 500;

            spacing: 7;

//            Rectangle {
//                id: buttonsAvatar;

//                anchors.horizontalCenter: contactInfoColumn.horizontalCenter;

//                width: 125;
//                height: 30;

//                color: "transparent";

//                AuxButton {
//                    id: loadAvatarButton;

//                    anchors.left: buttonsAvatar.left;
//                    anchors.verticalCenter: buttonsAvatar.verticalCenter;

//                    width: 50;
//                    height:  20;

//                    fontPixelSize: 12;

//                    hasText: true;
//                    textButton: "Load";
//                }

//                AuxButton {
//                    id: removeAvatarButton;

//                    anchors.left: loadAvatarButton.right;
//                    anchors.leftMargin: 5;
//                    anchors.verticalCenter: buttonsAvatar.verticalCenter;

//                    width: 50;
//                    height:  20;

//                    fontPixelSize: 12;

//                    hasText: true;

//                    textButton: "Remove";
//                }
//            }

            Rectangle {
                id: avatarRect;

                anchors.horizontalCenter: contactInfoColumn.horizontalCenter;

                width: 150;
                height: 150;

                color: "transparent";

                Image {
                    id: avatarIcon;

                    height: 150;
                    width: 150;

                    source: "../../Icons/Light/Account_On_Normal.svg";

                    sourceSize.width: width;
                    sourceSize.height: height;

                    fillMode: Image.PreserveAspectFit;
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
                currentText: cbTypeAccount.currentIndex === 0 ? "Private" : "Company";
                textCentered: false;

                //borderColor: Style.alternateBaseColor;

                property bool wasFocus: false;

                onCurrentIndexChanged: {
                    console.log("cbTypeAccount onCurrentIndexChanged", cbTypeAccount.currentIndex);

                    var accountType = typeAccountModel.get(cbTypeAccount.currentIndex).name.toLowerCase();
                    console.log("accountType", accountType);
                    containerContactInfo.accountType = accountType;

                    if (!containerContactInfo.contactInfoModel){
                        return;
                    }

                    containerContactInfo.contactInfoModel.SetData("AccountType", accountType);
                    containerContactInfo.contactInfoModel.SetData("AccountTypeComboBoxIndex", cbTypeAccount.currentIndex)
//                    if (containerContactInfo.operation !== "New" && !cbTypeAccount.wasFocus){
//                        cbTypeAccount.wasFocus = true;
//                        return;
//                    }
//                    containerContactInfo.wasChanged = true;
                }

                onClicked: {
                    cbTypeAccount.focus = true;
                    cbTypeAccount.openContextMenu();
                }

                onDialogResultChanged: {
                    cbTypeAccount.focus = true;
                }

                Keys.onPressed: {
                    if (event.key === Qt.Key_Space){
                        cbTypeAccount.clicked();
                    }
                }

                KeyNavigation.tab: rectTfcAccountName;
            }

            Text {
                id: titleAccountName;
                z: 5;

                text: qsTr("Account name");
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }

            Rectangle {
                id: rectTfcAccountName;

                anchors.horizontalCenter: contactInfoColumn.horizontalCenter;

                width: contactInfoColumn.width;
                height: 30;

                TextFieldCustom {
                    id: tfcAccountName;

                    anchors.fill: parent;

                    onInputTextChanged: {
                        console.log("ContactInfoEditor TextFieldCustom AccountName onInputTextChanged");
                        containerContactInfo.contactInfoModel.SetData("AccountName", tfcAccountName.text);
                        containerContactInfo.wasChanged = true;
                    }
                }

                onFocusChanged: {
                    if (rectTfcAccountName.focus){
                        tfcAccountName.setFocus(true);
                    }
                }

                KeyNavigation.tab: rectTfcAccountDescription;
                KeyNavigation.backtab: cbTypeAccount;
            }

//            TextFieldCustom {
//                id: tfcAccountName;

//                anchors.horizontalCenter: contactInfoColumn.horizontalCenter;

//                width: contactInfoColumn.width;
//                height: 30;

//                onInputTextChanged: {
//                    console.log("ContactInfoEditor TextFieldCustom AccountName onInputTextChanged");
//                    containerContactInfo.contactInfoModel.SetData("AccountName", tfcAccountName.text);
//                    containerContactInfo.wasChanged = true;
//                }

//                KeyNavigation.tab: tfcAccountDescription;
//            }

            Text {
                id: titleAccountDescription;

                text: qsTr("Account description");
                color: Style.textColor;

                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }

            Rectangle {
                id: rectTfcAccountDescription;

                anchors.horizontalCenter: contactInfoColumn.horizontalCenter;

                width: contactInfoColumn.width;
                height: 30;

                TextFieldCustom {
                    id: tfcAccountDescription;

                    anchors.fill: parent;

                    onInputTextChanged: {
                        console.log("ContactInfoEditor TextFieldCustom AccountDescription onInputTextChanged");
                        containerContactInfo.contactInfoModel.SetData("AccountDescription", tfcAccountDescription.text);
                        containerContactInfo.wasChanged = true;
                    }
                }

                onFocusChanged: {
                    if (rectTfcAccountDescription.focus){
                        tfcAccountDescription.setFocus(true);
                    }
                }

                KeyNavigation.backtab: rectTfcAccountName;
                KeyNavigation.tab: containerContactInfo.accountType === "company" ? countryBlock : emailBlock;
            }

//            TextFieldCustom {
//                id: tfcAccountDescription;

//                anchors.horizontalCenter: contactInfoColumn.horizontalCenter;

//                width: contactInfoColumn.width;
//                height: 30;

//                onInputTextChanged: {
//                    console.log("ContactInfoEditor TextFieldCustom AccountDescription onInputTextChanged");
//                    containerContactInfo.contactInfoModel.SetData("AccountDescription", tfcAccountDescription.text);
//                    containerContactInfo.wasChanged = true;
//                }

//                KeyNavigation.tab: tfcAccountDescription;
//                KeyNavigation.backtab: tfcAccountName;
//            }

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

                width: contactInfoColumn.width;
                height: 300;

                color: Style.imagingToolsGradient1;

                border.width: 1;
                border.color: Style.borderColor;

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

                    color: Style.imagingToolsGradient1;

                    border.width: 1;
                    border.color: Style.borderColor;

                    TextFieldCustom {
                        id: tfcCountryText;

                        anchors.horizontalCenter: countryBlock.horizontalCenter;
                        anchors.verticalCenter: countryBlock.verticalCenter;

                        width: countryBlock.width - 22;
                        height: 30;

                        onInputTextChanged: {
                            console.log("ContactInfoEditor TextFieldCustom Country onInputTextChanged");
                            containerContactInfo.contactInfoModel.SetData("Country", tfcCountryText.text);
                            containerContactInfo.wasChanged = true;
                        }
                    }

                    onFocusChanged: {
                        if (countryBlock.focus){
                            tfcCountryText.setFocus(true);
                        }
                    }

                    KeyNavigation.tab: cityBlock;
                    KeyNavigation.backtab: rectTfcAccountDescription;
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

                    color: Style.imagingToolsGradient1;

                    border.width: 1;
                    border.color: Style.borderColor;

                    TextFieldCustom {
                        id: tfcCity;

                        anchors.horizontalCenter: cityBlock.horizontalCenter;
                        anchors.verticalCenter: cityBlock.verticalCenter;

                        width: cityBlock.width - 22;
                        height: 30;

                        onInputTextChanged: {
                            console.log("ContactInfoEditor TextFieldCustom City onInputTextChanged");
                            containerContactInfo.contactInfoModel.SetData("City", tfcCity.text);
                            containerContactInfo.wasChanged = true;
                        }
                    }

                    onFocusChanged: {
                        if (cityBlock.focus){
                            tfcCity.setFocus(true);
                        }
                    }

                    KeyNavigation.tab: postalCodeBlock;
                    KeyNavigation.backtab: countryBlock;
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

                    color: Style.imagingToolsGradient1;

                    border.width: 1;
                    border.color: Style.borderColor;

                    TextFieldCustom {
                        id: postalCode;

                        //text: containerContactInfo.accountType === "company" ? containerContactInfo.contactInfoModel.GetData("Addresses").GetData("PostalCode") : "";
                        anchors.horizontalCenter: postalCodeBlock.horizontalCenter;
                        anchors.verticalCenter: postalCodeBlock.verticalCenter;

                        width: postalCodeBlock.width - 22;
                        height: 30;

                        text: "5";

                        onInputTextChanged: {
                            console.log("ContactInfoEditor TextFieldCustom PostalCode onInputTextChanged");
                            containerContactInfo.contactInfoModel.SetData("PostalCode", postalCode.text);
                            containerContactInfo.wasChanged = true;
                        }
                    }

                    onFocusChanged: {
                        if (postalCodeBlock.focus){
                            postalCode.setFocus(true);
                        }
                    }

                    KeyNavigation.tab: streetBlock;
                    KeyNavigation.backtab: cityBlock;
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

                    anchors.horizontalCenter: companyAddressBlock.horizontalCenter;
                    anchors.top: titleStreet.bottom;

                    height: 45;
                    width: companyAddressBlock.width - 20;

                    color: Style.imagingToolsGradient1;

                    border.width: 1;
                    border.color: Style.borderColor;

                    TextFieldCustom {
                        id: tfcStreet;

                        anchors.horizontalCenter: streetBlock.horizontalCenter;
                        anchors.verticalCenter: streetBlock.verticalCenter;

                        width: streetBlock.width - 22;
                        height: 30;

                        onInputTextChanged: {
                            console.log("ContactInfoEditor TextFieldCustom Street onInputTextChanged");
                            containerContactInfo.contactInfoModel.SetData("Street", tfcStreet.text);
                            containerContactInfo.wasChanged = true;
                        }
                    }

                    onFocusChanged: {
                        if (streetBlock.focus){
                            tfcStreet.setFocus(true);
                        }
                    }

                    KeyNavigation.tab: emailBlock;
                    KeyNavigation.backtab: postalCodeBlock;
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

                width: contactInfoColumn.width;
                height: cbTypeAccount.currentIndex === 0 ? 680 : 250;

                color: Style.imagingToolsGradient1;

                border.width: 1;
                border.color: Style.borderColor;

                Text {
                    id: titleEmail;

                    anchors.top: parent.top;
                    anchors.topMargin: 7;
                    anchors.left: accountOwnerBlock.left;
                    anchors.leftMargin: 10;

                    text: qsTr("Email");
                    color: Style.textColor;
                    font.family: Style.fontFamily;

                    font.pixelSize: Style.fontSize_common;
                }

                Rectangle {
                    id: emailBlock;

                    anchors.horizontalCenter: accountOwnerBlock.horizontalCenter;
                    anchors.top: titleEmail.bottom;

                    width: accountOwnerBlock.width - 20;
                    height: 45;

                    color: Style.imagingToolsGradient1;

                    border.width: 1;
                    border.color: Style.borderColor;

                    TextFieldCustom {
                        id: tfcEmail;

                        anchors.horizontalCenter: emailBlock.horizontalCenter;
                        anchors.verticalCenter: emailBlock.verticalCenter;

                        width: emailBlock.width - 22;
                        height: 30;

                        onInputTextChanged: {
                            console.log("ContactInfoEditor TextFieldCustom Email onInputTextChanged");
                            containerContactInfo.contactInfoModel.SetData("Email", tfcEmail.text);
                            containerContactInfo.wasChanged = true;
//                            containerContactInfo.activateSaveButton();
                        }
                    }

                    onFocusChanged: {
                        if (emailBlock.focus){
                            tfcEmail.setFocus(true);
                        }
                    }

                    KeyNavigation.tab: containerContactInfo.accountType === "company" ? firstNameBlock : bdBlock;
                    KeyNavigation.backtab: containerContactInfo.accountType === "company" ? streetBlock : rectTfcAccountDescription;
                }

                Text {
                    id: titleBD;

                    anchors.top: emailBlock.bottom;
                    anchors.topMargin: 7;
                    anchors.left: accountOwnerBlock.left;
                    anchors.leftMargin: 10;

                    text: qsTr("BirthDay");
                    color: Style.textColor;
                    font.family: Style.fontFamily;
                    font.pixelSize: Style.fontSize_common;

                    visible: cbTypeAccount.currentIndex === 0;
                }

                Rectangle {
                    id: bdBlock;

                    anchors.horizontalCenter: accountOwnerBlock.horizontalCenter;
                    anchors.top: titleBD.bottom;

                    height: 45;
                    width: accountOwnerBlock.width - 20;

                    visible: cbTypeAccount.currentIndex === 0;

                    color: Style.imagingToolsGradient1;

                    border.width: 1;
                    border.color: Style.borderColor;

                    TextFieldCustom {
                        id: tfcBD;

                        anchors.horizontalCenter: bdBlock.horizontalCenter;
                        anchors.verticalCenter: bdBlock.verticalCenter;

                        width: bdBlock.width - 22;
                        height: 30;

                        onInputTextChanged: {
                            console.log("ContactInfoEditor TextFieldCustom BirthDay onInputTextChanged");
                            containerContactInfo.contactInfoModel.SetData("BirthDay", tfcBD.text);
                            containerContactInfo.wasChanged = true;
                        }
                    }

                    onFocusChanged: {
                        if (bdBlock.focus){
                            tfcBD.setFocus(true);
                        }
                    }

                    KeyNavigation.tab: genderCB;
                    KeyNavigation.backtab: emailBlock;
                }

                Text {
                    id: titleGender;

                    anchors.top: bdBlock.bottom;
                    anchors.topMargin: 7;
                    anchors.left: accountOwnerBlock.left;
                    anchors.leftMargin: 10;

                    text: qsTr("Gender");
                    color: Style.textColor;
                    font.family: Style.fontFamily;
                    font.pixelSize: Style.fontSize_common;
                    visible: cbTypeAccount.currentIndex === 0;
                }

                Rectangle {
                    id: genderBlock;

                    z: 10;

                    anchors.top: titleGender.bottom;
                    anchors.horizontalCenter: accountOwnerBlock.horizontalCenter;

                    width: accountOwnerBlock.width - 20;
                    height: 45;

                    visible: cbTypeAccount.currentIndex === 0;

                    color: Style.imagingToolsGradient1;

                    border.width: 1;
                    border.color: Style.borderColor;

                    ComboBox {
                        id: genderCB;

                        anchors.horizontalCenter: genderBlock.horizontalCenter;
                        anchors.verticalCenter: genderBlock.verticalCenter;

                        width: bdBlock.width - 22;
                        height: 23;

                        radius: 3;
                        model: genderModel;
                        backgroundColor: "#d0d0d0";
                        //borderColor: Style.alternateBaseColor;
                        textCentered: false;

                        onCurrentIndexChanged: {
                            console.log("ContactInfoEditor TextFieldCustom Gender onInputTextChanged");
                            var gender = genderModel.get(genderCB.currentIndex).name.toLowerCase();
                            containerContactInfo.contactInfoModel.SetData("Gender", gender);
                            containerContactInfo.wasChanged = true;
                        }

                        onClicked: {
                            genderCB.openContextMenu();
                        }

                        onDialogResultChanged: {
                            genderCB.forceActiveFocus();
                        }

                        Keys.onPressed: {
                            console.log("genderCB keys pressed")
                            if (event.key === Qt.Key_Space){
                                genderCB.clicked();
                            }
                        }

                        KeyNavigation.tab: firstNameBlock;
                        KeyNavigation.backtab: bdBlock;
                    }
                }

                Text {
                    id: titleFirstName;
                    z: 5;

                    anchors.left: accountOwnerBlock.left;
                    anchors.leftMargin: 7;
                    anchors.top: cbTypeAccount.currentIndex === 0 ? genderBlock.bottom : emailBlock.bottom;
                    anchors.topMargin: 10;

                    text: qsTr("First name");
                    color: Style.textColor;
                    font.family: Style.fontFamily;
                    font.pixelSize: Style.fontSize_common;
                }

                Rectangle {
                    id: firstNameBlock;
                    z: 5;

                    anchors.top: titleFirstName.bottom;
                    anchors.horizontalCenter: accountOwnerBlock.horizontalCenter;

                    width: accountOwnerBlock.width - 20;
                    height: 45;

                    color: Style.imagingToolsGradient1;
                    border.color: Style.borderColor;

                    TextFieldCustom {
                        id: tfcFirstNameText;

                        anchors.horizontalCenter: firstNameBlock.horizontalCenter;
                        anchors.verticalCenter: firstNameBlock.verticalCenter;

                        width: firstNameBlock.width - 22;
                        height: 30;

                        onInputTextChanged: {
                            console.log("ContactInfoEditor TextFieldCustom FirstName onInputTextChanged");
                            containerContactInfo.contactInfoModel.SetData("FirstName", tfcFirstNameText.text);
                            containerContactInfo.wasChanged = true;
                        }
                    }

                    onFocusChanged: {
                        if (firstNameBlock.focus){
                            tfcFirstNameText.setFocus(true);
                        }
                    }

                    KeyNavigation.tab: lastNameBlock;
                    KeyNavigation.backtab: containerContactInfo.accountType === "company" ? emailBlock : genderCB;
                }

                Text {
                    id: titleLastName;

                    anchors.left: accountOwnerBlock.left;
                    anchors.leftMargin: 10;
                    anchors.top: firstNameBlock.bottom;
                    anchors.topMargin: 7;

                    text: qsTr("Last name");
                    color: Style.textColor;
                    font.family: Style.fontFamily;
                    font.pixelSize: Style.fontSize_common;
                }

                Rectangle {
                    id: lastNameBlock;

                    anchors.horizontalCenter: accountOwnerBlock.horizontalCenter;
                    anchors.top: titleLastName.bottom;

                    width: accountOwnerBlock.width - 20;
                    height: 45;

                    color: Style.imagingToolsGradient1;

                    border.width: 1;
                    border.color: Style.borderColor;

                    TextFieldCustom {
                        id: tfcLastName;

                        anchors.horizontalCenter: lastNameBlock.horizontalCenter;
                        anchors.verticalCenter: lastNameBlock.verticalCenter;

                        width: lastNameBlock.width - 22;
                        height: 30;

                        onInputTextChanged: {
                            console.log("ContactInfoEditor TextFieldCustom LastName onInputTextChanged");
                            containerContactInfo.contactInfoModel.SetData("LastName", tfcLastName.text);
                            containerContactInfo.wasChanged = true;
                        }
                    }

                    onFocusChanged: {
                        if (lastNameBlock.focus){
                            tfcLastName.setFocus(true);
                        }
                    }

                    KeyNavigation.tab: containerContactInfo.accountType === "company" ? lastNameBlock : nickNameBlock;
                    KeyNavigation.backtab: firstNameBlock;
                }

                Text {
                    id: titleNickName;

                    anchors.left: accountOwnerBlock.left;
                    anchors.leftMargin: 10;
                    anchors.top: lastNameBlock.bottom;
                    anchors.topMargin: 7;

                    text: qsTr("Nickname");
                    color: Style.textColor;
                    font.family: Style.fontFamily;
                    font.pixelSize: Style.fontSize_common;

                    visible: cbTypeAccount.currentIndex === 0;
                }

                Rectangle {
                    id: nickNameBlock;

                    anchors.top: titleNickName.bottom;
                    anchors.horizontalCenter: accountOwnerBlock.horizontalCenter;

                    width: accountOwnerBlock.width - 20;
                    height: 45;

                    visible: cbTypeAccount.currentIndex === 0;
                    color: Style.imagingToolsGradient1;

                    border.width: 1;
                    border.color: Style.borderColor;

                    TextFieldCustom {
                        id: tfcNickName;

                        anchors.horizontalCenter: nickNameBlock.horizontalCenter;
                        anchors.verticalCenter: nickNameBlock.verticalCenter;

                        width: nickNameBlock.width - 22;
                        height: 30;

                        onInputTextChanged: {
                            console.log("ContactInfoEditor TextFieldCustom NickName onInputTextChanged");
                            containerContactInfo.contactInfoModel.SetData("NickName", tfcNickName.text);
                            containerContactInfo.wasChanged = true;
                        }
                    }

                    onFocusChanged: {
                        if (nickNameBlock.focus){
                            tfcNickName.setFocus(true);
                        }
                    }

                    KeyNavigation.tab: nickNameBlock;
                    KeyNavigation.backtab: lastNameBlock;
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
                    visible: false;
//                    visible: cbTypeAccount.currentIndex === 0;
                }

                Rectangle {
                    id: addressesBlock;

                    anchors.horizontalCenter: accountOwnerBlock.horizontalCenter;
                    anchors.top: titleAddresses.bottom;

                    height: 200;
                    width: accountOwnerBlock.width - 20;

//                    visible: cbTypeAccount.currentIndex === 0;
                    visible: false;
                    color: Style.imagingToolsGradient1;

                    border.width: 1;
                    border.color: Style.borderColor;

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

                        headers: headersModelAddresses;
                    }

                    TreeItemModel {
                        id: headersModelAddresses;

                        Component.onCompleted: {
                            headersModelAddresses.SetData("Name", "Country", 0)
                            headersModelAddresses.InsertNewItem()
                            headersModelAddresses.SetData("Name", "City", 1)
                            headersModelAddresses.InsertNewItem()
                            headersModelAddresses.SetData("Name", "Postal code", 2)
                            headersModelAddresses.InsertNewItem()
                            headersModelAddresses.SetData("Name", "Street", 3)
                            headersModelAddresses.Refresh()
                        }
                    }
                }
            }
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
                        containerContactInfo.model.SetExternTreeModel('data', containerContactInfo.contactInfoModel)
                        //dataModelLocal.RemoveData("item");
                    }
                    else if(accountItemModel.ContainsKey("errors")){
                        var errorsModel = accountItemModel.GetData("errors");
                        if(errorsModel.ContainsKey(containerContactInfo.gqlModelItems)){
                            console.log("message", errorsModel.GetData(containerContactInfo.gqlModelItems).GetData("message"));
                        }
                    }
                }
            }
        }
    }

    GqlModel {
        id: contactInfoSaveQuery;

        function updateModel() {
            console.log( "updateModel contactInfoSaveQuery");

            var query;
            var queryFields;
            var inputParams = Gql.GqlObject("input");

            if(containerContactInfo.operation === "Open"){

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
//            console.log("jsonString", jsonString)
            jsonString = jsonString.replace(/\"/g,"\\\\\\\"")
//            console.log("jsonString", jsonString)

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
            console.log("State:", this.state, contactInfoSaveQuery);
            if (this.state === "Ready"){

                var dataModelLocal;

                if (contactInfoSaveQuery.ContainsKey("errors")){
                    dataModelLocal = contactInfoSaveQuery.GetData("errors");

                    if (dataModelLocal.ContainsKey(containerContactInfo.gqlModelQueryType)){
                        dataModelLocal = dataModelLocal.GetData(containerContactInfo.gqlModelQueryType);
                        var messageError = dataModelLocal.GetData("message");
                        containerContactInfo.openMessageDialog("Error dialog", messageError, "ErrorDialog");
                    }

                    return;
                }

                dataModelLocal = contactInfoSaveQuery.GetData("data");

                containerContactInfo.multiDocViewItem.activeCollectionItem.refresh();

                if (dataModelLocal.ContainsKey(containerContactInfo.gqlModelQueryType)) {
                    dataModelLocal = dataModelLocal.GetData(containerContactInfo.gqlModelQueryType);

                    if (dataModelLocal.ContainsKey(containerContactInfo.gqlModelQueryTypeNotification)) {
                        dataModelLocal = dataModelLocal.GetData(containerContactInfo.gqlModelQueryTypeNotification);

                        if (dataModelLocal.ContainsKey("Id")) {
                            var oldId = containerContactInfo.itemId;
                            containerContactInfo.itemId = dataModelLocal.GetData("Id");

                            var accName = containerContactInfo.contactInfoModel.GetData("AccountName");
                            if (containerContactInfo.itemName !== accName){

                                containerContactInfo.rootItem.updateTitleTab(containerContactInfo.itemId, accName);
                                containerContactInfo.itemName = accName;
                            }

//                            if (oldId !== containerContactInfo.itemId){
//                                var accName = containerContactInfo.contactInfoModel.GetData("AccountName");
//                                containerContactInfo.rootItem.updateTitleTab(containerContactInfo.itemId, accName);
//                            }
                            containerContactInfo.multiDocViewItem.activeCollectionItem.callMetaInfoQuery();
                        }
                    }
                }
            }
        }
    }
}


