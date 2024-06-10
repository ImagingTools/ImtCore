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
        groupsElement.table.readOnly = readOnly;
    }

    function updateGui(){
        console.log("updateGui start");
        if (accountEditorContainer.model.containsKey("Name")){
            accountNameInput.text = accountEditorContainer.model.getData("Name");
        }
        else{
            accountNameInput.text = "";
        }

        if (accountEditorContainer.model.containsKey("Description")){
            accountDescriptionInput.text = accountEditorContainer.model.getData("Description");
        }
        else{
            accountDescriptionInput.text = "";
        }

        if (accountEditorContainer.model.containsKey("Country")){
            countryInput.text = accountEditorContainer.model.getData("Country");
        }
        else{
            countryInput.text = "";
        }

        if (accountEditorContainer.model.containsKey("PostalCode")){
            postalCodeInput.text = accountEditorContainer.model.getData("PostalCode");
        }
        else{
            postalCodeInput.text = "";
        }

        if (accountEditorContainer.model.containsKey("City")){
            cityInput.text = accountEditorContainer.model.getData("City");
        }
        else{
            cityInput.text = "";
        }

        if (accountEditorContainer.model.containsKey("Street")){
            streetInput.text = accountEditorContainer.model.getData("Street");
        }
        else{
            streetInput.text = "";
        }

        if (accountEditorContainer.model.containsKey("Email")){
            emailInput.text = accountEditorContainer.model.getData("Email");
        }
        else{
            emailInput.text = "";
        }

        if (accountEditorContainer.model.containsKey("CustomerId")){
            customerIdInput.text = accountEditorContainer.model.getData("CustomerId");
        }
        else{
            customerIdInput.text = "";
        }

        let groupIds = [];
        if (accountEditorContainer.model.containsKey("Groups")){
            let groups = accountEditorContainer.model.getData("Groups")
            if (groups !== ""){
                groupIds = groups.split(';');
            }
        }

        groupsElement.table.uncheckAll();

        if (groupsElement.table.elements){
            for (let i = 0; i < groupsElement.table.elements.getItemsCount(); i++){
                let id = groupsElement.table.elements.getData("Id", i);
                if (groupIds.includes(id)){
                    groupsElement.table.checkItem(i);
                }
            }
        }

        console.log("updateGui end");
    }

    function updateModel(){
        console.log("start updateModel", model.toJson());
        let name = accountNameInput.text;
        accountEditorContainer.model.setData("Name", name)

        let description = accountDescriptionInput.text;
        accountEditorContainer.model.setData("Description", description)

        let country = countryInput.text;
        accountEditorContainer.model.setData("Country", country)

        let postalCode = postalCodeInput.text;
        accountEditorContainer.model.setData("PostalCode", postalCode)

        let city = cityInput.text;
        accountEditorContainer.model.setData("City", city)

        let street = streetInput.text;
        accountEditorContainer.model.setData("Street", street)

        let email = emailInput.text;
        accountEditorContainer.model.setData("Email", email);

        let customerId = customerIdInput.text;
        accountEditorContainer.model.setData("CustomerId", customerId);

        let selectedGroupIds = []
        let indexes = groupsElement.table.getCheckedItems();
        for (let index of indexes){
            let id = groupsElement.table.elements.getData("Id", index);
            selectedGroupIds.push(id)
        }

        selectedGroupIds.sort()

        let groups = selectedGroupIds.join(';');
        accountEditorContainer.model.setData("Groups", groups)

        console.log("end updateModel", model.toJson());
    }

    CustomScrollbar {
        id: scrollbar;
        z: parent.z + 1;

        anchors.right: parent.right;
        anchors.top: flickable.top;
        anchors.bottom: flickable.bottom;

        secondSize: 10;
        targetItem: flickable;

        visible: accountEditorContainer.visible;
    }

    CustomScrollbar{
        id: scrollHoriz;

        z: parent.z + 1;

        anchors.left: flickable.left;
        anchors.right: flickable.right;
        anchors.bottom: flickable.bottom;

        secondSize: 10;

        vertical: false;
        targetItem: flickable;
    }

    Flickable {
        id: flickable;

        anchors.top: parent.top;
        anchors.topMargin: Style.size_largeMargin;

        anchors.bottom: parent.bottom;
        anchors.bottomMargin: Style.size_largeMargin;

        anchors.left: parent.left;
        anchors.leftMargin: Style.size_largeMargin;

        anchors.right: scrollbar.left;
        anchors.rightMargin: Style.size_largeMargin;

        contentWidth: bodyColumn.width;
        contentHeight: bodyColumn.height + 2 * Style.size_largeMargin;

        boundsBehavior: Flickable.StopAtBounds;
        clip: true;

        Column {
            id: bodyColumn;

            width: 700;

            spacing: Style.size_largeMargin;

            GroupHeaderView {
                width: parent.width;

                title: qsTr("Customer Information");
                groupView: customerInformationGroup;
            }

            GroupElementView {
                id: customerInformationGroup;

                width: parent.width;

                TextInputElementView {
                    id: customerIdInput;

                    name: qsTr("Customer-ID");
                    placeHolderText: qsTr("Enter the customer-ID");

                    onEditingFinished: {
                        accountEditorContainer.doUpdateModel();
                    }

                    KeyNavigation.tab: accountNameInput;
                    KeyNavigation.backtab: groupsElement;

                    Component.onCompleted: {
                        let ok = PermissionsController.checkPermission("ChangeCustomerId");

                        customerIdInput.readOnly = !ok;
                    }
                }
            }

            GroupHeaderView {
                width: parent.width;

                title: qsTr("Account Information");
                groupView: accountInformationGroup;
            }

            GroupElementView {
                id: accountInformationGroup;

                width: parent.width;

                TextInputElementView {
                    id: accountNameInput;

                    name: qsTr("Account Name");
                    placeHolderText: qsTr("Enter the account name");

                    onEditingFinished: {
                        accountEditorContainer.doUpdateModel();
                    }

                    KeyNavigation.tab: accountDescriptionInput;
                    KeyNavigation.backtab: customerIdInput;

                    Component.onCompleted: {
                        let ok = PermissionsController.checkPermission("ChangeAccountName");

                        accountNameInput.readOnly = !ok;
                    }
                }

                TextInputElementView {
                    id: accountDescriptionInput;

                    name: qsTr("Account Description");
                    placeHolderText: qsTr("Enter the account description");

                    onEditingFinished: {
                        accountEditorContainer.doUpdateModel();
                    }

                    KeyNavigation.tab: emailInput;
                    KeyNavigation.backtab: accountNameInput;

                    Component.onCompleted: {
                        let ok = PermissionsController.checkPermission("ChangeAccountDescription");

                        accountDescriptionInput.readOnly = !ok;
                    }
                }

                TextInputElementView {
                    id: emailInput;

                    width: parent.width;

                    name: qsTr("Email");
                    placeHolderText: qsTr("Enter the email");

                    textInputValidator: mailValid;

                    onEditingFinished: {
                        accountEditorContainer.doUpdateModel();
                    }

                    KeyNavigation.tab: countryInput;
                    KeyNavigation.backtab: accountDescriptionInput;

                    Component.onCompleted: {
                        let ok = PermissionsController.checkPermission("ChangeAccountEmail");

                        emailInput.readOnly = !ok;
                    }
                }

                RegularExpressionValidator {
                    id: mailValid;

                    regularExpression: /\w+([-+.']\w+)*@\w+([-.]\w+)*\.\w+([-.]\w+)*/;
                }
            }

            GroupHeaderView {
                width: parent.width;

                title: qsTr("Company Address");
                groupView: companyAddressGroup;
            }

            GroupElementView {
                id: companyAddressGroup;

                width: parent.width;

                TextInputElementView {
                    id: countryInput;

                    name: qsTr("Country");
                    placeHolderText: qsTr("Enter the country");

                    onEditingFinished: {
                        accountEditorContainer.doUpdateModel();
                    }

                    KeyNavigation.tab: cityInput;
                    KeyNavigation.backtab: emailInput;

                    Component.onCompleted: {
                        let ok = PermissionsController.checkPermission("ChangeCompanyAddress");

                        countryInput.readOnly = !ok;
                    }
                }

                TextInputElementView {
                    id: cityInput;

                    name: qsTr("City");
                    placeHolderText: qsTr("Enter the city");

                    onEditingFinished: {
                        accountEditorContainer.doUpdateModel();
                    }

                    KeyNavigation.tab: postalCodeInput;
                    KeyNavigation.backtab: countryInput;

                    Component.onCompleted: {
                        let ok = PermissionsController.checkPermission("ChangeCompanyAddress");

                        cityInput.readOnly = !ok;
                    }
                }

                TextInputElementView {
                    id: postalCodeInput;

                    name: qsTr("Postal Code");
                    placeHolderText: qsTr("Enter the postal code");

                    onEditingFinished: {
                        accountEditorContainer.doUpdateModel();
                    }

                    KeyNavigation.tab: streetInput;
                    KeyNavigation.backtab: cityInput;

                    Component.onCompleted: {
                        let ok = PermissionsController.checkPermission("ChangeCompanyAddress");

                        postalCodeInput.readOnly = !ok;
                    }
                }

                TextInputElementView {
                    id: streetInput;

                    name: qsTr("Street");
                    placeHolderText: qsTr("Enter the street");

                    onEditingFinished: {
                        accountEditorContainer.doUpdateModel();
                    }

                    KeyNavigation.tab: groupsElement;
                    KeyNavigation.backtab: postalCodeInput;

                    Component.onCompleted: {
                        let ok = PermissionsController.checkPermission("ChangeCompanyAddress");

                        streetInput.readOnly = !ok;
                    }
                }
            }

            TreeItemModel {
                id: headersModel;

                Component.onCompleted: {
                    bodyColumn.updateHeaders();
                }
            }

            function updateHeaders(){
                headersModel.clear();

                headersModel.insertNewItem();

                headersModel.setData("Id", "Name");
                headersModel.setData("Name", qsTr("Group Name"));

                groupsElement.table.headers = headersModel;
            }

            Text {
                id: titleText;

                color: Style.textColor;
                font.family: Style.fontFamilyBold;
                font.pixelSize: Style.fontSize_title;

                text: qsTr("Group Information");
            }

            TableElementView {
                id: groupsElement;

                width: parent.width;

                name: qsTr("Groups");

                KeyNavigation.tab: customerIdInput;
                KeyNavigation.backtab: streetInput;

                onTableChanged: {
                    if (groupsElement.table){
                        groupsElement.table.checkable = true;
                        groupsElement.table.elements = CachedGroupCollection.collectionModel;

                        let ok = PermissionsController.checkPermission("ChangeAccountGroups");
                        groupsElement.table.readOnly = !ok;

                        tableConn.target = groupsElement.table;
                    }
                }

                Connections {
                    id: tableConn;

                    function onCheckedItemsChanged(){
                        accountEditorContainer.doUpdateModel();
                    }
                }
            }
        }//Body column
    }//Flickable
}// Account Editor container
