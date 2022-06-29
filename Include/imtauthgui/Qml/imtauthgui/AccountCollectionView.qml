import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0
import imtlicgui 1.0

Item {
    id: accountCollectionContainer;

    anchors.fill: parent;

    property Item rootItem;
    property Item multiDocViewItem;

    property alias itemId: accountCollectionView.itemId;
    property alias itemName: accountCollectionView.itemName;
    property alias model: accountCollectionView.collectionViewModel;

    property string operation;

    Component.onCompleted: {
//        accountCollectionContainer.forceActiveFocus();
    }

    ListModel {
        id: contextMenuModel;

        Component.onCompleted: {
            contextMenuModel.append({"id": "Edit", "name": qsTr("Edit"), "imageSource": "../../../Icons/Light/Edit_On_Normal.svg", "mode": "Normal"});
            contextMenuModel.append({"id": "Remove", "name": qsTr("Remove"), "imageSource": "../../../Icons/Light/Remove_On_Normal.svg", "mode": "Normal"});
            contextMenuModel.append({"id": "", "name": "", "imageSource": "", "mode": "Normal"});
            contextMenuModel.append({"id": "SetDescription", "name": qsTr("Set Description"), "imageSource": "", "mode": "Normal"});
            contextMenuModel.append({"id": "Rename", "name": qsTr("Rename"), "imageSource": "", "mode": "Normal"});
        }
    }

    function openContextMenu(item, mouseX, mouseY) {
        console.log("AccountCollectionView openContextMenu", mouseX, mouseY);

        if (accountCollectionView.table.height - mouseY <= 100){
            mouseY = mouseY - 100;
        }
        var point = accountCollectionContainer.mapToItem(thubnailDecoratorContainer, mouseX, mouseY);

        var source = "AuxComponents/PopupMenuDialog.qml";
        var parameters = {};
        parameters["model"] = contextMenuModel;
        parameters["resultItem"] = accountCollectionContainer;
        parameters["itemHeight"] = 25;
        parameters["itemWidth"] = 150;

        //parameters["dialogId"] = "PopupMenu";
        parameters["x"] = point.x;
        parameters["y"] = point.y;
        thubnailDecoratorContainer.openDialog(source, parameters);
    }

    function dialogResult(parameters) {
        console.log("AccountCollectionView dialogResult", parameters["dialog"], parameters["status"]);
        if (parameters["dialog"] === "PopupMenu"){

            var source = "AuxComponents/InputDialog.qml";
            var prmtrs= {};

            if (parameters["status"] === "Set Description") {

                prmtrs["message"] = qsTr("Please enter the description of the account: ");
                prmtrs["nameDialog"] = "Set description";
                prmtrs["typeOperation"] = "SetDescription";

                prmtrs["startingValue"] = accountCollectionView.getDescriptionBySelectedItem();

                prmtrs["resultItem"] = accountCollectionContainer;

                thubnailDecoratorContainer.openDialog(source, prmtrs);
            }
            else if (parameters["status"] === "Rename") {

                prmtrs["message"] = qsTr("Please enter the name of the account: ");
                prmtrs["nameDialog"] = "Rename";
                prmtrs["typeOperation"] = "Rename";

                prmtrs["startingValue"] = accountCollectionView.getNameBySelectedItem();
                prmtrs["resultItem"] = accountCollectionContainer;

                thubnailDecoratorContainer.openDialog(source, prmtrs);
            }
            else if (parameters["status"] === "Edit") {

                accountCollectionContainer.menuActivated("Edit");
            }
            else if (parameters["status"] === "Remove") {

                accountCollectionContainer.menuActivated("Remove");
            }
        }
        else if (parameters["dialog"] === "InputDialog"){

            if (parameters["status"] === "yes") {

                if (accountCollectionView.gqlModelRemove !== "") {
                    accountCollectionView.removeSelectedItem();
                }

                accountCollectionContainer.refresh();
                accountCollectionView.table.selectedIndex = -1;
            }

            if (parameters["status"] === "ok") {
                var value = parameters["value"];

                if (parameters["typeOperation"] === "SetDescription") {

                    accountCollectionView.gqlModelSetDescription = "AccountSetDescription";
                    accountCollectionView.callSetDescriptionQuery(value);
                }
                else if (parameters["typeOperation"] === "Rename"){
                    accountCollectionView.gqlModelRename = "AccountRename";
                    accountCollectionView.callRenameQuery(value);
                }

                accountCollectionView.refresh();
            }
        }
    }

    function menuActivated(menuId) {
        console.log("AccountCollectionView menuActivated ", menuId);
        accountCollectionView.menuActivated(menuId)
    }

    function refresh() {
        accountCollectionView.refresh();

//        metaInfo.getMetaInfo();
    }

    function commandsChanged(commandsId) {
        console.log("AccountCollectionView commandsChanged!", commandsId);
        if (commandsId !== "Accounts"){
            return;
        }
        if (accountCollectionView.selectedIndex > -1) {
            accountCollectionContainer.rootItem.setModeMenuButton("Remove", "Normal");
            accountCollectionContainer.rootItem.setModeMenuButton("Edit", "Normal");
        } else {
            accountCollectionContainer.rootItem.setModeMenuButton("Remove", "Disabled");
            accountCollectionContainer.rootItem.setModeMenuButton("Edit", "Disabled");
        }
    }

    function selectedIndexIncr(){
        console.log("accountCollectionView selectedIndexIncr");
        if (accountCollectionView.table.selectedIndex == accountCollectionView.getCountItems() - 1){
            accountCollectionView.table.selectedIndex = 0;
        }
        else
            accountCollectionView.table.selectedIndex++;

        accountCollectionView.table.changeDataByIndex(accountCollectionView.table.selectedIndex);
    }

    function selectedIndexDecr(){
        console.log("accountCollectionView selectedIndexDecr");
        if (accountCollectionView.table.selectedIndex == 0){
            accountCollectionView.table.selectedIndex = accountCollectionView.getCountItems() - 1;
        }
        else
            accountCollectionView.table.selectedIndex--;

        accountCollectionView.table.changeDataByIndex(accountCollectionView.table.selectedIndex);
    }

    function selectItem(){
        console.log("accountCollectionView selectItem");

        var itemId = accountCollectionView.table.getSelectedId();
        var name = accountCollectionView.table.getSelectedName();
        accountCollectionView.itemSelect(itemId, name);
    }

    CollectionView {
        id: accountCollectionView;

        anchors.top: parent.top;
        anchors.left: parent.left;
        anchors.bottom: parent.bottom;
        anchors.right: accountCollectionMetaInfo.left;

        rootItem: accountCollectionContainer;

        autoRefresh: true;

        Component.onCompleted: {
            accountCollectionView.gqlModelInfo = "AccountInfo"
            accountCollectionView.gqlModelItems = "AccountList"
            accountCollectionView.gqlModelRemove = "AccountRemove"
        }
        onItemSelect: {
            var typeOperation = "Open";
            if (idSelect === "") {
                name = "New Account";
                typeOperation = "New";
            }

            var id = name;
            accountCollectionContainer.multiDocViewItem.activeCollectionItem = accountCollectionContainer;
            accountCollectionContainer.multiDocViewItem.addToHeadersArray(id, name,  "../../imtauthgui/ContactInfoEditor.qml", "AccountEdit", typeOperation);
        }

        onCollectionViewRightButtonMouseClicked: {
            console.log("AccountCollectionView CollectionView AuxTable onCollectionViewRightButtonMouseClicked");
            accountCollectionContainer.openContextMenu(item, mouseX, mouseY);
        }

        onSelectedIndexChanged: {
            accountCollectionContainer.commandsChanged("Accounts");
            if (accountCollectionView.selectedIndex > -1){
                var index = -1;
                for (var i = 0; i < accountsMetaInfoModels.GetItemsCount(); i++){
                    var curId = accountsMetaInfoModels.GetData("Id", i);

                    if (curId === accountCollectionView.table.getSelectedId()){
                        index = i;
                        break;
                    }
                }

                if (index !== -1){
                    accountCollectionMetaInfo.modelData = accountsMetaInfoModels.GetData("ModelData", index);
                }
                else{
                    metaInfo.getMetaInfo();
                }
            }
        }

        onRemovedItem: {
            console.log("AccountCollection CollectionView onRemovedItem");
            var index = accountCollectionContainer.multiDocViewItem.getTabIndexById(itemId);
            if (index !== -1){
                accountCollectionContainer.multiDocViewItem.closeTab(index);
            }
            accountCollectionContainer.removeMetaInfoById(itemId);
        }

        onRenamedItem: {
            console.log("AccountCollection CollectionView onRenamedItem");
            var index = accountCollectionContainer.multiDocViewItem.getTabIndexById(oldId);
            if (index !== -1){
                accountCollectionContainer.multiDocViewItem.updateTitleTab(newId, newId, index);
                accountCollectionContainer.multiDocViewItem.reloadModelInOpenTab(index);
            }
        }

        onSetDescriptionItem: {
            var index = accountCollectionContainer.multiDocViewItem.getTabIndexById(id);
            if (index !== -1){
                accountCollectionContainer.multiDocViewItem.reloadModelInOpenTab(index);
            }
        }

        onHeaderClicked: {
            if (headerId == "AccountName"){
                headerId = "name";
            }
            else if (headerId == "Email"){
                headerId = "ownermail";
            }
            else if (headerId == "AccountType"){
                headerId = "type";
            }

            accountCollectionView.setHeaderSort(headerId, sortOrder);
        }
    }

    TreeItemModel {
        id: accountsMetaInfoModels;
    }

    function removeMetaInfoById(objectId){
        for (let i = 0; i < accountsMetaInfoModels.GetItemsCount(); i++){
            let curId = accountsMetaInfoModels.GetData("Id", i);
            if (curId == objectId){
                accountsMetaInfoModels.RemoveItem(i);
                return;
            }
        }
    }

    MetaInfo {
        id: accountCollectionMetaInfo;

        anchors.top: parent.top;
        anchors.right: parent.right;

        height: parent.height;
        width: 200;

        contentVisible: accountCollectionView.table.selectedIndex !== -1;

        color: Style.backgroundColor;
    }

    function callMetaInfoQuery(){
        metaInfo.getMetaInfo();
    }

    GqlModel {
        id: metaInfo;

        function getMetaInfo() {
            console.log( "AccountCollectionView metaInfo getMetaInfo");
            var query = Gql.GqlRequest("query", "AccountMetaInfo");;
            var inputParams = Gql.GqlObject("input");

            inputParams.InsertField("Id");
            inputParams.InsertFieldArgument("Id", accountCollectionView.table.getSelectedId());

            var queryFields = Gql.GqlObject("metaInfo");
            query.AddParam(inputParams);
            queryFields.InsertField("LastName");
            queryFields.InsertField("FirstName");
            queryFields.InsertField("Email");
            queryFields.InsertField("AccountDescription");
            queryFields.InsertField("AccountName");
            queryFields.InsertField("AccountType");
            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            console.log("AccountCollectionView metaInfo query ", gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, metaInfo);
            if (this.state === "Ready"){
                var dataModelLocal;

                if (metaInfo.ContainsKey("errors")){
                    return;
                }

                dataModelLocal = metaInfo.GetData("data");
                if (!dataModelLocal){
                    return;
                }

                if (dataModelLocal.ContainsKey("AccountMetaInfo")) {
                    dataModelLocal = dataModelLocal.GetData("AccountMetaInfo");

                    if (dataModelLocal.ContainsKey("metaInfo")) {
                        dataModelLocal = dataModelLocal.GetData("metaInfo");

                        accountCollectionMetaInfo.modelData = dataModelLocal;

                        var index = -1;
                        for (var i = 0; i < accountsMetaInfoModels.GetItemsCount(); i++){
                            var curId = accountsMetaInfoModels.GetData("Id", i);

                            if (curId === accountCollectionView.table.getSelectedId()){
                                index = i;
                                break;
                            }
                        }

                        if (index == -1){
                            index = accountsMetaInfoModels.InsertNewItem();
                        }

                        accountsMetaInfoModels.SetData("Id", accountCollectionView.table.getSelectedId(), index);
                        accountsMetaInfoModels.SetData("ModelData", dataModelLocal, index);

                    }
                }
            }
        }
    }
}
