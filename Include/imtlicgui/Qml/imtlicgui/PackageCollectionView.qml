import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

Item {
    id: packageCollectionContainer;

    anchors.fill: parent;

    property Item rootItem;
    property Item multiDocViewItem;

    property alias itemId: packageCollectionView.itemId;
    property alias itemName: packageCollectionView.itemName;
    property alias model: packageCollectionView.collectionViewModel;

    property string operation;

    function refresh(){
        packageCollectionView.refresh();
    }

    function menuActivated(menuId){
        packageCollectionView.menuActivated(menuId)
    }

    Component.onCompleted: {
//        packageCollectionContainer.forceActiveFocus();
    }

//    Keys.onPressed: {
//        console.log("PackageCollectionView keys pressed")

//        if (event.key === Qt.Key_Tab){
//            console.log('Key tab was pressed');

//            if (packageCollectionContainer.multiDocViewItem.tabPanel.count > 1){
//                packageCollectionContainer.multiDocViewItem.tabPanel.rightClicked();
//                packageCollectionContainer.multiDocViewItem.activeItem.forceActiveFocus();
//            }
//            else{
//                thubnailDecoratorContainer.setFocusOnMenuPanel();
//            }
//        }
//        else if (event.key === Qt.Key_Up){
//            console.log('Key up was pressed');
//            packageCollectionContainer.selectedIndexDecr();
//        }
//        else if (event.key === Qt.Key_Down){
//            console.log('Key down was pressed');
//             packageCollectionContainer.selectedIndexIncr();
//        }
//        else if (event.key === Qt.Key_Return){
//            console.log('Key down was pressed');
//            packageCollectionContainer.selectItem();

//            packageCollectionContainer.multiDocViewItem.activeItem.forceActiveFocus();
//        }
//    }

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

    TreeItemModel {
        id: metaInfoModels;
    }

    function removeMetaInfoById(objectId){
        for (let i = 0; i < metaInfoModels.GetItemsCount(); i++){
            let curId = metaInfoModels.GetData("Id", i);
            if (curId == objectId){
                metaInfoModels.RemoveItem(i);
                return;
            }
        }
    }

    function commandsChanged(commandsId) {
        console.log("PackageCollectionView commandsChanged!", commandsId, packageCollectionContainer.rootItem);
        if (commandsId !== "Packages"){
            return;
        }
        console.log("packageCollectionView.selectedIndex", packageCollectionView.selectedIndex);
        if (packageCollectionView.selectedIndex > -1) {

            packageCollectionContainer.rootItem.setModeMenuButton("Remove", "Normal");
            packageCollectionContainer.rootItem.setModeMenuButton("Edit", "Normal");
        } else {
            packageCollectionContainer.rootItem.setModeMenuButton("Remove", "Disabled");
            packageCollectionContainer.rootItem.setModeMenuButton("Edit", "Disabled");
        }

        packageCollectionContainer.rootItem.setModeMenuButton("New", "Normal");
        packageCollectionContainer.rootItem.setModeMenuButton("Undo", "Disabled");
        packageCollectionContainer.rootItem.setModeMenuButton("Redo", "Disabled");
    }

    function getDescriptionBySelectedItem(){
        var dataModelLocal = packageCollectionView.collectionViewModel.GetData("data");
        var description = dataModelLocal.GetData("Description", packageCollectionView.table.selectedIndex);

        return description;
    }

    function getNameBySelectedItem(){
        var dataModelLocal = packageCollectionView.collectionViewModel.GetData("data");
        var name = dataModelLocal.GetData("Name", packageCollectionView.table.selectedIndex);

        return name;
    }

    function updatePackageAfterRename(newId, newName) {
        console.log("PackageCollectionView updatePackageAfterRename", newId, newName);
        var dataModelLocal = packageCollectionView.collectionViewModel.GetData("data");
        dataModelLocal.SetData("Id", newId, packageCollectionView.table.selectedIndex);
        dataModelLocal.SetData("Name", newName, packageCollectionView.table.selectedIndex);
        packageCollectionView.collectionViewModel.SetData("data", dataModelLocal);
    }

    function updatePackageAfterSetDescription(description) {
        console.log("PackageCollectionView updatePackageAfterSetDescription", newId, newName);
        var dataModelLocal = packageCollectionView.collectionViewModel.GetData("data");
        dataModelLocal.SetData("Description", description, packageCollectionView.table.selectedIndex);
        packageCollectionView.collectionViewModel.SetData("data", dataModelLocal);
    }

    function dialogResult(parameters){
        console.log("PackageCollectionView dialogResult", parameters["status"]);
        packageCollectionContainer.forceActiveFocus();
        if (parameters["dialog"] === "PopupMenu"){

            var source = "AuxComponents/InputDialog.qml";
            var prmtrs= {};

            if (parameters["status"] === "Set Description"){

                prmtrs["message"] = qsTr("Please enter the description of the package: ");
                prmtrs["nameDialog"] = "Set description";
                prmtrs["typeOperation"] = "SetDescription";

                prmtrs["startingValue"] = packageCollectionView.getDescriptionBySelectedItem();

                prmtrs["resultItem"] = packageCollectionContainer;

                thubnailDecoratorContainer.openDialog(source, prmtrs);
            }
            else if (parameters["status"] === "Rename"){
                prmtrs["message"] = qsTr("Please enter the name of the package: ");
                prmtrs["nameDialog"] = "Rename";
                prmtrs["typeOperation"] = "Rename";

                prmtrs["startingValue"] = packageCollectionView.getNameBySelectedItem();
                prmtrs["resultItem"] = packageCollectionContainer;

                thubnailDecoratorContainer.openDialog(source, prmtrs);
            }
            else if (parameters["status"] === "Edit"){
//                packageCollectionContainer.menuActivated("Edit");

                var itemId = packageCollectionView.table.getSelectedId();
                var name = packageCollectionView.table.getSelectedName();
                packageCollectionView.itemSelect(itemId, name);
            }
            else if (parameters["status"] === "Remove"){
                packageCollectionContainer.menuActivated("Remove");
            }
        }
        else if (parameters["dialog"] === "InputDialog"){

            if (parameters["status"] === "yes"){

                if (packageCollectionView.gqlModelRemove !== ""){
                    packageCollectionView.removeSelectedItem();
                }
                packageCollectionContainer.refresh();
                packageCollectionView.table.selectedIndex = -1;
            }

            if (parameters["status"] === "ok"){
                var value = parameters["value"];

                if (parameters["typeOperation"] === "SetDescription"){

                    packageCollectionView.gqlModelSetDescription = "FeaturePackageSetDescription";
                    packageCollectionView.callSetDescriptionQuery(value);
                }
                else if (parameters["typeOperation"] === "Rename"){
                    packageCollectionView.gqlModelRename = "FeaturePackageRename";
                    packageCollectionView.callRenameQuery(value);
                }

                packageCollectionView.refresh();
            }
        }
    }

    function openContextMenu(item, mouseX, mouseY) {
        console.log("PackageCollectionView openContextMenu", mouseX, mouseY);
        if (packageCollectionView.table.height - mouseY <= 100){
            mouseY = mouseY - 100;
        }
        var point = packageCollectionContainer.mapToItem(thubnailDecoratorContainer, mouseX, mouseY);
        var source = "AuxComponents/PopupMenuDialog.qml";
        var parameters = {};

        parameters["model"] = contextMenuModel;
        parameters["resultItem"] = packageCollectionContainer;

        parameters["itemHeight"] = 25;
        parameters["itemWidth"] = 150;

        parameters["x"] = point.x;
        parameters["y"] = point.y;

        thubnailDecoratorContainer.openDialog(source, parameters);
    }

    function selectedIndexIncr(){
        console.log("PackageCollectionView selectedIndexIncr");
        if (packageCollectionView.table.selectedIndex == packageCollectionView.getCountItems() - 1){
            packageCollectionView.table.selectedIndex = 0;
        }
        else
            packageCollectionView.table.selectedIndex++;

        packageCollectionView.table.changeDataByIndex(packageCollectionView.table.selectedIndex);
    }

    function selectedIndexDecr(){
        console.log("PackageCollectionView selectedIndexDecr");
        if (packageCollectionView.table.selectedIndex == 0){
            packageCollectionView.table.selectedIndex = packageCollectionView.getCountItems() - 1;
        }
        else
            packageCollectionView.table.selectedIndex--;

        packageCollectionView.table.changeDataByIndex(packageCollectionView.table.selectedIndex);
    }

    function selectItem(){
        console.log("PackageCollectionView selectItem");

        var itemId = packageCollectionView.table.getSelectedId();
        var name = packageCollectionView.table.getSelectedName();
        packageCollectionView.itemSelect(itemId, name);
    }

    UndoRedo {
        id: undoRedo;
    }

    CollectionView {
        id: packageCollectionView;

        anchors.top: parent.top;
        anchors.left: parent.left;
        anchors.bottom: parent.bottom;
        anchors.right: packageCollectionMetaInfo.left;

        rootItem: packageCollectionContainer;

        autoRefresh: true;

        Component.onCompleted: {
            packageCollectionView.gqlModelInfo = "FeaturePackageInfo"
            packageCollectionView.gqlModelItems = "FeaturePackageList";
            packageCollectionView.gqlModelRemove = "FeaturePackageRemove";
            console.log("packageCollectionView onCompleted", packageCollectionView.gqlModelInfo)
        }

        onItemSelect: {
            var typeOperation = "Open";

            if (idSelect === ""){
                name = "New Package";
                typeOperation = "New";
            }

            packageCollectionContainer.multiDocViewItem.activeCollectionItem = packageCollectionContainer;
            packageCollectionContainer.multiDocViewItem.addToHeadersArray(idSelect, name,  "../../imtlicgui/PackageView.qml", "PackageEdit", typeOperation)
        }

        onCollectionViewRightButtonMouseClicked: {
            console.log("PackageCollectionView CollectionView AuxTable onCollectionViewRightButtonMouseClicked");
            packageCollectionContainer.openContextMenu(item, mouseX, mouseY);
        }

        onSelectedIndexChanged: {
            console.log("packageCollectionView onSelectedIndexChanged", packageCollectionView.table.selectedIndex);
            packageCollectionContainer.commandsChanged("Packages");
            if (packageCollectionView.table.selectedIndex > -1){
//                if (metaInfoModels.GetItemsCount() >= packageCollectionView.table.selectedIndex + 1){
//                    packageCollectionMetaInfo.modelData = metaInfoModels.GetData("ModelData",
//                                                                                 packageCollectionView.table.selectedIndex);
//                }
//                else{
//                    metaInfo.getMetaInfo();
//                }


                var index = -1;
                for (var i = 0; i < metaInfoModels.GetItemsCount(); i++){
                    var curId = metaInfoModels.GetData("Id", i);

                    if (curId === packageCollectionView.table.getSelectedId()){
                        index = i;
                        break;
                    }
                }

                if (index !== -1){
                    packageCollectionMetaInfo.modelData = metaInfoModels.GetData("ModelData", index);
                }
                else{
                    metaInfo.getMetaInfo();
                }
            }
        }

        onSetActiveFocusFromCollectionView: {
            console.log("PackageCollection CollectionView onSetActiveFocusFromCollectionView");
        }

        onRemovedItem: {
            console.log("PackageCollection CollectionView onRemovedItem");

            var index = packageCollectionContainer.multiDocViewItem.getTabIndexById(itemId);

            if (index !== -1){
                packageCollectionContainer.multiDocViewItem.closeTab(index);
            }
            packageCollectionContainer.removeMetaInfoById(itemId);
        }

        onRenamedItem: {
            console.log("PackageCollection CollectionView onRenamedItem");

            var index = packageCollectionContainer.multiDocViewItem.getTabIndexById(oldId);
            if (index !== -1){
                packageCollectionContainer.multiDocViewItem.updateTitleTab(newId, newId, index);
            }

//            featuresTreeView.updateFeaturesDependenciesAfterPackageEditing(oldId, newId);
            featureDependenciesModel.updateFeaturesDependenciesAfterPackageEditing(oldId, newId);
            metaInfo.getMetaInfo();
        }

        onSetDescriptionItem: {
            metaInfo.getMetaInfo();
        }

        onHeaderClicked: {
            if (headerId == "ModificationTime"){
                headerId = "lastmodified";
            }

            packageCollectionView.setHeaderSort(headerId, sortOrder);
        }
    }

    MetaInfo {
        id: packageCollectionMetaInfo;

        anchors.top: parent.top;
        anchors.right: parent.right;

        height: parent.height;
        width: 200;

        contentVisible: packageCollectionView.table.selectedIndex !== -1;

        color: Style.backgroundColor;
    }

    function callMetaInfoQuery(){
        metaInfo.getMetaInfo();
    }

    GqlModel {
        id: metaInfo;

        function getMetaInfo(){
            console.log( "PackageCollectionView metaInfo getMetaInfo");
            var query = Gql.GqlRequest("query", "FeaturePackageMetaInfo");;
            var inputParams = Gql.GqlObject("input");

            inputParams.InsertField("Id", packageCollectionView.table.getSelectedId());

            var queryFields = Gql.GqlObject("metaInfo");
            query.AddParam(inputParams);
            queryFields.InsertField("ModificationTime");
            queryFields.InsertField("Checksum");
            queryFields.InsertField("Features");
            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            console.log("PackageCollectionView metaInfo query ", gqlData);
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

                if (dataModelLocal && dataModelLocal.ContainsKey("FeaturePackageMetaInfo")){
                    dataModelLocal = dataModelLocal.GetData("FeaturePackageMetaInfo");

                    if (dataModelLocal.ContainsKey("metaInfo")){
                        dataModelLocal = dataModelLocal.GetData("metaInfo");

                        packageCollectionMetaInfo.modelData = dataModelLocal;

                        var index = -1;
                        for (var i = 0; i < metaInfoModels.GetItemsCount(); i++){

                            if (metaInfoModels.GetData("Id", i) === packageCollectionView.table.getSelectedId()){
                                index = i;
                                break;
                            }
                        }

                        if (index === -1){
                            index = metaInfoModels.InsertNewItem();
                        }

                        metaInfoModels.SetData("Id", packageCollectionView.table.getSelectedId(), index);
                        metaInfoModels.SetData("ModelData", dataModelLocal, index);
                    }
                }
            }
        }
    }
}
