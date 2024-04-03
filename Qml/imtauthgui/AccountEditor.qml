import QtQuick 2.15
import Acf 1.0
import imtcontrols 1.0
import imtgui 1.0
import imtdocgui 1.0
import imtcolgui 1.0

ViewBase {
    id: accountEditorContainer;

    anchors.fill: parent;

    property int radius: 3;

    property int textInputHeight: 30;

    Component.onCompleted: {
        CachedGroupCollection.updateModel();

        Events.subscribeEvent("OnLocalizationChanged", accountEditorContainer.onLocalizationChanged);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("OnLocalizationChanged", accountEditorContainer.onLocalizationChanged);
    }

    function onLocalizationChanged(language){
        bodyColumn.updateHeaders();
    }

    function setReadOnly(readOnly){
        accountNameInput.readOnly = readOnly;
        accountDescriptionInput.readOnly = readOnly;
        countryInput.readOnly = readOnly;
        streetInput.readOnly = readOnly;
        postalCodeInput.readOnly = readOnly;
        cityInput.readOnly = readOnly;
        emailInput.readOnly = readOnly;
        groupsTable.readOnly = readOnly;
    }

    function updateGui(){
        console.log("AccountEditor updateGui");

        if (accountEditorContainer.model.ContainsKey("Name")){
            accountNameInput.text = accountEditorContainer.model.GetData("Name");
        }
        else{
            accountNameInput.text = "";
        }

        if (accountEditorContainer.model.ContainsKey("Description")){
            accountDescriptionInput.text = accountEditorContainer.model.GetData("Description");
        }
        else{
            accountDescriptionInput.text = "";
        }

        if (accountEditorContainer.model.ContainsKey("Country")){
            countryInput.text = accountEditorContainer.model.GetData("Country");
        }
        else{
            countryInput.text = "";
        }

        if (accountEditorContainer.model.ContainsKey("PostalCode")){
            postalCodeInput.text = accountEditorContainer.model.GetData("PostalCode");
        }
        else{
            postalCodeInput.text = "";
        }

        if (accountEditorContainer.model.ContainsKey("City")){
            cityInput.text = accountEditorContainer.model.GetData("City");
        }
        else{
            cityInput.text = "";
        }

        if (accountEditorContainer.model.ContainsKey("Street")){
            streetInput.text = accountEditorContainer.model.GetData("Street");
        }
        else{
            streetInput.text = "";
        }

        if (accountEditorContainer.model.ContainsKey("Email")){
            emailInput.text = accountEditorContainer.model.GetData("Email");
        }
        else{
            emailInput.text = "";
        }

        let groupIds = [];
        if (accountEditorContainer.model.ContainsKey("Groups")){
            let groups = accountEditorContainer.model.GetData("Groups")
            if (groups !== ""){
                groupIds = groups.split(';');
            }
        }

        groupsTable.uncheckAll();

        console.log("groupsTable.elements", groupsTable.elements);

        if (groupsTable.elements){
            for (let i = 0; i < groupsTable.elements.GetItemsCount(); i++){
                let id = groupsTable.elements.GetData("Id", i);
                if (groupIds.includes(id)){
                    groupsTable.checkItem(i);
                }
            }
        }
    }

    function updateModel(){
        let name = accountNameInput.text;
        accountEditorContainer.model.SetData("Name", name)

        let description = accountDescriptionInput.text;
        accountEditorContainer.model.SetData("Description", description)

        let country = countryInput.text;
        accountEditorContainer.model.SetData("Country", country)

        let postalCode = postalCodeInput.text;
        accountEditorContainer.model.SetData("PostalCode", postalCode)

        let city = cityInput.text;
        accountEditorContainer.model.SetData("City", city)

        let street = streetInput.text;
        accountEditorContainer.model.SetData("Street", street)

        let email = emailInput.text;
        accountEditorContainer.model.SetData("Email", email);

        let selectedGroupIds = []
        let indexes = groupsTable.getCheckedItems();
        for (let index of indexes){
            let id = groupsTable.elements.GetData("Id", index);
            selectedGroupIds.push(id)
        }

        selectedGroupIds.sort()

        let groups = selectedGroupIds.join(';');
        accountEditorContainer.model.SetData("Groups", groups)
    }

    Rectangle {
        anchors.fill: parent;

        color: Style.backgroundColor;
    }

    CustomScrollbar {
        id: scrollbar;

        anchors.left: flickable.right;
        anchors.leftMargin: 5;
        anchors.top: parent.top;
        anchors.bottom: parent.bottom;

        secondSize: 10;
        targetItem: flickable;

        visible: accountEditorContainer.visible;
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
                    accountEditorContainer.doUpdateModel();
                }

                KeyNavigation.tab: accountDescriptionInput;

                Component.onCompleted: {
                    let ok = PermissionsController.checkPermission("ChangeAccount");

                    accountNameInput.readOnly = !ok;
                }
            }

            TextFieldWithTitle {
                id: accountDescriptionInput;

                width: parent.width;

                title: qsTr("Account description");
                placeHolderText: qsTr("Enter the account description");

                onEditingFinished: {
                    accountEditorContainer.doUpdateModel();
                }

                KeyNavigation.tab: emailInput;

                Component.onCompleted: {
                    let ok = PermissionsController.checkPermission("ChangeAccount");

                    accountDescriptionInput.readOnly = !ok;
                }
            }

            TextFieldWithTitle {
                id: emailInput;

                width: parent.width;

                title: qsTr("Email");
                placeHolderText: qsTr("Enter the email");

                textInputValidator: mailValid;

                onEditingFinished: {
                    accountEditorContainer.doUpdateModel();
                }

                KeyNavigation.tab: countryInput;

                Component.onCompleted: {
                    let ok = PermissionsController.checkPermission("ChangeAccount");

                    emailInput.readOnly = !ok;
                }
            }

            RegularExpressionValidator {
                id: mailValid;

                regularExpression: /\w+([-+.']\w+)*@\w+([-.]\w+)*\.\w+([-.]\w+)*/;
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
                            accountEditorContainer.doUpdateModel();
                        }

                        KeyNavigation.tab: cityInput;

                        Component.onCompleted: {
                            let ok = PermissionsController.checkPermission("ChangeAccount");

                            countryInput.readOnly = !ok;
                        }
                    }

                    TextFieldWithTitle {
                        id: cityInput;

                        width: parent.width;

                        title: qsTr("City");
                        placeHolderText: qsTr("Enter the city");

                        onEditingFinished: {
                            accountEditorContainer.doUpdateModel();
                        }

                        KeyNavigation.tab: postalCodeInput;

                        Component.onCompleted: {
                            let ok = PermissionsController.checkPermission("ChangeAccount");

                            cityInput.readOnly = !ok;
                        }
                    }

                    TextFieldWithTitle {
                        id: postalCodeInput;

                        width: parent.width;

                        title: qsTr("Postal Code");
                        placeHolderText: qsTr("Enter the postal code");

                        onEditingFinished: {
                            accountEditorContainer.doUpdateModel();
                        }

                        KeyNavigation.tab: streetInput;

                        Component.onCompleted: {
                            let ok = PermissionsController.checkPermission("ChangeAccount");

                            postalCodeInput.readOnly = !ok;
                        }
                    }

                    TextFieldWithTitle {
                        id: streetInput;

                        width: parent.width;

                        title: qsTr("Street");
                        placeHolderText: qsTr("Enter the street");

                        onEditingFinished: {
                            accountEditorContainer.doUpdateModel();
                        }

                        KeyNavigation.tab: accountNameInput;

                        Component.onCompleted: {
                            let ok = PermissionsController.checkPermission("ChangeAccount");

                            streetInput.readOnly = !ok;
                        }
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

                Table {
                    id: groupsTable;

                    width: parent.width;
                    height: 250;

                    checkable: true;

                    radius: accountEditorContainer.radius;

                    onCheckedItemsChanged: {
                        console.log("onCheckedItemsChanged");
                        accountEditorContainer.doUpdateModel();
                    }

                    elements: CachedGroupCollection.collectionModel;

                    Component.onCompleted: {
                        let ok = PermissionsController.checkPermission("ChangeAccount");

                        groupsTable.readOnly = !ok;
                    }
                }
            }
        }//Body column

    }//Flickable
}// Account Editor container
