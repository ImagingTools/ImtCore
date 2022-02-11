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

    function menuActivated(menuId) {
        console.log("AccountCollectionView menuActivated ", menuId);
        accountCollectionView.menuActivated(menuId)
    }

    function refresh() {
        accountCollectionView.refresh();
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

    CollectionView {
        id: accountCollectionView;

        anchors.top: parent.top;
        anchors.left: parent.left;
        anchors.bottom: parent.bottom;
        anchors.right: accountCollectionMetaInfo.left;

        autoRefresh: true;

        Component.onCompleted: {
            accountCollectionView.gqlModelInfo = "AccountInfo"
            accountCollectionView.gqlModelItems = "AccountList"
            accountCollectionView.gqlModelRemove = "AccountRemove"
        }

        onSelectItem: {
            console.log("Item id = ", selectedId);
            console.log("Name = ", name);
            var typeOperation = "Open";
            if (selectedId === "") {
                name = "New Account";
                typeOperation = "New";
            }
//            name.replace(/[^a-zа-яё^0-9]/gi, ''); удалить все кроме букв и цифр

//            var id = name.replace(/\s/g, '');
            var id = name;
            accountCollectionContainer.multiDocViewItem.activeCollectionItem = accountCollectionContainer;
            accountCollectionContainer.multiDocViewItem.addToHeadersArray(id, name,  "../../imtauthgui/ContactInfoEditor.qml", "AccountEdit", typeOperation);
        }

        onSelectedIndexChanged: {
            if (accountCollectionView.selectedIndex > -1){
                accountCollectionContainer.commandsChanged("Accounts");

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
    }

    TreeItemModel {
        id: accountsMetaInfoModels;
    }

    MetaInfo {
        id: accountCollectionMetaInfo;

        anchors.top: parent.top;
        anchors.right: parent.right;

        height: parent.height;
        width: 200;

        color: Style.backgroundColor;
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
            queryFields.InsertField("Description");
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

                if (dataModelLocal.ContainsKey("AccountMetaInfo")) {
                    dataModelLocal = dataModelLocal.GetData("AccountMetaInfo");

                    if (dataModelLocal.ContainsKey("metaInfo")) {
                        dataModelLocal = dataModelLocal.GetData("metaInfo");

                        accountCollectionMetaInfo.modelData = dataModelLocal;

                        var index = accountsMetaInfoModels.InsertNewItem();

                        accountsMetaInfoModels.SetData("Id", accountCollectionView.table.getSelectedId(), index);
                        accountsMetaInfoModels.SetData("ModelData", dataModelLocal, index);

                    }
                }
            }
        }
    }
//    Rectangle {
//        id: packageMetaInfo;
//        anchors.right: parent.right;
//        height: parent.height;
//        width: 100;
//        color: "green";
//    }
}
