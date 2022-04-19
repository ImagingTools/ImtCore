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

    Component.onCompleted: {
        console.log("PackageView onCompleted", featureCollectionView.selectedIndex);
    }

    onWasChangedChanged: {
        featureCollectionViewContainer.commandsChanged("PackageEdit");
    }

    Keys.onPressed: {
        console.log("PackageView keys pressed")

        if (treeView.focus){
            return;
        }

        if (event.key === Qt.Key_Tab){
            console.log('Key tab was pressed');
            treeView.forceActiveFocus();
        }
        else if (event.key === Qt.Key_Up){
            console.log('Key up was pressed');
            featureCollectionViewContainer.selectedIndexDecr();
        }
        else if (event.key === Qt.Key_Down){
            console.log('Key down was pressed');
             featureCollectionViewContainer.selectedIndexIncr();
        }
        else if (event.key === Qt.Key_Return){
            console.log('Key down was pressed');

            featureCollectionViewContainer.selectItem();
        }
    }

    function refresh() {
        console.log("PackageView refresh");
        featureCollectionView.refresh();
        featuresTreeView.loadFeaturesModel();
        featuresTreeView.loadDependModel();
    }

//    function openMessageDialog(nameDialog, message) {

//        var source = "AuxComponents/MessageDialog.qml";
//        var parameters = {};
//        parameters["resultItem"] = featureCollectionViewContainer;
//        parameters["noButtonVisible"] = false;
//        parameters["textOkButton"] = "Ok";
//        parameters["message"] = message;
//        parameters["nameDialog"] = nameDialog;

//        thubnailDecoratorContainer.openDialog(source, parameters);
//    }

    ListModel {
        id: contextMenuModel;

        Component.onCompleted: {
            contextMenuModel.append({"id": "Edit", "name": "Edit", "imageSource": "../../../Icons/Light/Edit_On_Normal.svg", "mode": "Normal"});
            contextMenuModel.append({"id": "Remove", "name": "Remove", "imageSource": "../../../Icons/Light/Remove_On_Normal.svg", "mode": "Normal"});
            contextMenuModel.append({"id": "", "name": "", "imageSource": "", "mode": "Normal"});
            contextMenuModel.append({"id": "SetDescription", "name": "Set Description", "imageSource": "", "mode": "Normal"});
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

    function dialogResult(parameters) {
         console.log("PackageView dialogResult", parameters["status"]);

//        if (parameters["dialog"] === "EditFeature"){

//            if (parameters["status"] === "ok") {
//                var dataModelLocal = featureCollectionView.collectionViewModel.GetData("data");
//                console.log("PackageView onClicked ", dataModelLocal.GetItemsCount())
//                dataModelLocal.SetData("Id", parameters["newFeatureId"] , featureCollectionView.selectedIndex);
//                dataModelLocal.SetData("Name", parameters["newFeatureName"], featureCollectionView.selectedIndex);
//                dataModelLocal.SetData("Description", "", featureCollectionView.selectedIndex);
//                featureCollectionView.collectionViewModel.SetData("data", dataModelLocal);
//                featureCollectionView.refresh();

//                featuresTreeView.addFeatureInTreeViewModel(featureCollectionViewContainer.itemId,
//                                                           parameters["newFeatureId"],
//                                                           parameters["newFeatureName"]);
//            }
//        }
//        else if (parameters["dialog"] === "InputDialog"){

//            if (parameters["typeOperation"] === "Save") {
//                var value = parameters["value"];
//                console.log("featureCollectionViewContainer dialogResult", value);
//                packageViewSaveQuery.updateModel(value);
//            }
//            else if (parameters["typeOperation"] === "SetDescription") {
//                var value = parameters["value"];
//                var dataModelLocal = featureCollectionView.collectionViewModel.GetData("data");

//                dataModelLocal.SetData("Description", value, featureCollectionView.table.selectedIndex);
//                featureCollectionView.collectionViewModel.SetData("data", dataModelLocal);

//                featureCollectionView.refresh();
//            }
//        }
//        else if (parameters["dialog"] === "PopupMenu"){

//            if (parameters["status"] === "Edit") {
//                featureCollectionViewContainer.menuActivated("Edit");
//            }
//            else if (parameters["status"] === "Remove") {
//                featureCollectionViewContainer.menuActivated("Remove");
//            }
//            else if (parameters["status"] === "Set Description") {
//                var source = "AuxComponents/InputDialog.qml";
//                var parameters = {};
//                parameters["message"] = "Please enter the description of the feature: ";
//                parameters["nameDialog"] = "Set description";
//                parameters["typeOperation"] = "SetDescription";
//                parameters["startingValue"] = featureCollectionViewContainer.getDescriptionBySelectedItem();
//                parameters["resultItem"] = featureCollectionViewContainer;
//                thubnailDecoratorContainer.openDialog(source, parameters);
//            }
//        }

        if (parameters["status"] === "ok") {

            if (parameters["dialog"] === "EditFeature") {
                var dataModelLocal = featureCollectionView.collectionViewModel.GetData("data");
                console.log("PackageView onClicked ", dataModelLocal.GetItemsCount())


                var oldId = dataModelLocal.GetData("Id", featureCollectionView.selectedIndex);
                var oldName = dataModelLocal.GetData("Name", featureCollectionView.selectedIndex);

                if (oldId !== parameters["newFeatureId"] || oldName !== parameters["newFeatureName"]){
                    dataModelLocal.SetData("Id", parameters["newFeatureId"] , featureCollectionView.selectedIndex);
                    dataModelLocal.SetData("Name", parameters["newFeatureName"], featureCollectionView.selectedIndex);
                    featureCollectionView.collectionViewModel.SetData("data", dataModelLocal);
                    featureCollectionView.refresh();

                    featureCollectionViewContainer.updateDependsModelAfterEdit(oldId, parameters["newFeatureId"]);

                    featureCollectionViewContainer.wasChanged = true;
                }

                featuresTreeView.addFeatureInTreeViewModel(featureCollectionViewContainer.itemId,
                                                           parameters["newFeatureId"],
                                                           parameters["newFeatureName"]);
            }
            else if (parameters["dialog"] === "InputDialog") {
                if (parameters["typeOperation"] === "Save") {
                    var value = parameters["value"];
                    console.log("featureCollectionViewContainer dialogResult", value);
                    packageViewSaveQuery.updateModel(value);
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
        else if (parameters["status"] === "yes") {

            if (featureCollectionView.collectionViewModel.ContainsKey("data")) {
                var dataModelLocal = featureCollectionView.collectionViewModel.GetData("data");
                dataModelLocal.RemoveItem(featureCollectionView.table.selectedIndex);
                featureCollectionView.collectionViewModel.SetData("data", dataModelLocal);
                featureCollectionView.collectionViewModel.Refresh();
                featureCollectionView.refresh();

                featureCollectionView.table.selectedIndex = -1;
                featureCollectionViewContainer.commandsChanged("PackageEdit");
                featureCollectionViewContainer.makeCommandActive("Save");

                featuresTreeView.removeFeatureInTreeViewModel(featureCollectionViewContainer.itemId, featureCollectionView.table.getSelectedId());
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
            parameters["message"] = "Please enter the description of the feature: ";
            parameters["nameDialog"] = "Set description";
            parameters["typeOperation"] = "SetDescription";
            parameters["startingValue"] = featureCollectionViewContainer.getDescriptionBySelectedItem();
            parameters["resultItem"] = featureCollectionViewContainer;
            thubnailDecoratorContainer.openDialog(source, parameters);
        }
    }

    function updateDependsModelAfterEdit(oldFeatureId, newFeatureId){
        console.log("PackageView updateDependsModelAfterEdit", oldFeatureId, newFeatureId);
        featuresTreeView.printInfo();

        if (!featuresTreeView.modelDepends){
            return;
        }

        for (var i = 0; i < featuresTreeView.modelDepends.GetItemsCount(); i++){
            var rootFeatureId = featuresTreeView.modelDepends.GetData("RootFeatureId", i);
            var rootPackageId = featuresTreeView.modelDepends.GetData("RootPackageId", i);

            if (rootFeatureId === oldFeatureId && rootPackageId === featureCollectionViewContainer.itemId){
                featuresTreeView.modelDepends.SetData("RootFeatureId", newFeatureId, i);
            }

            var packages = featuresTreeView.modelDepends.GetData("Packages", i);

            if (!packages){
                continue;
            }

            var indexChild = -1;
            for (var j = 0; j < packages.GetItemsCount(); j++){
                 var packageId = packages.GetData("Id", j);

                if (packageId === featureCollectionViewContainer.itemId){
                    indexChild = j;
                }
            }

            if (indexChild === -1){
                continue;
            }

            var childItems = packages.GetData("childItemModel", indexChild);

            if (!childItems){
                continue;
            }

            for (j = 0; j < childItems.GetItemsCount(); j++){
                var curId = childItems.GetData("Id", j);

                if (curId === oldFeatureId){
                    childItems.SetData("Id", newFeatureId, j);
                }
            }

            packages.SetData("childItemModel", childItems, indexChild);
            featuresTreeView.modelDepends.GetData("Packages", packages, i);
        }
        featuresTreeView.printInfo();

        featuresTreeView.modelDepends.Refresh();
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

            model.SetData("data", dataModelLocal);
            //model.Refresh();
            featureCollectionView.refresh();
        }
        else if (menuId  === "Save") {
            console.log("PackageView operation save", featureCollectionViewContainer.itemId);

            var emptyId = featureCollectionViewContainer.alreadyExistIdHasEmpty();
            if (emptyId !== "") {
                featureCollectionView.openMessageDialog("ErrorDialog", emptyId + " has an empty id !", "ErrorDialog");
                return;
            }

            if (featureCollectionViewContainer.itemId == "") {
                var source = "AuxComponents/InputDialog.qml";
                var parameters = {};
                parameters["message"] = "Please enter the name of the document: ";
                parameters["nameDialog"] = "Document Name";
                parameters["typeOperation"] = "Save";
                parameters["resultItem"] = featureCollectionViewContainer;
                thubnailDecoratorContainer.openDialog(source, parameters);
            }
            else {
                packageViewSaveQuery.updateModel()
            }
        }
        else if (menuId  === "Remove") {

            var source = "AuxComponents/MessageDialog.qml";
            var parameters = {};
            parameters["message"] = "Remove selected file from the database ?";
            parameters["nameDialog"] = "RemoveDialog";
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
        } else {
            featureCollectionViewContainer.rootItem.setModeMenuButton("Remove", "Disabled");
            featureCollectionViewContainer.rootItem.setModeMenuButton("Edit", "Disabled");
            featureCollectionViewContainer.rootItem.setModeMenuButton("Export", "Disabled");
        }

        featureCollectionViewContainer.rootItem.setModeMenuButton("Save", "Normal");
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
                treeView.visible = true;
            } else {
                treeView.visible = false;
            }

            featureCollectionViewContainer.updateFeaturesTreeView();
        }

        onSetActiveFocusFromCollectionView: {
            console.log("PackageView CollectionView onSetActiveFocusFromCollectionView");
            featureCollectionViewContainer.forceActiveFocus();
        }
    }

    function updateFeaturesTreeView(){
        console.log("PackageView updateFeaturesTreeView");
        featureCollectionViewContainer.hideCurrentFeatureTreeView();
        featureCollectionViewContainer.checkInActiveItems(featureCollectionView.itemId, featureCollectionView.table.getSelectedId());
        featureCollectionViewContainer.updateStateCheckedCheckBox();
        featuresTreeView.printInfo();
    }

    function clearCheckedCheckBox() {
        console.log("PackageView clearCheckedCheckBox");
        var modelItems = treeView.modelItems;

        for (var i = 0; i < modelItems.GetItemsCount(); i++) {
            var modelChildren = modelItems.GetData("childItemModel", i);

            if (!modelChildren){
                continue;
            }

            for (var j = 0; j < modelChildren.GetItemsCount(); j++) {

                if (modelChildren.GetData("stateChecked", j) === 2) {
                    modelChildren.SetData("stateChecked", 0, j);
                }
            }

            modelItems.SetData("childItemModel", modelChildren, i);
        }
        treeView.modelItems = modelItems;
//        featureCollectionViewContainer.printModelItems(treeView.modelItems);
    }

    function printModelItems(modelItems) {
        //var modelItems = treeView.modelItems;

        for (var i = 0; i < modelItems.GetItemsCount(); i++) {
            var modelChildren = modelItems.GetData("childItemModel", i);
            console.log("Package Id", modelItems.GetData("Id", i));

            if (modelChildren) {
                for (var j = 0; j < modelChildren.GetItemsCount(); j++) {
                    console.log("\tFeature id ", modelChildren.GetData("Id", j));
                    console.log("\tstateChecked ", modelChildren.GetData("stateChecked", j));
                    console.log("\tisActive ", modelChildren.GetData("isActive", j));
                    console.log("\tvisible ", modelChildren.GetData("visible", j));
                }
            }
        }
    }

    function checkInActiveItems(curPackageId, curFeatureId) {
        console.log("PackageView checkInActiveItems");

//        var curFeatureId = featureCollectionView.table.getSelectedId();
//        var curPackageId = featureCollectionView.itemId;
        var modelItems = treeView.modelItems;
        if (!modelItems){
            return;
        }

        var result = [];
        var dependsFeatures = featuresTreeView.findInAllRootFeaturesDependFeatureById(curPackageId, curFeatureId, result);
        console.log("result", result);
        for (var i = 0; i < modelItems.GetItemsCount(); i++) {
            var packageId = modelItems.GetData("Id", i);
            var modelChildren = modelItems.GetData("childItemModel", i);
            console.log("Current Package Id", packageId);
            if (!modelChildren) {
                continue;
            }

            for (var j = 0; j < modelChildren.GetItemsCount(); j++) {
                var id = modelChildren.GetData("Id", j);
                modelChildren.SetData("isActive", 1, j);
                for (var k = 0; k < dependsFeatures.length; k++) {
                    var data = dependsFeatures[k].split(".");
                    var pId = data[0];
                    var fId = data[1];

                    if (pId === packageId && fId === id) {
                        modelChildren.SetData("isActive", 0, j);
                    }
                }
            }
            modelItems.SetData("childItemModel", modelChildren, i);
        }
        treeView.modelItems = modelItems;
//        featureCollectionViewContainer.printModelItems(treeView.modelItems);
    }

    function selectDependsFeatures(packageId, featureId, state){
        console.log("selectDependsFeatures", packageId, featureId, state);
        featuresTreeView.printInfo();
        /*
          Выбрать фичи от которых зависит featureId
            state = true => делаем select
            state = false => делаем deselect

        */

        if (!treeView.modelItems){
            return;
        }

        var result = [];
        featuresTreeView.findInAllDependsFeaturesByRootFeatureId(packageId, featureId, result);
        for (var i = 0; i < treeView.modelItems.GetItemsCount(); i++) {
            var packId = treeView.modelItems.GetData("Id", i);
            var modelChildren = treeView.modelItems.GetData("childItemModel", i);

            if (!modelChildren) {
                continue;
            }

            for (var j = 0; j < modelChildren.GetItemsCount(); j++) {
                var id = modelChildren.GetData("Id", j);
                //modelChildren.SetData("isActive", 1, j);
                for (var k = 0; k < result.length; k++) {
                    var data = result[k].split(".");
                    var pId = data[0];
                    var fId = data[1];

                    if (pId === packId && fId === id) {
                        if (state){
                            modelChildren.SetData("isActive", 0, j);
                            modelChildren.SetData("stateChecked", 2, j);
                        }
                        else{
                            modelChildren.SetData("isActive", 1, j);
                            modelChildren.SetData("stateChecked", 0, j);
                        }
                    }
                }
            }
            treeView.modelItems.SetData("childItemModel", modelChildren, i);
        }
    }

    function updateStateCheckedCheckBox() {
        console.log("PackageView updateStateCheckedCheckBox", featuresTreeView);
        var curFeatureId = featureCollectionView.table.getSelectedId();
        var curPackageId = featureCollectionView.itemId;

        var modelItems = treeView.modelItems;
        var rootIndex = featuresTreeView.getIndexByRootFeatureId(curFeatureId);

        if (!modelItems){
            return;
        }

        if (rootIndex !== -1) {
            //var packagesModel =  featuresTreeView.modelDepends.GetData("Packages", rootIndex);

            for (var i = 0; i < modelItems.GetItemsCount(); i++) {
                var packageId = modelItems.GetData("Id", i);
                var modelChildren = modelItems.GetData("childItemModel", i);

                var packageIndex = featuresTreeView.getIndexByPackageId(rootIndex, packageId);

                if (!modelChildren || packageIndex === -1) {
                    continue;
                }

                for (var j = 0; j < modelChildren.GetItemsCount(); j++) {
                    var id = modelChildren.GetData("Id", j);
                    if (featuresTreeView.dependModePackageGetIndexByFeatureId(rootIndex, packageIndex, id) !== -1){
                        modelChildren.SetData("stateChecked", 2, j);
                        featureCollectionViewContainer.selectDependsFeatures(packageId, id, true);
                    }
                    else{
                        modelChildren.SetData("stateChecked", 0, j);
                        //featureCollectionViewContainer.selectDependsFeatures(packageId, id, false);
                    }
                }
                modelItems.SetData("childItemModel", modelChildren, i);
            }
            treeView.modelItems = modelItems;
//            featureCollectionViewContainer.printModelItems(treeView.modelItems);
        }
        else {
            featureCollectionViewContainer.clearCheckedCheckBox();
        }
    }

    function hideCurrentFeatureTreeView() {
        console.log("PackageView hideCurrentFeatureTreeView");
        //var modelItems = treeView.modelItems;

        if (!treeView.modelItems){
            return;
        }

        for (var i = 0; i < treeView.modelItems.GetItemsCount(); i++) {
            if (treeView.modelItems.GetData("Name", i) === featureCollectionViewContainer.itemId) {
                var childModelItems = treeView.modelItems.GetData("childItemModel", i);

                if (!childModelItems){
                    continue;
                }

                for (var j = 0; j < childModelItems.GetItemsCount(); j++) {
                    if (childModelItems.GetData("visible", j) === 0) {
                        childModelItems.SetData("visible", 1, j);
                    }

                    if (childModelItems.GetData("Id", j) === featureCollectionView.table.getSelectedId()){
                        console.log("PackageView element hided", featureCollectionView.table.getSelectedId());
                        childModelItems.SetData("visible", 0, j);
                    }
                }
                treeView.modelItems.SetData("childItemModel", childModelItems, i);
                break;
            }
        }

        treeView.modelItems.Refresh();
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
                inputParams.InsertField("Id");
                inputParams.InsertFieldArgument("Id", featureCollectionViewContainer.itemId);
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
                modelPackages.SetData("Id", featureCollectionViewContainer.itemId );
                modelPackages.SetData("Name", featureCollectionViewContainer.itemName);
            }

            modelPackages.SetExternTreeModel("features", featureCollectionView.collectionViewModel.GetData("data"));
            modelPackages.SetExternTreeModel("dependencies", featuresTreeView.modelDepends);

            var jsonString = modelPackages.toJSON();
            jsonString = jsonString.replace(/\"/g,"\\\\\\\"")

            inputParams.InsertField("Item");
            inputParams.InsertFieldArgument ("Item", jsonString);

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
                var dataModelLocal;
                if (packageViewSaveQuery.ContainsKey("errors")){
                    dataModelLocal = packageViewSaveQuery.GetData("errors");
                    dataModelLocal = dataModelLocal.GetData(featureCollectionViewContainer.gqlModelQueryType);
                    if (dataModelLocal){
                        var messageError = dataModelLocal.GetData("message");
                        featureCollectionView.openMessageDialog("Error Dialog", messageError, "ErrorDialog");
                    }

                    return;
                }

                if (packageViewSaveQuery.ContainsKey("data")){
                    dataModelLocal = packageViewSaveQuery.GetData("data");

                    dataModelLocal = dataModelLocal.GetData(featureCollectionViewContainer.gqlModelQueryType);

                    if (dataModelLocal && dataModelLocal.ContainsKey(featureCollectionViewContainer.gqlModelQueryTypeNotification)){
                        dataModelLocal = dataModelLocal.GetData(featureCollectionViewContainer.gqlModelQueryTypeNotification);

                        var newId = dataModelLocal.GetData("Id");

                        featureCollectionViewContainer.itemId = newId;

                        if (featureCollectionViewContainer.itemName !== newId){
                            featureCollectionViewContainer.itemName = newId;
                            featureCollectionViewContainer.rootItem.updateTitleTab(featureCollectionViewContainer.itemId, newId);
                        }

                        featureCollectionViewContainer.multiDocViewItem.activeCollectionItem.callMetaInfoQuery();
                        featureCollectionViewContainer.wasChanged = false;
                    }
                }

            }
        }
    }

    Splitter {
        id: packageSplitter;
        x: featureCollectionViewContainer.width - 300;

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

        anchors.right: parent.right;
        anchors.left: packageSplitter.right;

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

                text: "Features";
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

        TreeView {
            id: treeView;

            anchors.top: headerBottomBorder.bottom;

            width: 200;
            height: parent.height;

            visible: false;

            clip: true;

            onItemTreeViewCheckBoxStateChanged: {
                console.log("PackageView TreeView onItemTreeViewCheckBoxStateChanged", state, packageId, featureId);
                featureCollectionViewContainer.wasChanged = true;

                if (state == 0){
                    featureCollectionViewContainer.selectDependsFeatures(packageId, featureId, false);
                }

                var curFeatureId = featureCollectionView.table.getSelectedId();
                var curPackageId = featureCollectionView.itemId;

                var rootIndex = featuresTreeView.getIndexByRootFeatureId(curFeatureId);
                if (rootIndex === -1) {
                    rootIndex = featuresTreeView.modelDepends.InsertNewItem();
                    featuresTreeView.modelDepends.SetData("RootFeatureId", curFeatureId, rootIndex);
                    featuresTreeView.modelDepends.SetData("RootPackageId", curPackageId, rootIndex);
                }

                var modelPackages;
                if (featuresTreeView.rootFeatureIdHasDependPackage(rootIndex)) {
                    modelPackages = featuresTreeView.modelDepends.GetData("Packages", rootIndex);
                }
                else {
                    modelPackages = featuresTreeView.modelDepends.AddTreeModel("Packages", rootIndex);
                }

                var packageIndex = featuresTreeView.getPackageIndexByPackageId(packageId, rootIndex);
                if (packageIndex === -1) {
                    packageIndex = featuresTreeView.addNewPackageToRootFeatureByRootIndex(packageId, rootIndex);
                }

                var modelChildren;
                if (featuresTreeView.dependModelPackageHasChild(rootIndex, packageIndex)) {
                    modelChildren = modelPackages.GetData("childItemModel", packageIndex);
                }
                else {
                    modelChildren = modelPackages.AddTreeModel("childItemModel", packageIndex);
                }

                var childIndex = featuresTreeView.dependModePackageGetIndexByFeatureId(rootIndex, packageIndex, featureId);

                if (childIndex === -1) {
                    if (state === 2) {
                        childIndex = modelChildren.InsertNewItem();
                        modelChildren.SetData("Id", featureId, childIndex);
                        modelChildren.SetData("stateChecked", state, childIndex);
                    }

                }
                else {
                    if (state === 0) {
                        modelChildren.RemoveItem(childIndex);
                    }
                }

                modelPackages.SetData("childItemsModel", modelChildren, packageIndex);

                if (modelChildren.GetItemsCount() === 0) {
                    featuresTreeView.dependModelRemovePackageByIndex(rootIndex, packageIndex);
                }

                featuresTreeView.modelDepends.SetData("Packages", modelPackages, rootIndex);

                if (modelPackages.GetItemsCount() === 0) {
                    featuresTreeView.dependModelRemoveRootFeatureByIndex(rootIndex);
                }


                if (state == 2){
                    featureCollectionViewContainer.selectDependsFeatures(packageId, featureId, true);
                }
               // featureCollectionViewContainer.checkInActiveItems(packageId, featureId);
                featuresTreeView.printInfo();
            }
        }

        FeaturesTreeView {
            id: featuresTreeView;
            //packageItem: featureCollectionViewContainer;

            onModelDependsChanged: {
                console.log( "PackageView FeaturesTreeView onModelDependsChanged");
                featureCollectionViewContainer.updateFeaturesTreeView();
            }

            onModelTreeItemsChanged: {
                console.log("PackageView FeaturesTreeView onModelTreeItemsChanged");
                treeView.modelItems = featuresTreeView.modelTreeItems;
            }

        }
    }
}
