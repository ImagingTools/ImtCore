import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

DocumentBase {
    id: accountEditorContainer;

    anchors.fill: parent;

    property int radius: 3;

    property int textInputHeight: 30;
    property bool modelsIsLoaded: groupsProvider.completed && accountEditorContainer.modelIsReady;

    Component.onCompleted: {
        groupsProvider.updateModel();

        Events.subscribeEvent("OnLocalizationChanged", accountEditorContainer.onLocalizationChanged);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("OnLocalizationChanged", accountEditorContainer.onLocalizationChanged);
    }

    function onLocalizationChanged(language){
        bodyColumn.updateHeaders();
    }

    onModelsIsLoadedChanged: {
        if (accountEditorContainer.modelIsReady){
            accountEditorContainer.updateGui();
            undoRedoManager.registerModel(accountEditorContainer.documentModel);

            accountNameInput.focus = true;
        }
    }

    UndoRedoManager {
        id: undoRedoManager;

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
            groupsTable.elements = groupsProvider.collectionModel;
        }
    }

    function blockEditing(){
        accountNameInput.readOnly = true;
        accountDescriptionInput.readOnly = true;
        countryInput.readOnly = true;
        streetInput.readOnly = true;
        postalCodeInput.readOnly = true;
        cityInput.readOnly = true;
        emailInput.readOnly = true;
        groupsTable.readOnly = true;
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

        groupsTable.uncheckAll();

        if (groupsTable.elements){
            for (let i = 0; i < groupsTable.elements.GetItemsCount(); i++){
                let id = groupsTable.elements.GetData("Id", i);
                if (groupIds.includes(id)){
                    groupsTable.checkItem(i);
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

        undoRedoManager.beginChanges();

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

        let email = emailInput.text;
        accountEditorContainer.documentModel.SetData("Email", email);

        let selectedGroupIds = []
        let indexes = groupsTable.getCheckedItems();
        for (let index of indexes){
            let id = groupsTable.elements.GetData("Id", index);
            selectedGroupIds.push(id)
        }

        let groups = selectedGroupIds.join(';');
        accountEditorContainer.documentModel.SetData("Groups", groups)

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
        interactive: !mouseArea.containsMouse;

        Column {
            id: bodyColumn;

            width: flickable.width;

            spacing: 15;

            TextFieldWithTitle {
                id: accountNameInput;

                width: parent.width;

                title: qsTr("Account name");
                placeHolderText: qsTr("Enter the account name");

                onEditingFinished: {
                    let oldText = accountEditorContainer.documentModel.GetData("Name");
                    if (oldText && oldText !== accountNameInput.text || !oldText && accountNameInput.text !== ""){
                        accountEditorContainer.updateModel();
                    }
                }

                KeyNavigation.tab: accountDescriptionInput;
            }

            TextFieldWithTitle {
                id: accountDescriptionInput;

                width: parent.width;

                title: qsTr("Account description");
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

            TextFieldWithTitle {
                id: emailInput;

                width: parent.width;

                title: qsTr("Email");
                placeHolderText: qsTr("Enter the email");

                textInputValidator: mailValid;

                onEditingFinished: {
                    console.log("emailInput");
                    let oldText = accountEditorContainer.documentModel.GetData("Email");
                    if (oldText && oldText !== emailInput.text || !oldText && emailInput.text !== ""){
                        accountEditorContainer.updateModel();
                    }
                }

                KeyNavigation.tab: countryInput;
            }

            RegExpValidator {
                id: mailValid;

                regExp: /\w+([-+.']\w+)*@\w+([-.]\w+)*\.\w+([-.]\w+)*/;
            }

            Text {
                color: Style.textColor;
                font.family: Style.fontFamilyBold;
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

                radius: accountEditorContainer.radius;

                Column {
                    id: companyAddressBlock;

                    anchors.horizontalCenter: companyAddressBlockBorders.horizontalCenter;
                    anchors.verticalCenter: companyAddressBlockBorders.verticalCenter;

                    width: parent.width - 20;

                    spacing: 15;

                    TextFieldWithTitle {
                        id: countryInput;

                        width: parent.width;

                        title: qsTr("Country");
                        placeHolderText: qsTr("Enter the country");

                        onEditingFinished: {
                            let oldText = accountEditorContainer.documentModel.GetData("Country");
                            if (oldText && oldText !== countryInput.text || !oldText && countryInput.text !== ""){
                                accountEditorContainer.updateModel();
                            }
                        }

                        KeyNavigation.tab: cityInput;
                    }

                    TextFieldWithTitle {
                        id: cityInput;

                        width: parent.width;

                        title: qsTr("City");
                        placeHolderText: qsTr("Enter the city");

                        onEditingFinished: {
                            let oldText = accountEditorContainer.documentModel.GetData("City");
                            if (oldText && oldText !== cityInput.text || !oldText && cityInput.text !== ""){
                                accountEditorContainer.updateModel();
                            }
                        }

                        KeyNavigation.tab: postalCodeInput;
                    }

                    TextFieldWithTitle {
                        id: postalCodeInput;

                        width: parent.width;

                        title: qsTr("Postal Code");
                        placeHolderText: qsTr("Enter the postal code");

                        onEditingFinished: {
                            let oldText = String(accountEditorContainer.documentModel.GetData("PostalCode"));
                            if (oldText && oldText !== postalCodeInput.text || !oldText && postalCodeInput.text !== ""){
                                accountEditorContainer.updateModel();
                            }
                        }

                        KeyNavigation.tab: streetInput;
                    }

                    TextFieldWithTitle {
                        id: streetInput;

                        width: parent.width;

                        title: qsTr("Street");
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
                    bodyColumn.updateHeaders();
                }
            }

            function updateHeaders(){
                headersModel.Clear();

                headersModel.InsertNewItem();

                headersModel.SetData("Id", "Name");
                headersModel.SetData("Name", qsTr("Group Name"));

                groupsTable.headers = headersModel;
            }

            Text {
                color: Style.textColor;
                font.family: Style.fontFamilyBold;
                font.pixelSize: Style.fontSize_common;

                text: qsTr("Groups");
            }

            Item {
                width: parent.width;
                height: groupsTable.height;

                MouseArea {
                    id: mouseArea;

                    anchors.fill: groupsTable;
                    hoverEnabled: true;
                }

                AuxTable {
                    id: groupsTable;

                    width: parent.width;
                    height: 250;

                    checkable: true;

                    radius: accountEditorContainer.radius;

                    onCheckedItemsChanged: {
                        if (accountEditorContainer.blockUpdatingModel){
                            return;
                        }

                        let indexes = groupsTable.getCheckedItems();
                        let groups = accountEditorContainer.documentModel.GetData("Groups");
                        let groupIDs = [];
                        for (let index of indexes){
                            let groupId = groupsTable.elements.GetData("Id", index);
                            groupIDs.push(groupId);
                        }

                        let newGroups = groupIDs.join(';');
                        if (groups !== newGroups){
                            accountEditorContainer.documentModel.SetData("Groups", groupIDs.join(';'));
                            undoRedoManager.makeChanges();
                        }
                    }
                }
            }
        }//Body column

    }//Flickable
}// Account Editor container
