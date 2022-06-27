import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0
import QtQuick.Dialogs 1.3

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
        if (installationCollectionView.table.height - mouseY <= 100){
            mouseY = mouseY - 100;
        }
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
        if (menuId === "Duplicate"){
            var dataModelLocal = installationCollectionView.collectionViewModel.GetData("data");
            var currentName = dataModelLocal.GetData("Name", installationCollectionView.selectedIndex);
            var currentId = dataModelLocal.GetData("Id", installationCollectionView.selectedIndex);
            var name = "Copy of " + currentName;

            installationCollectionContainer.multiDocViewItem.addToHeadersArray(currentId, name,  "../../imtlicgui/InstallationInfoEditor.qml", "InstallationEdit", "Copy")

        }
        else if (menuId === "CreateLicense"){
            fileDialogSave.open();
        }
        else{
            installationCollectionView.menuActivated(menuId)
        }
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
                prmtrs["nameDialog"] = "Rename";
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

    function selectedIndexIncr(){
        console.log("PackageCollectionView selectedIndexIncr");
        if (installationCollectionView.table.selectedIndex == installationCollectionView.getCountItems() - 1){
            installationCollectionView.table.selectedIndex = 0;
        }
        else
            installationCollectionView.table.selectedIndex++;

        installationCollectionView.table.changeDataByIndex(installationCollectionView.table.selectedIndex);
    }

    function selectedIndexDecr(){
        console.log("installationCollectionView selectedIndexDecr");
        if (installationCollectionView.table.selectedIndex == 0){
            installationCollectionView.table.selectedIndex = installationCollectionView.getCountItems() - 1;
        }
        else
            installationCollectionView.table.selectedIndex--;

        installationCollectionView.table.changeDataByIndex(installationCollectionView.table.selectedIndex);
    }

    function selectItem(){
        console.log("installationCollectionView selectItem");

        var itemId = installationCollectionView.table.getSelectedId();
        var name = installationCollectionView.table.getSelectedName();
        installationCollectionView.itemSelect(itemId, name);
    }

    CollectionView {
        id: installationCollectionView;

        anchors.top: parent.top;
        anchors.left: parent.left;
        anchors.bottom: parent.bottom;
        anchors.right: installCollectionMetaInfo.left;

        rootItem: installationCollectionContainer;

        autoRefresh: true;

        Component.onCompleted: {
            installationCollectionView.gqlModelInfo = "InstallationInfo"
            installationCollectionView.gqlModelItems = "InstallationList"
            installationCollectionView.gqlModelRemove = "InstallationRemove"
        }

        onItemSelect: {
            console.log("InstallationCollectionView onSelectItem", idSelect, name);
            var typeOperation = "Open";
            if (idSelect === "") {
                name = "New Installation";
                typeOperation = "New";
            }
            installationCollectionContainer.multiDocViewItem.activeCollectionItem = installationCollectionContainer;
            installationCollectionContainer.multiDocViewItem.addToHeadersArray(idSelect, name,  "../../imtlicgui/InstallationInfoEditor.qml", "InstallationEdit", typeOperation)

        }

        onCollectionViewRightButtonMouseClicked: {
            console.log("InstallationCollectionView CollectionView AuxTable onCollectionViewRightButtonMouseClicked");
            installationCollectionContainer.openContextMenu(item, mouseX, mouseY);
        }

        onSelectedIndexChanged: {
            installationCollectionContainer.commandsChanged("Installations")
            if (installationCollectionView.selectedIndex > -1){
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

        onRemovedItem: {
            console.log("InstallationCollection CollectionView onRemovedItem");
            var index = installationCollectionContainer.multiDocViewItem.getTabIndexById(itemId);
            if (index !== -1){
                installationCollectionContainer.multiDocViewItem.closeTab(index);
            }
            installationCollectionContainer.removeMetaInfoById(itemId);
        }

        onRenamedItem: {
            console.log("InstallationCollection CollectionView onRenamedItem");
            var index = installationCollectionContainer.multiDocViewItem.getTabIndexById(oldId);
            if (index !== -1){
                installationCollectionContainer.multiDocViewItem.updateTitleTab(newId, newId, index);
            }
            metaInfo.getMetaInfo();
        }

        onSetDescriptionItem: {
            metaInfo.getMetaInfo();
        }

        onHeaderClicked: {
            if (headerId == "Id"){
                headerId = "instanceid";
            }
            else if (headerId == "Customer"){
                headerId = "accountid";
            }

            installationCollectionView.setHeaderSort(headerId, sortOrder);
        }
    }

    TreeItemModel {
        id: installMetaInfoModels;
    }

    function removeMetaInfoById(objectId){
        for (let i = 0; i < installMetaInfoModels.GetItemsCount(); i++){
            let curId = installMetaInfoModels.GetData("Id", i);
            if (curId == objectId){
                installMetaInfoModels.RemoveItem(i);
                return;
            }
        }
    }

    MetaInfo {
        id: installCollectionMetaInfo;

        anchors.top: parent.top;
        anchors.right: parent.right;

        height: parent.height;
        width: 200;

        contentVisible: installationCollectionView.table.selectedIndex !== -1;

        color: Style.backgroundColor;
    }

    RemoteFileController {
        id: remoteFileController;

        onFileDownloaded: {
            console.log('onFileDownloaded', filePath);
           // console.log('onopening file', remoteFileController.OpenFile(filePath));
        }
    }

    FileDialog {
        id: fileDialogSave;

        title: qsTr("Save file");
        selectExisting: false;
        folder: shortcuts.home;

        nameFilters: ["License files (*.lic)", "All files (*)"];

        onAccepted: {
            console.log("You chose: " + fileDialogSave.fileUrl);
            console.log("You chose folder: " + fileDialogSave.folder);

            var pathDir = fileDialogSave.folder.toString();
            remoteFileController.downloadedFileLocation = pathDir.replace('file:///', '');
            var fileName = fileDialogSave.fileUrl.toString().replace(pathDir + "/", '');

            console.log("You chose file name: ", fileName);
            var id = installationCollectionView.table.getSelectedId();

            if (fileName == ""){
                fileName = {};
                fileName["name"] = id + ".lic";
            }

            console.log("id ",id);
            console.log("fileName ",fileName);
            remoteFileController.GetFile(id, fileName);
        }
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

                if (metaInfo.ContainsKey("data")){
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


    GqlModel {
        id: licenseFile;

        function createLicenseFile() {
            console.log( "InstallationCollectionView licenseFile createLicenseFile");
            var query = Gql.GqlRequest("query", "CreateLicenseFile");;
            var inputParams = Gql.GqlObject("input");

            inputParams.InsertField("Id");
            inputParams.InsertFieldArgument("Id", installationCollectionView.table.getSelectedId());
            query.AddParam(inputParams);

            var queryFields = Gql.GqlObject("createLicenseFile");
            queryFields.InsertField("Status");
            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            console.log("InstallationCollectionView licenseFile query ", gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, licenseFile);
            if (this.state === "Ready"){
                var dataModelLocal;

                if (licenseFile.ContainsKey("errors")){
                    return;
                }

                dataModelLocal = licenseFile.GetData("data");

//                if (dataModelLocal.ContainsKey("InstallationMetaInfo")) {

//                }

            }
        }
    }

}
