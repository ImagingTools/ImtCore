import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0
//import imtlicgui 1.0

Item {
    id: featureCollectionViewContainer;

    anchors.fill: parent;

    property Item rootItem;
    property Item multiDocViewItem;

    property alias itemId: featureCollectionView.itemId;
    property alias itemName: featureCollectionView.itemName;
    property alias model: featureCollectionView.collectionViewModel;

    property string gqlModelQueryType;
    property string gqlModelQueryTypeNotification;
    property string operation;

    property bool wasChanged: false;

    onWidthChanged: {
        console.log("PackageView onWidthChanged", featureCollectionViewContainer.width);
       if (featureCollectionViewContainer.width > 0 && featureCollectionViewContainer.width - packageMetaInfo.width > 250){
           packageSplitter.x = featureCollectionViewContainer.width - packageMetaInfo.width;
       }
    }

    onItemIdChanged: {
        console.log("PackageView onItemIdChanged", featureCollectionViewContainer.itemId);
    }

    Component.onCompleted: {
        console.log("PackageView onCompleted", featureCollectionView.selectedIndex);
        if (!featuresTreeView.modelDepends || !featuresTreeView.modelTreeItems){
            featuresTreeView.loadFeaturesModel();
            featuresTreeView.loadDependModel();
        }

        Events.subscribeEvent("FeaturesTreeUpdate", featureCollectionView.refresh);
    }

    Component.onDestruction: {
        console.log("PackageView onDestruction");
        Events.unSubscribeEvent("FeaturesTreeUpdate", featureCollectionView.refresh)
    }

    onWasChangedChanged: {
        featureCollectionViewContainer.commandsChanged("PackageEdit");
    }

    function refresh() {
        console.log("PackageView refresh");
        featureCollectionView.refresh();
    }

    ListModel {
        id: contextMenuModel;

        Component.onCompleted: {
            contextMenuModel.append({"id": "Edit", "name": qsTr("Edit"), "imageSource": "../../../Icons/Light/Edit_On_Normal.svg", "mode": "Normal"});
            contextMenuModel.append({"id": "Remove", "name": qsTr("Remove"), "imageSource": "../../../Icons/Light/Remove_On_Normal.svg", "mode": "Normal"});
            contextMenuModel.append({"id": "", "name": "", "imageSource": "", "mode": "Normal"});
            contextMenuModel.append({"id": "SetDescription", "name": qsTr("Set Description"), "imageSource": "", "mode": "Normal"});
        }
    }

    function getDescriptionBySelectedItem(){
        var dataModelLocal = featureCollectionView.collectionViewModel.GetData("data");
        var description = dataModelLocal.GetData("Description", featureCollectionView.table.selectedIndex);
        return description;
    }

    function openContextMenu(item, mouseX, mouseY){
        if (featureCollectionView.table.height - mouseY <= 75){
            mouseY = mouseY - 75;
        }

        var point = featureCollectionViewContainer.mapToItem(thubnailDecoratorContainer, mouseX, mouseY);
        var source = "AuxComponents/PopupMenuDialog.qml";
        var parameters = {};
        parameters["model"] = contextMenuModel;
        parameters["resultItem"] = featureCollectionViewContainer;
        parameters["itemHeight"] = 25;
        parameters["itemWidth"] = 150;
        parameters["x"] = point.x;
        parameters["y"] = point.y;

        thubnailDecoratorContainer.openDialog(source, parameters);
    }

    function alreadyExistIdHasEmpty() {
        var dataModelLocal = featureCollectionView.collectionViewModel.GetData("data")
        for (var i = 0; i < dataModelLocal.GetItemsCount(); i++) {
            if (dataModelLocal.GetData("Id", i) === "#" || dataModelLocal.GetData("Id", i) === "") {
                return dataModelLocal.GetData("Name", i);
            }
        }
        return "";
    }

    UndoRedo {
        id: undoRedo;
    }

    function dialogResult(parameters){
         console.log("PackageView dialogResult", parameters["status"]);
        //featureCollectionView.forceActiveFocus();
        if (parameters["status"] === "ok"){
            if (parameters["dialog"] === "EditFeature"){
                var dataModelLocal = featureCollectionView.collectionViewModel.GetData("data");
                console.log("PackageView EditFeature dialogResult", dataModelLocal.GetItemsCount())

                var oldId = dataModelLocal.GetData("Id", featureCollectionView.selectedIndex);
                var oldName = dataModelLocal.GetData("Name", featureCollectionView.selectedIndex);

                if (oldId !== parameters["newFeatureId"] || oldName !== parameters["newFeatureName"]){

                    dataModelLocal.SetData("Id", parameters["newFeatureId"] , featureCollectionView.selectedIndex);
                    dataModelLocal.SetData("Name", parameters["newFeatureName"], featureCollectionView.selectedIndex);

                    if (featuresTreeView.featureIsNew(featureCollectionViewContainer.itemId, oldId)){
                        featuresTreeView.addFeatureInTreeViewModel(featureCollectionViewContainer.itemId,
                                                                                   parameters["newFeatureId"],
                                                                                   parameters["newFeatureName"]);
                    }
                    else{
                        featuresTreeView.updateTreeViewAfterFeatureEditing(featureCollectionViewContainer.itemId, oldId,
                                                                           parameters["newFeatureId"],
                                                                           parameters["newFeatureName"]);
                        featuresTreeView.updateFeaturesDependenciesAfterFeatureEditing(featureCollectionViewContainer.itemId, oldId,
                                                                           parameters["newFeatureId"],
                                                                           parameters["newFeatureName"]);
                    }
                    featureCollectionView.refresh();
                }
            }
            else if (parameters["dialog"] === "InputDialog") {
                if (parameters["typeOperation"] === "Save") {
                    var value = parameters["value"];
                    console.log("featureCollectionViewContainer dialogResult", value);
                    packageViewSaveQuery.updateModel(value);
                    //featureCollectionView.refresh();
                }
                else if (parameters["typeOperation"] === "SetDescription") {
                    var value = parameters["value"];
                    var dataModelLocal = featureCollectionView.collectionViewModel.GetData("data");

                    var oldDescription =  dataModelLocal.GetData("Description", featureCollectionView.table.selectedIndex);

                    if (oldDescription !== value){
                        dataModelLocal.SetData("Description", value, featureCollectionView.table.selectedIndex);
                        featureCollectionView.collectionViewModel.SetData("data", dataModelLocal);
                        featureCollectionView.refresh();
                        featureCollectionViewContainer.wasChanged = true;
                    }
                }
            }
        }
        else if (parameters["status"] === "yes"){

            if (featureCollectionView.collectionViewModel.ContainsKey("data")) {
                featuresTreeView.removeFeatureInTreeViewModel(featureCollectionViewContainer.itemId, featureCollectionView.table.getSelectedId());
                let dataModelLocal = featureCollectionView.collectionViewModel.GetData("data");
                dataModelLocal.RemoveItem(featureCollectionView.table.selectedIndex);
                //featureCollectionView.collectionViewModel.SetData("data", dataModelLocal);
                //featureCollectionView.collectionViewModel.Refresh();
                featureCollectionView.refresh();

                featureCollectionView.table.selectedIndex = -1;
                featureCollectionViewContainer.commandsChanged("PackageEdit");

                if (dataModelLocal.GetItemsCount() == 0){
                    treeView.visible = false;
                }

                //featureCollectionViewContainer.makeCommandActive("Save");


            }
        }
        else if (parameters["status"] === "Edit") {
            featureCollectionViewContainer.menuActivated("Edit");
        }
        else if (parameters["status"] === "Remove") {
            featureCollectionViewContainer.menuActivated("Remove");
        }
        else if (parameters["status"] === "Set Description") {
            var source = "AuxComponents/InputDialog.qml";
            var parameters = {};
            parameters["message"] = qsTr("Please enter the description of the feature:");
            parameters["nameDialog"] = "Set description";
            parameters["typeOperation"] = "SetDescription";
            parameters["startingValue"] = featureCollectionViewContainer.getDescriptionBySelectedItem();
            parameters["resultItem"] = featureCollectionViewContainer;
            thubnailDecoratorContainer.openDialog(source, parameters);
        }

//        console.log("featureCollectionView.forceActiveFocus()");
        featureCollectionView.forceActiveFocus();
    }

    function menuActivated(menuId) {
        console.log("PackageView menuActivated", menuId);
        if (menuId  === "New"){
            //var countItems = model.GetData("data").GetItemsCount();

            var dataModelLocal = model.GetData("data");
            var index = dataModelLocal.InsertNewItem();

            dataModelLocal.SetData("Name", "Feature Name", index);
            dataModelLocal.SetData("Id", "", index);
            dataModelLocal.SetData("Description", "", index);

            //model.SetData("data", dataModelLocal);
            //model.Refresh();
            featureCollectionView.refresh();
        }
        else if (menuId  === "Save") {
            console.log("PackageView operation save", featureCollectionViewContainer.itemId);

            var emptyId = featureCollectionViewContainer.alreadyExistIdHasEmpty();
            if (emptyId !== "") {
                featureCollectionView.openMessageDialog("Error", emptyId + " " + qsTr("has an empty id!"), "ErrorDialog");
                return;
            }

            if (featureCollectionViewContainer.itemId == "") {
                var source = "AuxComponents/InputDialog.qml";
                var parameters = {};
                parameters["message"] = qsTr("Please enter the name of the package: ");
                parameters["nameDialog"] = "Package Name";
                parameters["typeOperation"] = "Save";
                parameters["resultItem"] = featureCollectionViewContainer;
                thubnailDecoratorContainer.openDialog(source, parameters);
            }
            else {
                packageViewSaveQuery.updateModel()
            }
        }
        else if (menuId  === "Remove") {
            let source = "AuxComponents/MessageDialog.qml";
            let parameters = {};
            parameters["message"] = "Remove selected file from the collection?";
            parameters["nameDialog"] = "Remove";
            parameters["resultItem"] = featureCollectionViewContainer;

            thubnailDecoratorContainer.openDialog(source, parameters);
        }
        else if (menuId  === "Close") {
            featureCollectionViewContainer.rootItem.closeTab();
        }
        else {
           featureCollectionView.menuActivated(menuId)
        }
    }

    function makeCommandActive(commandId){
        featureCollectionViewContainer.rootItem.setModeMenuButton(commandId, "Normal");
    }

    function commandsChanged(commandsId){
        console.log("PackageView commandsChanged", commandsId, featureCollectionView.table.selectedIndex);
        if (commandsId !== "PackageEdit") {
            return;
        }

        if (featureCollectionView.table.selectedIndex > -1) {
            featureCollectionViewContainer.rootItem.setModeMenuButton("Remove", "Normal");
            featureCollectionViewContainer.rootItem.setModeMenuButton("Edit", "Normal");
            featureCollectionViewContainer.rootItem.setModeMenuButton("Import", "Normal");
            featureCollectionViewContainer.rootItem.setModeMenuButton("Export", "Normal");
            featureCollectionViewContainer.rootItem.setModeMenuButton("Close", "Normal");
            featureCollectionViewContainer.rootItem.setModeMenuButton("Save", "Normal");
        }
        else {
            featureCollectionViewContainer.rootItem.setModeMenuButton("Remove", "Disabled");
            featureCollectionViewContainer.rootItem.setModeMenuButton("Edit", "Disabled");
            featureCollectionViewContainer.rootItem.setModeMenuButton("Export", "Disabled");
//            featureCollectionViewContainer.rootItem.setModeMenuButton("Save", "Disabled");
            //featureCollectionViewContainer.rootItem.setModeMenuButton("New", "Disabled");
        }

//        featureCollectionViewContainer.rootItem.setModeMenuButton("Save", "Normal");
//        if (featureCollectionViewContainer.wasChanged){
//            featureCollectionViewContainer.rootItem.setModeMenuButton("Save", "Normal");
//        } else {
//            featureCollectionViewContainer.rootItem.setModeMenuButton("Save", "Disabled");
//        }
    }

    TreeItemModel {
        id: modelPackages;
    }

    function selectedIndexIncr(){
        console.log("PackageView selectedIndexIncr");
        if (featureCollectionView.table.selectedIndex == featureCollectionView.getCountItems() - 1){
            featureCollectionView.table.selectedIndex = 0;
        }
        else
            featureCollectionView.table.selectedIndex++;

        featureCollectionView.table.changeDataByIndex(featureCollectionView.table.selectedIndex);
    }

    function selectedIndexDecr(){
        console.log("PackageView selectedIndexDecr");
        if (featureCollectionView.table.selectedIndex == 0){
            featureCollectionView.table.selectedIndex = featureCollectionView.getCountItems() - 1;
        }
        else
            featureCollectionView.table.selectedIndex--;
        featureCollectionView.table.changeDataByIndex(featureCollectionView.table.selectedIndex);
    }

    function selectItem(){
        console.log("PackageCollectionView selectItem");

        var itemId = featureCollectionView.table.getSelectedId();
        var name = featureCollectionView.table.getSelectedName();
        featureCollectionView.itemSelect(itemId, name);
    }

    CollectionView {
        id: featureCollectionView;

        anchors.left: parent.left;
        anchors.right: packageSplitter.left;

        rootItem: featureCollectionViewContainer;

        height: parent.height;

        Component.onCompleted: {
            console.log("PackageView CollectionView onCompleted");
            featureCollectionView.gqlModelInfo = "PackageInfo"
            featureCollectionView.gqlModelItems = "FeatureList"
            featureCollectionView.gqlModelRemove = "";
        }

        onItemIdChanged: {
            console.log("PackageView CollectionView onItemIdChanged", featureCollectionView.itemId)
            if (featureCollectionView.itemId){
                featureCollectionView.gqlModelInfo = "PackageInfo"
                featureCollectionView.gqlModelItems = "FeatureList"
                featureCollectionView.gqlModelRemove = "";
            }
        }

        onCollectionViewRightButtonMouseClicked: {
            console.log("PackageView CollectionView AuxTable onCollectionViewRightButtonMouseClicked");
            featureCollectionViewContainer.openContextMenu(item, mouseX, mouseY);
        }

        onItemSelect: {
            console.log("PackageView CollectionView onSelectItem", idSelect, name);
            var source = "../imtlicgui/EditFeatureDialog.qml";
            var parameters = {};
            parameters["featureId"] = idSelect;
            parameters["featureName"] = name;
            parameters["collectionViewFeatures"] = featureCollectionView;
            parameters["resultItem"] = featureCollectionViewContainer;
            thubnailDecoratorContainer.openDialog(source, parameters);
        }

        onSelectedIndexChanged: {
            console.log("PackageView CollectionView onSelectedIndexChanged", featureCollectionView.selectedIndex);
            if (featureCollectionView.selectedIndex > -1){
                featureCollectionViewContainer.commandsChanged("PackageEdit")
                //valueText.visible = false;
                var dataModelLocal = featureCollectionView.collectionViewModel.GetData("data");

                let curFeatureId = featureCollectionView.table.getSelectedId();
                if (curFeatureId == ""){
                    treeView.visible = false;
                }
                else{
                    if (featureCollectionViewContainer.operation != "New"){
                        treeView.visible = true;
                    }
                }
            }
            else{

                treeView.visible = false;
                return;
            }

            let rootPackageId = featureCollectionViewContainer.itemId;
            let rootFeatureId = featureCollectionView.table.getSelectedId();
            let rootkey = rootPackageId + '.' + rootFeatureId;

            featuresTreeView.clearTreeView();
            console.log("packageview hideCurrentFeatureTreeView", rootPackageId, rootFeatureId)
            featuresTreeView.hideCurrentFeatureTreeView(rootPackageId, rootFeatureId);
            if (featuresTreeView.modelDepends){
                let strValues = featuresTreeView.modelDepends.GetData(rootkey);
                if (strValues){
                    let values = strValues.split(';');
                    for (let value of values){
                        featuresTreeView.selectFeature(value);
                    }
                }

                let upFeatures = [];
                featuresTreeView.getFeaturesDependsByFeatureUp(rootkey, upFeatures);

                if (upFeatures.length > 0){
                    featuresTreeView.updateDataFeatureList(upFeatures, 0, 0);
                }
            }
        }

        onDataLoaded: {
            loadingPage.visible = false;
            featureCollectionViewContainer.makeCommandActive("Save");
           // featureCollectionViewContainer.makeCommandActive("New");
        }
    }

    GqlModel {
        id: packageViewSaveQuery;

        function updateModel(newId) {
            console.log( "updateModel saveModel");

            var query;
            var queryFields;
            var inputParams = Gql.GqlObject("input");

            if(featureCollectionViewContainer.itemId != ""){
                featureCollectionViewContainer.gqlModelQueryType = "PackageUpdate";
                featureCollectionViewContainer.gqlModelQueryTypeNotification = "updatedNotification";
                query = Gql.GqlRequest("query", "PackageUpdate");
                inputParams.InsertField("Id", featureCollectionViewContainer.itemId);
                queryFields = Gql.GqlObject("updatedNotification");
            }
            else{
                featureCollectionViewContainer.gqlModelQueryType = "PackageAdd";
                featureCollectionViewContainer.gqlModelQueryTypeNotification = "addedNotification";
                query = Gql.GqlRequest("query", "PackageAdd");
                queryFields = Gql.GqlObject("addedNotification");
            }
            query.AddParam(inputParams);

            if (featureCollectionViewContainer.gqlModelQueryType == "PackageAdd"){
                modelPackages.SetData("Id", newId);
                modelPackages.SetData("Name", newId);
            }
            else {
                modelPackages.SetData("Id", featureCollectionViewContainer.itemId);
                modelPackages.SetData("Name", featureCollectionViewContainer.itemName);
            }

            modelPackages.SetExternTreeModel("features", featureCollectionView.collectionViewModel.GetData("data"));
            modelPackages.SetExternTreeModel("dependencies", featuresTreeView.modelDepends);

            var jsonString = modelPackages.toJSON();
            jsonString = jsonString.replace(/\"/g,"\\\\\\\"")

            inputParams.InsertField ("Item", jsonString);

            queryFields.InsertField("Id");
            queryFields.InsertField("Successed");

            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            console.log("PackageEdit query ", gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, packageViewSaveQuery);
            if (this.state === "Ready"){
                let keys = packageViewSaveQuery.GetKeys();
                if (!keys || keys.length == 0){
                    thubnailDecoratorContainer.setInvalidConnection(true);
                    return;
                }

                var dataModelLocal;
                if (packageViewSaveQuery.ContainsKey("errors")){
                    dataModelLocal = packageViewSaveQuery.GetData("errors");
                    dataModelLocal = dataModelLocal.GetData(featureCollectionViewContainer.gqlModelQueryType);
                    if (dataModelLocal){
                        var messageError = dataModelLocal.GetData("message");
                        featureCollectionView.openMessageDialog("Error", messageError, "ErrorDialog");
                    }

                    return;
                }

                if (packageViewSaveQuery.ContainsKey("data")){
                    dataModelLocal = packageViewSaveQuery.GetData("data");

                    dataModelLocal = dataModelLocal.GetData(featureCollectionViewContainer.gqlModelQueryType);

                    if (dataModelLocal && dataModelLocal.ContainsKey(featureCollectionViewContainer.gqlModelQueryTypeNotification)){
                        dataModelLocal = dataModelLocal.GetData(featureCollectionViewContainer.gqlModelQueryTypeNotification);

                        var newId = dataModelLocal.GetData("Id");
                        var newName = dataModelLocal.GetData("Name");
                        featureCollectionViewContainer.itemId = newId;

                        if (featureCollectionViewContainer.itemName !== newId){
                            featureCollectionViewContainer.itemName = newId;
                            featureCollectionViewContainer.rootItem.updateTitleTab(featureCollectionViewContainer.itemId, newId);
                        }

                        let modelLocal = featureCollectionView.collectionViewModel.GetData("data");
                        if (featureCollectionViewContainer.operation == "New"){

                            featuresTreeView.loadFeaturesModel();
                            featuresTreeView.loadDependModel();

                            featureCollectionViewContainer.operation = "Open";

                            if (modelLocal.GetItemsCount() > 0){
                                treeView.visible = true;
                            }
                        }

                        featureCollectionViewContainer.multiDocViewItem.activeCollectionItem.callMetaInfoQuery();
                    }
                }
            }
        }
    }

    Splitter {
        id: packageSplitter;
        x: featureCollectionViewContainer.width - 250;

        height: parent.height;
        width: 4;

        onXChanged: {
            if (!featureCollectionViewContainer.visible){
                return;
            }

            if (packageSplitter.x > featureCollectionViewContainer.width - titleHeader.width){
                packageSplitter.x = featureCollectionViewContainer.width - packageSplitter.width;
            }

            if (packageSplitter.x < 250){
                packageSplitter.x = 250;
            }
        }
    }

    Rectangle {
        id: packageMetaInfo;

        //anchors.right: parent.right;
        anchors.left: packageSplitter.right;

        width: featureCollectionViewContainer.width > 0 ? featureCollectionViewContainer.width - featureCollectionView.width : 250;
        height: parent.height;

        color: "transparent";

        Rectangle {
            id: headerTreeView;

            anchors.top: parent.top;

            width: parent.width;
            height: 35;

            color: Style.baseColor;

            Text {
                id: titleHeader;

                anchors.verticalCenter: headerTreeView.verticalCenter;

                anchors.left: headerTreeView.left;
                anchors.leftMargin: 10;

                text: qsTr("Dependencies");
                color: Style.textColor;
                font.pixelSize: Style.fontSize_common;
                font.family: Style.fontFamilyBold;
                font.bold: true;
            }
        }

        Rectangle {
            id: headerBottomBorder;

            anchors.bottom: headerTreeView.bottom;

            height: 1;
            width: parent.width;

            color: "lightgray";
        }

        Text {
            id: valueText;

            anchors.top: headerBottomBorder.bottom;
            anchors.topMargin: 10;
            anchors.horizontalCenter: parent.horizontalCenter;

            text: qsTr("Please save the package first!");
            visible: featureCollectionViewContainer.operation == "New";

            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;

            color: Style.textColor;
        }

        TreeView {
            id: treeView;

            anchors.top: headerBottomBorder.bottom;

            height: parent.height;
            width: parent.width;

            visible: false;

            clip: true;

            modelItems: featuresTreeView.modelTreeItems;

            onItemTreeViewCheckBoxStateChanged: {
                console.log("PackageView TreeView onItemTreeViewCheckBoxStateChanged", state, packageId, featureId);
                featureCollectionViewContainer.wasChanged = true;

                let rootPackageId = featureCollectionViewContainer.itemId;
                let rootFeatureId = featureCollectionView.table.getSelectedId();
                let rootkey = rootPackageId + '.' + rootFeatureId;
                let value = packageId + '.' + featureId;

                if (state == 0){
                    featuresTreeView.deselectFeature(value);
                }

                if (featuresTreeView.modelDepends.ContainsKey(rootkey)){
                    let str = featuresTreeView.modelDepends.GetData(rootkey);
                    let arr = str.split(";");
                    if (state == 0){
                        if (arr){
                            let index = arr.indexOf(value);
                            if (index > -1){
                                arr.splice(index, 1);
                            }

                            if (arr.length == 0){
                                featuresTreeView.modelDepends.RemoveData(rootkey);
                            }
                            else{
                                let resStr = arr.join(';');
                                featuresTreeView.modelDepends.SetData(rootkey, resStr);
                            }
                        }
                    }
                    else{
                        if (arr.indexOf(value) == -1){
                            arr.push(value);
                            let resStr = arr.join(';');
                            featuresTreeView.modelDepends.SetData(rootkey, resStr);
                        }
                    }
                }
                else{
                    featuresTreeView.modelDepends.SetData(rootkey, value);
                }

                if (state == 2){
                    featuresTreeView.selectFeature(value);
                }
            }
        }
    }

    LoadingPage {
        id: loadingPage;

        anchors.fill: parent;

        visible: false;
    }
}
