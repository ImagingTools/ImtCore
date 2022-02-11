import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

Item {
    id: installationCollectionContainer;
    anchors.fill: parent;
    property Item rootItem;
    property Item multiDocViewItem;
    property alias itemId: installationCollectionView.itemId;
    property alias itemName: installationCollectionView.itemName;
    property alias model: installationCollectionView.collectionViewModel;

    function menuActivated(menuId) {
        installationCollectionView.menuActivated(menuId)
    }

    function refresh() {
        installationCollectionView.refresh();
    }

    function commandsChanged(commandsId) {
        console.log("InstallationCollectionView commandsChanged!", commandsId);
        if (commandsId !== "Installations"){
            return;
        }
        if (installationCollectionView.selectedIndex > -1) {
            installationCollectionContainer.rootItem.setModeMenuButton("Remove", "Normal");
            installationCollectionContainer.rootItem.setModeMenuButton("Edit", "Normal");
            installationCollectionContainer.rootItem.setModeMenuButton("Duplicate", "Normal");
            installationCollectionContainer.rootItem.setModeMenuButton("CreateLicense", "Normal");
        } else {
            installationCollectionContainer.rootItem.setModeMenuButton("Remove", "Disabled");
            installationCollectionContainer.rootItem.setModeMenuButton("Edit", "Disabled");
            installationCollectionContainer.rootItem.setModeMenuButton("Duplicate", "Disabled");
            installationCollectionContainer.rootItem.setModeMenuButton("CreateLicense", "Disabled");
        }
    }

    CollectionView {
        id: installationCollectionView;

        anchors.top: parent.top;
        anchors.left: parent.left;
        anchors.bottom: parent.bottom;
        anchors.right: installCollectionMetaInfo.left;

        Component.onCompleted: {
            installationCollectionView.gqlModelInfo = "InstallationInfo"
            installationCollectionView.gqlModelItems = "InstallationList"
            installationCollectionView.gqlModelItems = "InstallationRemove"
        }

        onSelectItem: {
            console.log("InstallationCollectionView onSelectItem", selectedId, name);
            var typeOperation = "Open";
            if (selectedId === "") {
                name = "New Installation";
                typeOperation = "New";
            }
            installationCollectionContainer.multiDocViewItem.activeCollectionItem = installationCollectionContainer;
            installationCollectionContainer.multiDocViewItem.addToHeadersArray(selectedId, name,  "../../imtlicgui/InstallationInfoEditor.qml", "InstallationEdit", typeOperation)
//            multiDocView.addToHeadersArray(itemId, name,  "../../imtlicgui/InstallationInfoEditor.qml", "InstallationEdit")
        }

        onSelectedIndexChanged: {
            if (installationCollectionView.selectedIndex > -1){
                installationCollectionContainer.commandsChanged("Installations")

                var index = -1;
                for (var i = 0; i < installMetaInfoModels.GetItemsCount(); i++){
                    var curId = installMetaInfoModels.GetData("Id", i);

                    if (curId === installationCollectionView.table.getSelectedId()){
                        index = i;
                        break;
                    }
                }

                if (index !== -1){
                    installCollectionMetaInfo.modelData = installMetaInfoModels.GetData("ModelData", index);
                }
                else{
                    metaInfo.getMetaInfo();
                }
            }
        }
    }

    TreeItemModel {
        id: installMetaInfoModels;
    }

    MetaInfo {
        id: installCollectionMetaInfo;

        anchors.top: parent.top;
        anchors.right: parent.right;

        height: parent.height;
        width: 200;

        color: Style.backgroundColor;
    }

    GqlModel {
        id: metaInfo;

        function getMetaInfo() {
            console.log( "InstallationCollectionView metaInfo getMetaInfo");
            var query = Gql.GqlRequest("query", "InstallationMetaInfo");;
            var inputParams = Gql.GqlObject("input");

            inputParams.InsertField("Id");
            inputParams.InsertFieldArgument("Id", installationCollectionView.table.getSelectedId());

            var queryFields = Gql.GqlObject("metaInfo");
            query.AddParam(inputParams);

            queryFields.InsertField("ModificationTime");
            queryFields.InsertField("InstanceId");
            queryFields.InsertField("Licenses");
            queryFields.InsertField("Checksum");

            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            console.log("InstallationCollectionView metaInfo query ", gqlData);
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

                if (dataModelLocal.ContainsKey("InstallationMetaInfo")) {
                    dataModelLocal = dataModelLocal.GetData("InstallationMetaInfo");

                    if (dataModelLocal.ContainsKey("metaInfo")) {
                        dataModelLocal = dataModelLocal.GetData("metaInfo");

                        installCollectionMetaInfo.modelData = dataModelLocal;

                        var index = installMetaInfoModels.InsertNewItem();

                        installMetaInfoModels.SetData("Id", installationCollectionView.table.getSelectedId(), index);
                        installMetaInfoModels.SetData("ModelData", dataModelLocal, index);
                    }
                }
            }
        }
    }

}
