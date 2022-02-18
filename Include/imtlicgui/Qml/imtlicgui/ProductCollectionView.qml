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

        } else {
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
        } else {
            productCollectionContainer.rootItem.setModeMenuButton("Remove", "Disabled");
            productCollectionContainer.rootItem.setModeMenuButton("Edit", "Disabled");
            productCollectionContainer.rootItem.setModeMenuButton("Duplicate", "Disabled");
        }
    }

    CollectionView {
        id: productCollectionView;
//        anchors.fill: parent;
        anchors.top: parent.top;
        anchors.left: parent.left;
        anchors.bottom: parent.bottom;
        anchors.right: productCollectionMetaInfo.left;

        autoRefresh: true;

        Component.onCompleted: {
            productCollectionView.gqlModelInfo = "ProductCollectionInfo"
            productCollectionView.gqlModelItems = "ProductList"
            productCollectionView.gqlModelRemove = "ProductRemove"
        }

        onSelectItem: {
            console.log("ProductCollectionView productCollectionView onSelectItem", selectedId, name);
            var typeOperation = "Open";
            if (selectedId === "") {
                name = "New Product";
                typeOperation = "New";
            }

            productCollectionContainer.multiDocViewItem.activeCollectionItem = productCollectionContainer;
            productCollectionContainer.multiDocViewItem.addToHeadersArray(selectedId, name,  "../../imtlicgui/ProductView.qml", "ProductEdit", typeOperation)
        }

        onSelectedIndexChanged: {
            if (productCollectionView.selectedIndex > -1){
                productCollectionContainer.commandsChanged("Products")

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
    }

    TreeItemModel {
        id: productsMetaInfoModels;
    }

    MetaInfo {
        id: productCollectionMetaInfo;

        anchors.top: parent.top;
        anchors.right: parent.right;

        height: parent.height;
        width: 200;

        color: Style.backgroundColor;
    }

    GqlModel {
        id: metaInfo;

        function getMetaInfo() {
            console.log( "ProductCollectionView metaInfo getMetaInfo");
            var query = Gql.GqlRequest("query", "ProductMetaInfo");;
            var inputParams = Gql.GqlObject("input");

            inputParams.InsertField("Id");
            inputParams.InsertFieldArgument("Id", productCollectionView.table.getSelectedId());

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

                if (dataModelLocal.ContainsKey("ProductMetaInfo")) {
                    dataModelLocal = dataModelLocal.GetData("ProductMetaInfo");

                    if (dataModelLocal.ContainsKey("metaInfo")) {
                        dataModelLocal = dataModelLocal.GetData("metaInfo");

                        productCollectionMetaInfo.modelData = dataModelLocal;

                        var index = productsMetaInfoModels.InsertNewItem();

                        productsMetaInfoModels.SetData("Id", productCollectionView.table.getSelectedId(), index);
                        productsMetaInfoModels.SetData("ModelData", dataModelLocal, index);

                    }
                }
            }
        }
    }
}
