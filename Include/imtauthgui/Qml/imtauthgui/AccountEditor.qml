import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

DocumentBase {
    id: accountEditorContainer;

    anchors.fill: parent;

    commandsDelegateSourceComp: AccountEditorCommandsDelegate {}

    property int textInputHeight: 30;

    Component.onCompleted: {
        accountNameInput.focus = true;
    }

    onDocumentModelChanged: {
        updateGui();
        undoRedoManager.registerModel(documentModel);
    }

    UndoRedoManager {
        id: undoRedoManager;

        commandsId: accountEditorContainer.commandsId;

        onModelStateChanged: {
            updateGui();
        }
    }

    function updateGui(){
        console.log("AccountEditor updateGui");
        accountNameInput.text = documentModel.GetData("Name");
        accountDescriptionInput.text = documentModel.GetData("Description");

        if (documentModel.ContainsKey("Addresses")){
            let addressesModel = documentModel.GetData("Addresses");

            countryInput.text = addressesModel.GetData("Country");
            postalCodeInput.text = addressesModel.GetData("PostalCode");
            cityInput.text = addressesModel.GetData("City");
            streetInput.text = addressesModel.GetData("Street");
        }

        if (documentModel.ContainsKey("FirstName")){
            firstNameInput.text = documentModel.GetData("FirstName");
        }
        if (documentModel.ContainsKey("LastName")){
            lastNameInput.text = documentModel.GetData("LastName");
        }
        if (documentModel.ContainsKey("NickName")){
            nickNameInput.text = documentModel.GetData("NickName");
        }
        if (documentModel.ContainsKey("Email")){
            emailInput.text = documentModel.GetData("Email");
        }
        if (documentModel.ContainsKey("BirthDay")){
            birthDayInput.text = documentModel.GetData("BirthDay");
        }
    }

    function updateModel(){
        console.log("updateModel");

        undoRedoManager.beginChanges();

        let name = accountNameInput.text;
        documentModel.SetData("Name", name)

        let description = accountDescriptionInput.text;
        documentModel.SetData("Description", description)

        if (documentModel.ContainsKey("Addresses")){
            let adressesModel = documentModel.GetData("Addresses");

            let country = countryInput.text;
            adressesModel.SetData("Country", country)

            let postalCode = postalCodeInput.text;
            adressesModel.SetData("PostalCode", postalCode)

            let city = cityInput.text;
            adressesModel.SetData("City", city)

            let street = streetInput.text;
            adressesModel.SetData("Street", street)
        }

        let firstName = firstNameInput.text;
        documentModel.SetData("FirstName", firstName)

        let lastName = lastNameInput.text;
        documentModel.SetData("LastName", lastName)

        let nickName = nickNameInput.text;
        documentModel.SetData("NickName", nickName)

        let email = emailInput.text;
        documentModel.SetData("Email", email)

        let birthDay = birthDayInput.text;
        documentModel.SetData("BirthDay", birthDay)

        undoRedoManager.endChanges();
    }

    Rectangle {
        anchors.fill: parent;

        color: Style.backgroundColor;
    }

    Flickable {
        anchors.fill: parent;

        contentWidth: bodyColumn.width;
        contentHeight: bodyColumn.height + 50;

        boundsBehavior: Flickable.StopAtBounds;

        Column {
            id: bodyColumn;

            anchors.left: parent.left;
            anchors.leftMargin: 20;

            width: 450;

            spacing: 7;

            Text {
                id: titleAccountName;
                text: qsTr("Account name");
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }

            CustomTextField {
                id: accountNameInput;

                height: accountEditorContainer.textInputHeight;
                width: bodyColumn.width;

                onEditingFinished: {
                    let oldText = documentModel.GetData("Name");
                    if (oldText != accountNameInput.text){
                        updateModel();
                    }
                }

                KeyNavigation.tab: accountDescriptionInput;
            }

            Text {
                id: titleAccountDescription;
                text: qsTr("Account description");
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }

            CustomTextField {
                id: accountDescriptionInput;

                height: accountEditorContainer.textInputHeight;
                width: bodyColumn.width;

                onEditingFinished: {
                    let oldText = documentModel.GetData("Description");
                    if (oldText != accountDescriptionInput.text){
                        updateModel();
                    }
                }

                KeyNavigation.tab: countryInput;
            }

            Text {
                text: qsTr("Company address");
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }

            Rectangle {
                id: companyAddressBlockBorders;

                width: parent.width;
                height: companyAddressBlock.height + 25;

                color: "transparent";

                border.width: 1;
                border.color: Style.borderColor;

                Column {
                    id: companyAddressBlock;

                    anchors.horizontalCenter: companyAddressBlockBorders.horizontalCenter;
                    anchors.verticalCenter: companyAddressBlockBorders.verticalCenter;

                    width: parent.width - 20;

                    spacing: 7;

                    Text {
                        id: countryTitle;
                        text: qsTr("Country");
                        color: Style.textColor;
                        font.family: Style.fontFamily;
                        font.pixelSize: Style.fontSize_common;
                    }

                    CustomTextField {
                        id: countryInput;

                        height: accountEditorContainer.textInputHeight;
                        width: companyAddressBlock.width;

                        onEditingFinished: {
                            if (documentModel.ContainsKey("Addresses")){
                                let adressesModel = documentModel.GetData("Addresses");
                                let oldText = adressesModel.GetData("Country");
                                if (oldText != countryInput.text){
                                    updateModel();
                                }
                            }
                        }

                        KeyNavigation.tab: cityInput;
                    }

                    Text {
                        text: qsTr("City");
                        color: Style.textColor;
                        font.family: Style.fontFamily;
                        font.pixelSize: Style.fontSize_common;
                    }

                    CustomTextField {
                        id: cityInput;

                        height: accountEditorContainer.textInputHeight;
                        width: companyAddressBlock.width;

                        onEditingFinished: {
                            if (documentModel.ContainsKey("Addresses")){
                                let adressesModel = documentModel.GetData("Addresses");
                                let oldText = adressesModel.GetData("City");
                                if (oldText != cityInput.text){
                                    updateModel();
                                }
                            }
                        }
                        KeyNavigation.tab: postalCodeInput;
                    }

                    Text {
                        text: qsTr("Postal Code");
                        color: Style.textColor;
                        font.family: Style.fontFamily;
                        font.pixelSize: Style.fontSize_common;
                    }

                    CustomTextField {
                        id: postalCodeInput;

                        height: accountEditorContainer.textInputHeight;
                        width: companyAddressBlock.width;

                        onEditingFinished: {
                            if (documentModel.ContainsKey("Addresses")){
                                let adressesModel = documentModel.GetData("Addresses");
                                let oldText = adressesModel.GetData("PostalCode");
                                if (oldText != postalCodeInput.text){
                                    updateModel();
                                }
                            }
                        }
                        KeyNavigation.tab: streetInput;
                    }

                    Text {
                        text: qsTr("Street");
                        color: Style.textColor;
                        font.family: Style.fontFamily;
                        font.pixelSize: Style.fontSize_common;
                    }

                    CustomTextField {
                        id: streetInput;

                        height: accountEditorContainer.textInputHeight;
                        width: companyAddressBlock.width;

                        onEditingFinished: {
                            if (documentModel.ContainsKey("Addresses")){
                                let adressesModel = documentModel.GetData("Addresses");
                                let oldText = adressesModel.GetData("Street");
                                if (oldText != streetInput.text){
                                    updateModel();
                                }
                            }
                        }

                        KeyNavigation.tab: emailInput;
                    }
                }// Company address block
            }//Company address block borders

            Text {
                text: qsTr("Account Owner");
                color: Style.textColor;

                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }

            Rectangle {
                id: accountOwnerBlockBorders;

                width: parent.width;
                height: accountOwnerBlock.height + 25;

                color: "transparent";

                border.width: 1;
                border.color: Style.borderColor;

                Column {
                    id: accountOwnerBlock;

                    anchors.horizontalCenter: accountOwnerBlockBorders.horizontalCenter;
                    anchors.verticalCenter: accountOwnerBlockBorders.verticalCenter;

                    width: parent.width - 20;

                    spacing: 7;

                    Text {
                        text: qsTr("Email");
                        color: Style.textColor;
                        font.family: Style.fontFamily;
                        font.pixelSize: Style.fontSize_common;
                    }

                    CustomTextField {
                        id: emailInput;

                        height: accountEditorContainer.textInputHeight;
                        width: accountOwnerBlock.width;

                        onEditingFinished: {
                            let oldText = documentModel.GetData("Email");
                            if (oldText != emailInput.text){
                                updateModel();
                            }
                        }

                        KeyNavigation.tab: birthDayInput;
                    }

                    Text {
                        text: qsTr("Birthday");
                        color: Style.textColor;
                        font.family: Style.fontFamily;
                        font.pixelSize: Style.fontSize_common;
                    }

                    CustomTextField {
                        id: birthDayInput;

                        height: accountEditorContainer.textInputHeight;
                        width: accountOwnerBlock.width;

                        onEditingFinished: {
                            let oldText = documentModel.GetData("BirthDay");
                            if (oldText != birthDayInput.text){
                                updateModel();
                            }
                        }

                        KeyNavigation.tab: firstNameInput;
                    }

                    Text {
                        text: qsTr("First Name");
                        color: Style.textColor;
                        font.family: Style.fontFamily;
                        font.pixelSize: Style.fontSize_common;
                    }

                    CustomTextField {
                        id: firstNameInput;

                        height: accountEditorContainer.textInputHeight;
                        width: accountOwnerBlock.width;

                        onEditingFinished: {
                            let oldText = documentModel.GetData("FirstName");
                            if (oldText != firstNameInput.text){
                                updateModel();
                            }
                        }

                        KeyNavigation.tab: lastNameInput;
                    }

                    Text {
                        text: qsTr("Last Name");
                        color: Style.textColor;
                        font.family: Style.fontFamily;
                        font.pixelSize: Style.fontSize_common;
                    }

                    CustomTextField {
                        id: lastNameInput;

                        height: accountEditorContainer.textInputHeight;
                        width: accountOwnerBlock.width;

                        onEditingFinished: {
                            let oldText = documentModel.GetData("LastName");
                            if (oldText != lastNameInput.text){
                                updateModel();
                            }
                        }

                        KeyNavigation.tab: nickNameInput;
                    }


                    Text {
                        text: qsTr("Nickname");
                        color: Style.textColor;
                        font.family: Style.fontFamily;
                        font.pixelSize: Style.fontSize_common;
                    }

                    CustomTextField {
                        id: nickNameInput;

                        height: accountEditorContainer.textInputHeight;
                        width: accountOwnerBlock.width;

                        onEditingFinished: {
                            let oldText = documentModel.GetData("NickName");
                            if (oldText != nickNameInput.text){
                                updateModel();
                            }
                        }

                        KeyNavigation.tab: accountNameInput;
                    }
                } // Account owner block
            } //Account owner borders
        }//Body column
    }//Flickable
}// Account Editor container
