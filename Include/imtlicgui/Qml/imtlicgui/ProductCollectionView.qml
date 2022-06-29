import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

Item {
    id: productCollectionContainer;

    anchors.fill: parent;

    property Item rootItem;
    property Item multiDocViewItem;

    property alias itemId: productCollectionView.itemId;
    property alias itemName: productCollectionView.itemName;
    property alias model: productCollectionView.collectionViewModel;

    property string operation;

    Component.onCompleted: {
        console.log("productCollectionContainer");
       // productCollectionContainer.forceActiveFocus();
        console.log("focus", productCollectionContainer.focus);
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
        if (productCollectionView.table.height - mouseY <= 100){
            mouseY = mouseY - 100;
        }

        var point = productCollectionContainer.mapToItem(thubnailDecoratorContainer, mouseX, mouseY);

        var source = "AuxComponents/PopupMenuDialog.qml";
        var parameters = {};
        parameters["model"] = contextMenuModel;
        parameters["resultItem"] = productCollectionContainer;
        parameters["itemHeight"] = 25;
        parameters["itemWidth"] = 150;
        parameters["x"] = point.x;
        parameters["y"] = point.y;

        thubnailDecoratorContainer.openDialog(source, parameters);
    }

    function dialogResult(parameters) {
        console.log("ProductCollectionView dialogResult", parameters["dialog"], parameters["status"]);
        if (parameters["dialog"] === "PopupMenu"){

            var source = "AuxComponents/InputDialog.qml";
            var prmtrs= {};

            if (parameters["status"] === "Set Description") {

                prmtrs["message"] = qsTr("Please enter the description of the product: ");
                prmtrs["nameDialog"] = "Set description";
                prmtrs["typeOperation"] = "SetDescription";

                prmtrs["startingValue"] = productCollectionView.getDescriptionBySelectedItem();

                prmtrs["resultItem"] = productCollectionContainer;

                thubnailDecoratorContainer.openDialog(source, prmtrs);
            }
            else if (parameters["status"] === "Rename") {

                prmtrs["message"] = qsTr("Please enter the name of the product: ");
                prmtrs["nameDialog"] = "Rename";
                prmtrs["typeOperation"] = "Rename";

                prmtrs["startingValue"] = productCollectionView.getNameBySelectedItem();
                prmtrs["resultItem"] = productCollectionContainer;

                thubnailDecoratorContainer.openDialog(source, prmtrs);
            }
            else if (parameters["status"] === "Edit") {

                productCollectionContainer.menuActivated("Edit");
            }
            else if (parameters["status"] === "Remove") {

                productCollectionContainer.menuActivated("Remove");
            }
        }
        else if (parameters["dialog"] === "InputDialog"){

            if (parameters["status"] === "yes") {

                if (productCollectionView.gqlModelRemove !== "") {
                    productCollectionView.removeSelectedItem();
                }

                productCollectionContainer.refresh();
                productCollectionView.table.selectedIndex = -1;
            }

            if (parameters["status"] === "ok") {
                var value = parameters["value"];

                if (parameters["typeOperation"] === "SetDescription") {

                    productCollectionView.gqlModelSetDescription = "ProductSetDescription";
                    productCollectionView.callSetDescriptionQuery(value);
                }
                else if (parameters["typeOperation"] === "Rename"){
                    productCollectionView.gqlModelRename = "ProductRename";
                    productCollectionView.callRenameQuery(value);
                }

                productCollectionView.refresh();
            }
        }
    }

    function refresh() {
        productCollectionView.refresh();
    }

    function menuActivated(menuId) {
        console.log("ProductCollectionView menuActivated", menuId);
        if (menuId === "Duplicate") {
            var dataModelLocal = productCollectionView.collectionViewModel.GetData("data");
            var currentName = dataModelLocal.GetData("Name", productCollectionView.selectedIndex);
            var currentId = dataModelLocal.GetData("Id", productCollectionView.selectedIndex);
            var name = "Copy of " + currentName;

            productCollectionContainer.multiDocViewItem.addToHeadersArray(currentId, name,  "../../imtlicgui/ProductView.qml", "ProductEdit", "Copy")

        }
        else {
            productCollectionView.menuActivated(menuId)
        }
    }

    function commandsChanged(commandsId) {
        console.log("ProductCollectionView commandsChanged!", commandsId);
        if (commandsId !== "Products"){
            return;
        }
        if (productCollectionView.selectedIndex > -1) {
            productCollectionContainer.rootItem.setModeMenuButton("Remove", "Normal");
            productCollectionContainer.rootItem.setModeMenuButton("Edit", "Normal");
            productCollectionContainer.rootItem.setModeMenuButton("Duplicate", "Normal");
        }
        else {
            productCollectionContainer.rootItem.setModeMenuButton("Remove", "Disabled");
            productCollectionContainer.rootItem.setModeMenuButton("Edit", "Disabled");
            productCollectionContainer.rootItem.setModeMenuButton("Duplicate", "Disabled");
        }
    }

    function selectedIndexIncr(){
        console.log("ProductCollectionView selectedIndexIncr");
        if (productCollectionView.table.selectedIndex == productCollectionView.getCountItems() - 1){
            productCollectionView.table.selectedIndex = 0;
        }
        else
            productCollectionView.table.selectedIndex++;

        productCollectionView.table.changeDataByIndex(productCollectionView.table.selectedIndex);
    }

    function selectedIndexDecr(){
        console.log("ProductCollectionView selectedIndexDecr");
        if (productCollectionView.table.selectedIndex == 0){
            productCollectionView.table.selectedIndex = productCollectionView.getCountItems() - 1;
        }
        else
            productCollectionView.table.selectedIndex--;

        productCollectionView.table.changeDataByIndex(productCollectionView.table.selectedIndex);
    }

    function selectItem(){
        console.log("ProductCollectionView selectItem");

        var itemId = productCollectionView.table.getSelectedId();
        var name = productCollectionView.table.getSelectedName();
        productCollectionView.itemSelect(itemId, name);
    }

    CollectionView {
        id: productCollectionView;

        anchors.top: parent.top;
        anchors.left: parent.left;
        anchors.bottom: parent.bottom;
        anchors.right: productCollectionMetaInfo.left;

        autoRefresh: true;

        rootItem: productCollectionContainer;

        Component.onCompleted: {
            productCollectionView.gqlModelInfo = "ProductCollectionInfo"
            productCollectionView.gqlModelItems = "ProductList"
            productCollectionView.gqlModelRemove = "ProductRemove"
        }

        onItemSelect: {
            console.log("ProductCollectionView productCollectionView onSelectItem", idSelect, name);
            var typeOperation = "Open";
            if (idSelect === "") {
                name = "New Product";
                typeOperation = "New";
            }

            productCollectionContainer.multiDocViewItem.activeCollectionItem = productCollectionContainer;
            productCollectionContainer.multiDocViewItem.addToHeadersArray(idSelect, name,  "../../imtlicgui/ProductView.qml", "ProductEdit", typeOperation)
        }

        onCollectionViewRightButtonMouseClicked: {
            console.log("ProductCollectionView CollectionView AuxTable onCollectionViewRightButtonMouseClicked");
            productCollectionContainer.openContextMenu(item, mouseX, mouseY);
        }

        onSelectedIndexChanged: {
            productCollectionContainer.commandsChanged("Products")
            if (productCollectionView.selectedIndex > -1){
                var index = -1;
                for (var i = 0; i < productsMetaInfoModels.GetItemsCount(); i++){
                    var curId = productsMetaInfoModels.GetData("Id", i);

                    if (curId === productCollectionView.table.getSelectedId()){
                        index = i;
                        break;
                    }
                }

                if (index !== -1){
                    productCollectionMetaInfo.modelData = productsMetaInfoModels.GetData("ModelData", index);
                }
                else{
                    metaInfo.getMetaInfo();
                }
            }
        }

        onRemovedItem: {
            console.log("ProductCollection CollectionView onRemovedItem");
            var index = productCollectionContainer.multiDocViewItem.getTabIndexById(itemId);
            if (index !== -1){
                productCollectionContainer.multiDocViewItem.closeTab(index);
            }
            productCollectionContainer.removeMetaInfoById(itemId);
        }

        onRenamedItem: {
            console.log("ProductCollection CollectionView onRenamedItem");
            var index = productCollectionContainer.multiDocViewItem.getTabIndexById(oldId);
            if (index !== -1){
                productCollectionContainer.multiDocViewItem.updateTitleTab(newId, newId, index);
            }
            metaInfo.getMetaInfo();
        }

        onSetDescriptionItem: {
            metaInfo.getMetaInfo();
        }

        onHeaderClicked: {
            if (headerId == "ModificationTime"){
                headerId = "lastmodified";
            }

            productCollectionView.setHeaderSort(headerId, sortOrder);
        }
    }

    TreeItemModel {
        id: productsMetaInfoModels;
    }

    function removeMetaInfoById(objectId){
        for (let i = 0; i < productsMetaInfoModels.GetItemsCount(); i++){
            let curId = productsMetaInfoModels.GetData("Id", i);
            if (curId == objectId){
                productsMetaInfoModels.RemoveItem(i);
                return;
            }
        }
    }

    MetaInfo {
        id: productCollectionMetaInfo;

        anchors.top: parent.top;
        anchors.right: parent.right;

        height: parent.height;
        width: 200;

        contentVisible: productCollectionView.table.selectedIndex !== -1;
        color: Style.backgroundColor;
    }

    function callMetaInfoQuery(){
        metaInfo.getMetaInfo();
    }

    GqlModel {
        id: metaInfo;

        function getMetaInfo() {
            console.log( "ProductCollectionView metaInfo getMetaInfo");
            var query = Gql.GqlRequest("query", "ProductMetaInfo");;
            var inputParams = Gql.GqlObject("input");

            inputParams.InsertField("Id", productCollectionView.table.getSelectedId());

            var queryFields = Gql.GqlObject("metaInfo");
            query.AddParam(inputParams);
            queryFields.InsertField("ModificationTime");
            queryFields.InsertField("Checksum");
            queryFields.InsertField("Licenses");
            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            console.log("ProductCollectionView metaInfo query ", gqlData);
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

                if (dataModelLocal.ContainsKey("ProductMetaInfo")) {
                    dataModelLocal = dataModelLocal.GetData("ProductMetaInfo");

                    if (dataModelLocal.ContainsKey("metaInfo")) {
                        dataModelLocal = dataModelLocal.GetData("metaInfo");

                        productCollectionMetaInfo.modelData = dataModelLocal;

                        var index = -1;
                        for (var i = 0; i < productsMetaInfoModels.GetItemsCount(); i++){

                            if (productsMetaInfoModels.GetData("Id", i) === productCollectionView.table.getSelectedId()){
                                index = i;
                                break;
                            }
                        }

                        if (index === -1){
                            index = productsMetaInfoModels.InsertNewItem();
                        }

//                        var index = productsMetaInfoModels.InsertNewItem();

                        productsMetaInfoModels.SetData("Id", productCollectionView.table.getSelectedId(), index);
                        productsMetaInfoModels.SetData("ModelData", dataModelLocal, index);
                    }
                }
            }
        }
    }
}
