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

    property string operation;

    ListModel {
        id: contextMenuModel;

        Component.onCompleted: {
            contextMenuModel.append({"id": "Edit", "name": "Edit", "imageSource": "../../../Icons/Light/Edit_On_Normal.svg", "mode": "Normal"});
            contextMenuModel.append({"id": "Remove", "name": "Remove", "imageSource": "../../../Icons/Light/Remove_On_Normal.svg", "mode": "Normal"});
            contextMenuModel.append({"id": "", "name": "", "imageSource": "", "mode": "Normal"});
            contextMenuModel.append({"id": "SetDescription", "name": "Set Description", "imageSource": "", "mode": "Normal"});
            contextMenuModel.append({"id": "Rename", "name": "Rename", "imageSource": "", "mode": "Normal"});
        }
    }

    function openContextMenu(item, mouseX, mouseY) {
        console.log("InstallationCollectionView openContextMenu", mouseX, mouseY);

        var point = installationCollectionContainer.mapToItem(thubnailDecoratorContainer, mouseX, mouseY);


        var source = "AuxComponents/PopupMenuDialog.qml";
        var parameters = {};
        parameters["model"] = contextMenuModel;
        parameters["resultItem"] = installationCollectionContainer;
        parameters["itemHeight"] = 25;
        parameters["itemWidth"] = 150;
        parameters["x"] = point.x;
        parameters["y"] = point.y;

        thubnailDecoratorContainer.openDialog(source, parameters);
    }

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

    function dialogResult(parameters) {
        console.log("InstallationCollectionView dialogResult", parameters["dialog"], parameters["status"]);
        if (parameters["dialog"] === "PopupMenu"){

            var source = "AuxComponents/InputDialog.qml";
            var prmtrs= {};

            if (parameters["status"] === "Set Description") {

                prmtrs["message"] = "Please enter the description of the installation: ";
                prmtrs["nameDialog"] = "Set description";
                prmtrs["typeOperation"] = "SetDescription";

                prmtrs["startingValue"] = installationCollectionView.getDescriptionBySelectedItem();

                prmtrs["resultItem"] = installationCollectionContainer;

                thubnailDecoratorContainer.openDialog(source, prmtrs);
            }
            else if (parameters["status"] === "Rename") {

                prmtrs["message"] = "Please enter the name of the product: ";
                prmtrs["nameDialog"] = "Rename Dialog";
                prmtrs["typeOperation"] = "Rename";

                prmtrs["startingValue"] = installationCollectionView.getNameBySelectedItem();
                prmtrs["resultItem"] = installationCollectionContainer;

                thubnailDecoratorContainer.openDialog(source, prmtrs);
            }
            else if (parameters["status"] === "Edit") {
                installationCollectionContainer.menuActivated("Edit");
            }
            else if (parameters["status"] === "Remove") {
                installationCollectionContainer.menuActivated("Remove");
            }
        }
        else if (parameters["dialog"] === "InputDialog"){

            if (parameters["status"] === "yes") {

                if (installationCollectionView.gqlModelRemove !== "") {
                    installationCollectionView.removeSelectedItem();
                }

                installationCollectionContainer.refresh();
                installationCollectionView.table.selectedIndex = -1;
            }

            if (parameters["status"] === "ok") {
                var value = parameters["value"];

                if (parameters["typeOperation"] === "SetDescription") {

                    installationCollectionView.gqlModelSetDescription = "InstallationSetDescription";
                    installationCollectionView.callSetDescriptionQuery(value);
                }
                else if (parameters["typeOperation"] === "Rename"){
                    installationCollectionView.gqlModelRename = "InstallationRename";
                    installationCollectionView.callRenameQuery(value);
                }

                installationCollectionView.refresh();
            }
        }
    }

    CollectionView {
        id: installationCollectionView;

        anchors.top: parent.top;
        anchors.left: parent.left;
        anchors.bottom: parent.bottom;
        anchors.right: installCollectionMetaInfo.left;

        autoRefresh: true;

        Component.onCompleted: {
            installationCollectionView.gqlModelInfo = "InstallationInfo"
            installationCollectionView.gqlModelItems = "InstallationList"
            installationCollectionView.gqlModelRemove = "InstallationRemove"
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
        }

        onCollectionViewRightButtonMouseClicked: {
            console.log("InstallationCollectionView CollectionView AuxTable onCollectionViewRightButtonMouseClicked");
            installationCollectionContainer.openContextMenu(item, mouseX, mouseY);
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

    function callMetaInfoQuery(){
        metaInfo.getMetaInfo();
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

                        var index = -1;
                        for (var i = 0; i < installMetaInfoModels.GetItemsCount(); i++){

                            if (installMetaInfoModels.GetData("Id", i) === installationCollectionView.table.getSelectedId()){
                                index = i;
                                break;
                            }
                        }

                        if (index === -1){
                            index = installMetaInfoModels.InsertNewItem();
                        }

                        installMetaInfoModels.SetData("Id", installationCollectionView.table.getSelectedId(), index);
                        installMetaInfoModels.SetData("ModelData", dataModelLocal, index);
                    }
                }
            }
        }
    }

}
