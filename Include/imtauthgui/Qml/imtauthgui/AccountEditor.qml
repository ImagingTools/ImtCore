import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

DocumentBase {
    id: accountEditorContainer;

    anchors.fill: parent;

    property int textInputHeight: 30;
    property bool modelsIsLoaded: groupsProvider.completed && accountEditorContainer.modelIsReady;

    Component.onCompleted: {
        groupsProvider.updateModel();
    }

    onModelIsReadyChanged: {
        if (accountEditorContainer.modelIsReady){
            accountEditorContainer.updateGui();
            undoRedoManager.registerModel(accountEditorContainer.documentModel);

            accountNameInput.focus = true;
        }
    }

    UndoRedoManager {
        id: undoRedoManager;

        commandsId: accountEditorContainer.documentUuid;

        commandsDelegate: accountEditorContainer.commandsDelegate;
        documentBase: accountEditorContainer;

        onModelStateChanged: {
            accountEditorContainer.updateGui();
        }
    }

    CollectionDataProvider {
        id: groupsProvider;

        commandId: "Groups";

        fields: ["Id", "Name"];

        onModelUpdated: {
            if (groupsProvider.collectionModel != null){
                for (let i = 0; i < groupsProvider.collectionModel.GetItemsCount(); i++){
                    groupsProvider.collectionModel.SetData("CheckedState", Qt.Unchecked, i);
                }
                groupsTable.elements = groupsProvider.collectionModel;
                groupsProvider.collectionModel.modelChanged.connect(accountEditorContainer.updateModel);
            }
        }
    }

    function updateGui(){
        console.log("AccountEditor updateGui");
        accountEditorContainer.blockUpdatingModel = true;

        if (accountEditorContainer.documentModel.ContainsKey("Name")){
            accountNameInput.text = accountEditorContainer.documentModel.GetData("Name");
        }

        if (accountEditorContainer.documentModel.ContainsKey("Description")){
            accountDescriptionInput.text = accountEditorContainer.documentModel.GetData("Description");
        }

        if (accountEditorContainer.documentModel.ContainsKey("Country")){
            countryInput.text = accountEditorContainer.documentModel.GetData("Country");
        }

        if (accountEditorContainer.documentModel.ContainsKey("PostalCode")){
            postalCodeInput.text = accountEditorContainer.documentModel.GetData("PostalCode");
        }

        if (accountEditorContainer.documentModel.ContainsKey("City")){
            cityInput.text = accountEditorContainer.documentModel.GetData("City");
        }

        if (accountEditorContainer.documentModel.ContainsKey("Street")){
            streetInput.text = accountEditorContainer.documentModel.GetData("Street");
        }

        if (accountEditorContainer.documentModel.ContainsKey("CompanyName")){
            companyNameInput.text = accountEditorContainer.documentModel.GetData("CompanyName");
        }

        if (accountEditorContainer.documentModel.ContainsKey("Email")){
            emailInput.text = accountEditorContainer.documentModel.GetData("Email");
        }

        let groupIds = [];
        if (accountEditorContainer.documentModel.ContainsKey("Groups")){
            let groups = accountEditorContainer.documentModel.GetData("Groups")
            if (groups !== ""){
                groupIds = groups.split(';');
            }
        }

        if (groupsTable.elements){
            for (let i = 0; i < groupsTable.elements.GetItemsCount(); i++){
                let id = groupsTable.elements.GetData("Id", i);
                if (groupIds.includes(id)){
                    groupsTable.elements.SetData("CheckedState", Qt.Checked, i);
                }
                else{
                    groupsTable.elements.SetData("CheckedState", Qt.Unchecked, i);
                }
            }
        }

        accountEditorContainer.blockUpdatingModel = false;
    }

    function updateModel(){
        console.log("updateModel");

        if (accountEditorContainer.blockUpdatingModel){
            return;
        }

//        undoRedoManager.beginChanges();

//        accountEditorContainer.documentModel.SetData("Id", accountEditorContainer.itemId);

        let name = accountNameInput.text;
        accountEditorContainer.documentModel.SetData("Name", name)

        let description = accountDescriptionInput.text;
        accountEditorContainer.documentModel.SetData("Description", description)

        let country = countryInput.text;
        accountEditorContainer.documentModel.SetData("Country", country)

        let postalCode = postalCodeInput.text;
        accountEditorContainer.documentModel.SetData("PostalCode", postalCode)

        let city = cityInput.text;
        accountEditorContainer.documentModel.SetData("City", city)

        let street = streetInput.text;
        accountEditorContainer.documentModel.SetData("Street", street)

        let companyName = companyNameInput.text;
        accountEditorContainer.documentModel.SetData("CompanyName", companyName)

        let email = emailInput.text;
        accountEditorContainer.documentModel.SetData("Email", email)

        let selectedGroupIds = []
        if (groupsTable.elements){
            for (let i = 0; i < groupsTable.elements.GetItemsCount(); i++){
                let id = groupsTable.elements.GetData("Id", i);
                let state = groupsTable.elements.GetData("CheckedState", i);
                if (state === Qt.Checked){
                    selectedGroupIds.push(id)
                }
            }
        }

        let groups = selectedGroupIds.join(';');
        accountEditorContainer.documentModel.SetData("Groups", groups)

//        undoRedoManager.endChanges();
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

        backgroundColor: Style.baseColor;

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

                placeHolderText: qsTr("Enter the account name");

                onEditingFinished: {
                    let oldText = accountEditorContainer.documentModel.GetData("Name");
                    if (oldText && oldText !== accountNameInput.text || !oldText && accountNameInput.text !== ""){
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

                placeHolderText: qsTr("Enter the account description");

                onEditingFinished: {
                    console.log("accountDescriptionInput");
                    let oldText = accountEditorContainer.documentModel.GetData("Description");
                    if (oldText && oldText !== accountDescriptionInput.text || !oldText && accountDescriptionInput.text !== ""){
                        accountEditorContainer.updateModel();
                    }
                }

                KeyNavigation.tab: emailInput;
            }

            Text {
                color: Style.textColor;

                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;

                text: qsTr("Additional information");
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

                        placeHolderText: qsTr("Enter the email");

                        onEditingFinished: {
                            console.log("emailInput");
                            let oldText = accountEditorContainer.documentModel.GetData("Email");
                            if (oldText && oldText !== emailInput.text || !oldText && emailInput.text !== ""){
                                accountEditorContainer.updateModel();
                            }
                        }

                        KeyNavigation.tab: companyNameInput;
                    }



                    Text {
                        color: Style.textColor;
                        font.family: Style.fontFamily;
                        font.pixelSize: Style.fontSize_common;

                        text: qsTr("Company Name");
                    }

                    CustomTextField {
                        id: companyNameInput;

                        height: accountEditorContainer.textInputHeight;
                        width: accountOwnerBlock.width;

                        placeHolderText: qsTr("Enter the company name");

                        onEditingFinished: {
                            console.log("companyNameInput");
                            let oldText = accountEditorContainer.documentModel.GetData("CompanyName");
                            if (oldText && oldText !== companyNameInput.text || !oldText && companyNameInput.text !== ""){
                                accountEditorContainer.updateModel();
                            }
                        }

                        KeyNavigation.tab: countryInput;
                    }

                } // Account owner block
            } //Account owner borders

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

                        placeHolderText: qsTr("Enter the country");

                        onEditingFinished: {
                            let oldText = accountEditorContainer.documentModel.GetData("Country");
                            if (oldText && oldText !== countryInput.text || !oldText && countryInput.text !== ""){
                                accountEditorContainer.updateModel();
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

                        placeHolderText: qsTr("Enter the city");

                        onEditingFinished: {
                            let oldText = accountEditorContainer.documentModel.GetData("City");
                            if (oldText && oldText !== cityInput.text || !oldText && cityInput.text !== ""){
                                accountEditorContainer.updateModel();
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

                        placeHolderText: qsTr("Enter the postal code");

                        onEditingFinished: {
                            let oldText = String(accountEditorContainer.documentModel.GetData("PostalCode"));
                            if (oldText && oldText !== postalCodeInput.text || !oldText && postalCodeInput.text !== ""){
                                accountEditorContainer.updateModel();
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

                        placeHolderText: qsTr("Enter the street");

                        onEditingFinished: {
                            let oldText = accountEditorContainer.documentModel.GetData("Street");
                            if (oldText && oldText !== streetInput.text || !oldText && streetInput.text !== ""){
                                accountEditorContainer.updateModel();
                            }
                        }

                        KeyNavigation.tab: accountNameInput;
                    }
                }// Company address block
            }//Company address block borders

            TreeItemModel {
                id: headersModel;

                Component.onCompleted: {
                    headersModel.InsertNewItem();

                    headersModel.SetData("Id", "Name");
                    headersModel.SetData("Name", "Group Name");

                    groupsTable.headers = headersModel;
                }
            }

            Text {
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;

                text: qsTr("Groups");
            }

            AuxTable {
                id: groupsTable;

                width: parent.width;
                height: 300;

                checkable: true;
                radius: 0;
            }

        }//Body column
    }//Flickable
}// Account Editor container
