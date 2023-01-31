import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

DocumentBase {
    id: accountEditorContainer;

    anchors.fill: parent;

    commandsDelegateSourceComp: accountEditorCommandsDelegate;

    property int textInputHeight: 30;

    property bool blockUpdatingModel: false;

    Component.onCompleted: {
        accountNameInput.focus = true;
    }

    Component.onDestruction: {
        console.log("AccountEditor onDestruction");
    }

    onDocumentModelChanged: {
        accountEditorContainer.updateGui();
        undoRedoManager.registerModel(accountEditorContainer.documentModel);
    }

    Component {
        id: accountEditorCommandsDelegate;
        AccountEditorCommandsDelegate {}
    }

    UndoRedoManager {
        id: undoRedoManager;

        commandsId: accountEditorContainer.commandsId;

        commandsDelegate: accountEditorContainer.commandsDelegate;

        onModelStateChanged: {
            accountEditorContainer.updateGui();
        }
    }

    function updateGui(){
        console.log("AccountEditor updateGui");
        accountEditorContainer.blockUpdatingModel = true;

        accountNameInput.text = accountEditorContainer.documentModel.GetData("Name");
        accountDescriptionInput.text = accountEditorContainer.documentModel.GetData("Description");

        if (accountEditorContainer.documentModel.ContainsKey("Addresses")){
            let addressesModel = accountEditorContainer.documentModel.GetData("Addresses");

            countryInput.text = addressesModel.GetData("Country");
            postalCodeInput.text = addressesModel.GetData("PostalCode");
            cityInput.text = addressesModel.GetData("City");
            streetInput.text = addressesModel.GetData("Street");
        }

        if (accountEditorContainer.documentModel.ContainsKey("FirstName")){
            firstNameInput.text = accountEditorContainer.documentModel.GetData("FirstName");
        }
        if (accountEditorContainer.documentModel.ContainsKey("LastName")){
            lastNameInput.text = accountEditorContainer.documentModel.GetData("LastName");
        }
        if (accountEditorContainer.documentModel.ContainsKey("Email")){
            emailInput.text = accountEditorContainer.documentModel.GetData("Email");
        }

        accountEditorContainer.blockUpdatingModel = false;
    }

    function updateModel(){
        console.log("updateModel");

        if (accountEditorContainer.blockUpdatingModel){
            return;
        }

        undoRedoManager.beginChanges();

        let name = accountNameInput.text;
        accountEditorContainer.documentModel.SetData("Name", name)

        let description = accountDescriptionInput.text;
        accountEditorContainer.documentModel.SetData("Description", description)

        if (accountEditorContainer.documentModel.ContainsKey("Addresses")){
            let adressesModel = accountEditorContainer.documentModel.GetData("Addresses");

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
        accountEditorContainer.documentModel.SetData("FirstName", firstName)

        let lastName = lastNameInput.text;
        accountEditorContainer.documentModel.SetData("LastName", lastName)

        let email = emailInput.text;
        accountEditorContainer.documentModel.SetData("Email", email)

        undoRedoManager.endChanges();
    }

    Rectangle {
        anchors.fill: parent;

        color: Style.backgroundColor;
    }

    CustomScrollbar {
        id: scrollbar;
        z: 100;

        anchors.left: flickable.right;
        anchors.leftMargin: 5;
        anchors.top: parent.top;
        anchors.bottom: parent.bottom;

        secondSize: 10;
        targetItem: flickable;
    }

    Flickable {
        id: flickable;
        anchors.top: parent.top;
        anchors.bottom: parent.bottom;
        anchors.left: parent.left;
        anchors.leftMargin: 20;

        width: 450;
        contentWidth: bodyColumn.width;
        contentHeight: bodyColumn.height;

        boundsBehavior: Flickable.StopAtBounds;

        Column {
            id: bodyColumn;

            width: flickable.width;

            spacing: 7;

            Text {
                id: titleAccountName;

                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;

                text: qsTr("Account name");
            }

            CustomTextField {
                id: accountNameInput;

                height: accountEditorContainer.textInputHeight;
                width: bodyColumn.width;

                onEditingFinished: {
                    let oldText = accountEditorContainer.documentModel.GetData("Name");
                    if (oldText != accountNameInput.text){
                        accountEditorContainer.updateModel();
                    }
                }

                KeyNavigation.tab: accountDescriptionInput;
            }

            Text {
                id: titleAccountDescription;

                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;

                text: qsTr("Account description");
            }

            CustomTextField {
                id: accountDescriptionInput;

                height: accountEditorContainer.textInputHeight;
                width: bodyColumn.width;

                onEditingFinished: {
                    let oldText = documentModel.GetData("Description");
                    if (oldText != accountDescriptionInput.text){
                        accountEditorContainer.updateModel();
                    }
                }

                KeyNavigation.tab: countryInput;
            }

            Text {
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;

                text: qsTr("Company address");
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

                        color: Style.textColor;
                        font.family: Style.fontFamily;
                        font.pixelSize: Style.fontSize_common;

                        text: qsTr("Country");
                    }

                    CustomTextField {
                        id: countryInput;

                        height: accountEditorContainer.textInputHeight;
                        width: companyAddressBlock.width;

                        onEditingFinished: {
                            if (accountEditorContainer.documentModel.ContainsKey("Addresses")){
                                let adressesModel = accountEditorContainer.documentModel.GetData("Addresses");
                                let oldText = adressesModel.GetData("Country");
                                if (oldText != countryInput.text){
                                    accountEditorContainer.updateModel();
                                }
                            }
                        }

                        KeyNavigation.tab: cityInput;
                    }

                    Text {
                        color: Style.textColor;
                        font.family: Style.fontFamily;
                        font.pixelSize: Style.fontSize_common;

                        text: qsTr("City");
                    }

                    CustomTextField {
                        id: cityInput;

                        height: accountEditorContainer.textInputHeight;
                        width: companyAddressBlock.width;

                        onEditingFinished: {
                            if (accountEditorContainer.documentModel.ContainsKey("Addresses")){
                                let adressesModel = accountEditorContainer.documentModel.GetData("Addresses");
                                let oldText = adressesModel.GetData("City");
                                if (oldText != cityInput.text){
                                    accountEditorContainer.updateModel();
                                }
                            }
                        }
                        KeyNavigation.tab: postalCodeInput;
                    }

                    Text {
                        color: Style.textColor;
                        font.family: Style.fontFamily;
                        font.pixelSize: Style.fontSize_common;

                        text: qsTr("Postal Code");
                    }

                    CustomTextField {
                        id: postalCodeInput;

                        height: accountEditorContainer.textInputHeight;
                        width: companyAddressBlock.width;

                        onEditingFinished: {
                            if (accountEditorContainer.documentModel.ContainsKey("Addresses")){
                                let adressesModel = accountEditorContainer.documentModel.GetData("Addresses");
                                let oldText = adressesModel.GetData("PostalCode");
                                if (oldText != postalCodeInput.text){
                                    accountEditorContainer.updateModel();
                                }
                            }
                        }
                        KeyNavigation.tab: streetInput;
                    }

                    Text {
                        color: Style.textColor;
                        font.family: Style.fontFamily;
                        font.pixelSize: Style.fontSize_common;

                        text: qsTr("Street");
                    }

                    CustomTextField {
                        id: streetInput;

                        height: accountEditorContainer.textInputHeight;
                        width: companyAddressBlock.width;

                        onEditingFinished: {
                            if (accountEditorContainer.documentModel.ContainsKey("Addresses")){
                                let adressesModel = accountEditorContainer.documentModel.GetData("Addresses");
                                let oldText = adressesModel.GetData("Street");
                                if (oldText != streetInput.text){
                                    accountEditorContainer.updateModel();
                                }
                            }
                        }

                        KeyNavigation.tab: emailInput;
                    }
                }// Company address block
            }//Company address block borders

            Text {
                color: Style.textColor;

                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;

                text: qsTr("Account Owner");
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
                        color: Style.textColor;
                        font.family: Style.fontFamily;
                        font.pixelSize: Style.fontSize_common;

                        text: qsTr("Email");
                    }

                    RegExpValidator {
                        id: mailValid;

                        regExp: /\w+([-+.']\w+)*@\w+([-.]\w+)*\.\w+([-.]\w+)*/;
                    }

                    CustomTextField {
                        id: emailInput;

                        height: accountEditorContainer.textInputHeight;
                        width: accountOwnerBlock.width;

                        textInputValidator: mailValid;

                        onEditingFinished: {
                            let oldText = accountEditorContainer.documentModel.GetData("Email");
                            if (oldText != emailInput.text){
                                accountEditorContainer.updateModel();
                            }
                        }

                        KeyNavigation.tab: firstNameInput;
                    }



                    Text {
                        color: Style.textColor;
                        font.family: Style.fontFamily;
                        font.pixelSize: Style.fontSize_common;

                        text: qsTr("First Name");
                    }

                    CustomTextField {
                        id: firstNameInput;

                        height: accountEditorContainer.textInputHeight;
                        width: accountOwnerBlock.width;

                        onEditingFinished: {
                            let oldText = accountEditorContainer.documentModel.GetData("FirstName");
                            if (oldText != firstNameInput.text){
                                accountEditorContainer.updateModel();
                            }
                        }

                        KeyNavigation.tab: lastNameInput;
                    }

                    Text {
                        color: Style.textColor;
                        font.family: Style.fontFamily;
                        font.pixelSize: Style.fontSize_common;

                        text: qsTr("Last Name");
                    }

                    CustomTextField {
                        id: lastNameInput;

                        width: accountOwnerBlock.width;
                        height: accountEditorContainer.textInputHeight;

                        onEditingFinished: {
                            let oldText = accountEditorContainer.documentModel.GetData("LastName");
                            if (oldText != lastNameInput.text){
                                accountEditorContainer.updateModel();
                            }
                        }

                        KeyNavigation.tab: accountNameInput;
                    }
                } // Account owner block
            } //Account owner borders
        }//Body column
    }//Flickable
}// Account Editor container
