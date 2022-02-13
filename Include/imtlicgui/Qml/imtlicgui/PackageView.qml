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


    property int contextMenuX;
    property int contextMenuY;

    Component.onCompleted: {
        console.log("PackageView onCompleted", featureCollectionView.selectedIndex);
    }

    onItemIdChanged: {
        console.log("PackageView onItemIdChanged", featureCollectionViewContainer.itemId)
    }

    function refresh() {
        console.log("PackageView refresh()");
        featureCollectionView.refresh();
        featuresTreeView.loadFeaturesModel();
        featuresTreeView.loadDependModel();
    }

    function openMessageDialog(nameDialog, message) {

        var source = "AuxComponents/MessageDialog.qml";
        var parameters = {};
        parameters["resultItem"] = featureCollectionViewContainer;
        parameters["noButtonVisible"] = false;
        parameters["textOkButton"] = "Ok";
        parameters["message"] = message;
        parameters["nameDialog"] = nameDialog;

        thubnailDecoratorContainer.openDialog(source, parameters);
    }

    ListModel {
        id: contextMenuModel;

        Component.onCompleted: {
            contextMenuModel.append({"id": "Edit", "name": "Edit", "imageSource": "../../../Icons/Light/Edit_On_Normal.svg", "mode": "Normal"});
            contextMenuModel.append({"id": "Remove", "name": "Remove", "imageSource": "../../../Icons/Light/Remove_On_Normal.svg", "mode": "Normal"});
            contextMenuModel.append({"id": "", "name": "", "imageSource": "", "mode": "Normal"});
            contextMenuModel.append({"id": "SetDescription", "name": "Set Description", "imageSource": "", "mode": "Normal"});
//            contextMenuModel.append({"id": "Rename", "name": "Rename", "imageSource": "", "mode": "Normal"});
        }
    }

    function getMenuButtonsX() {
        console.log("PackageView getMenuButtonsX");
        return featureCollectionViewContainer.contextMenuX + 75;
    }

    function getMenuButtonsY() {
        console.log("PackageView getMenuButtonsY");
        return featureCollectionViewContainer.contextMenuY + 132;
    }

    function getDescriptionBySelectedItem(){
        var dataModelLocal = featureCollectionView.collectionViewModel.GetData("data");
        var description = dataModelLocal.GetData("Description", featureCollectionView.table.selectedIndex);
        return description;
    }

    function openContextMenu(item, mouseX, mouseY) {
        var point = featureCollectionViewContainer.mapToItem(item, mouseX, mouseY);

        contextMenuX = point.x;
        contextMenuY = point.y;

        var source = "AuxComponents/PopupMenuDialog.qml";
        var parameters = {};
        parameters["model"] = contextMenuModel;
        parameters["backgroundOpacity"] = 0;
        parameters["resultItem"] = featureCollectionViewContainer;
        parameters["itemHeight"] = 25;
        parameters["itemWidth"] = 150;
        parameters["styleColor"] = Style.theme == "Dark" ? "Light" : "Dark";
        thubnailDecoratorContainer.openDialog(source, parameters);
    }

    function alreadyExistIdHasEmpty() {
        var dataModelLocal = featureCollectionView.collectionViewModel.GetData("data")
        for (var i = 0; i < dataModelLocal.GetItemsCount(); i++) {
            if (dataModelLocal.GetData("Id", i) === "#") {
                return dataModelLocal.GetData("Name", i);
            }
        }
        return "";
    }

    function dialogResult(parameters) {
         console.log("PackageView dialogResult", parameters["status"]);

        if (parameters["status"] === "ok") {

            if (parameters["dialog"] === "EditFeature") {
                var dataModelLocal = featureCollectionView.collectionViewModel.GetData("data");
                console.log("PackageView onClicked ", dataModelLocal.GetItemsCount())
                dataModelLocal.SetData("Id", parameters["newFeatureId"] , featureCollectionView.selectedIndex);
                dataModelLocal.SetData("Name", parameters["newFeatureName"], featureCollectionView.selectedIndex);
                dataModelLocal.SetData("Description", "", featureCollectionView.selectedIndex);
                featureCollectionView.collectionViewModel.SetData("data", dataModelLocal);
                featureCollectionView.refresh();

                featuresTreeView.addFeatureInTreeViewModel(featureCollectionViewContainer.itemId,
                                                           parameters["newFeatureId"],
                                                           parameters["newFeatureName"]);
            }
            else if (parameters["dialog"] === "InputDialog") {
                if (parameters["typeOperation"] === "Save") {
                    var value = parameters["value"];
                    console.log("featureCollectionViewContainer dialogResult", value);
                    saveModelPackage.updateModel(value);
                }
                else if (parameters["typeOperation"] === "SetDescription") {
                    var value = parameters["value"];
                    var dataModelLocal = featureCollectionView.collectionViewModel.GetData("data");

                    dataModelLocal.SetData("Description", value, featureCollectionView.table.selectedIndex);
                    featureCollectionView.collectionViewModel.SetData("data", dataModelLocal);

                    featureCollectionView.refresh();
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

//                featuresTreeView.removeDependsFeature(featureCollectionViewContainer.itemId, featureCollectionView.table.getSelectedId());
                featuresTreeView.removeFeatureInTreeViewModel(featureCollectionViewContainer.itemId, featureCollectionView.table.getSelectedId());
            }
        }
        else if (parameters["status"] === "Edit") {
//            var dataModelLocal = featureCollectionView.collectionViewModel.GetData("data");
//            var id = dataModelLocal.GetData("Id", featureCollectionView.table.selectedIndex);
//            var name = dataModelLocal.GetData("Id", featureCollectionView.table.selectedIndex);
//            featureCollectionView.selectItem(id, name);
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

    function menuActivated(menuId) {
        console.log("PackageView menuActivated", menuId);
        if (menuId  === "New"){
            //var countItems = model.GetData("data").GetItemsCount();

            var dataModelLocal = model.GetData("data");
            var index = dataModelLocal.InsertNewItem();

            dataModelLocal.SetData("Name", "Feature Name", index);//
            dataModelLocal.SetData("Id", "", index);//

            model.SetData("data", dataModelLocal);
            model.Refresh();
            featureCollectionView.refresh();
        }
        else if (menuId  === "Save") {
            console.log("PackageView operation save", featureCollectionViewContainer.itemId);
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
                var emptyId = featureCollectionViewContainer.alreadyExistIdHasEmpty();
                if (emptyId !== "") {
                    featureCollectionViewContainer.openMessageDialog(emptyId + " has an empty id !", "ErrorDialog");
                    return;
                }

                saveModelPackage.updateModel()
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
//            docsDataDeleg.closeTab();
            featureCollectionViewContainer.rootItem.closeTab();
           //thubnailDecoratorContainer.openDialog(source, parameters);
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
        //featureCollectionView.refresh();
        if (featureCollectionView.table.selectedIndex > -1) {
            featureCollectionViewContainer.rootItem.setModeMenuButton("Remove", "Normal");
            featureCollectionViewContainer.rootItem.setModeMenuButton("Edit", "Normal");
            featureCollectionViewContainer.rootItem.setModeMenuButton("Import", "Normal");
            featureCollectionViewContainer.rootItem.setModeMenuButton("Export", "Normal");
            featureCollectionViewContainer.rootItem.setModeMenuButton("Save", "Normal");
            featureCollectionViewContainer.rootItem.setModeMenuButton("Close", "Normal");
        } else {
            featureCollectionViewContainer.rootItem.setModeMenuButton("Remove", "Disabled");
            featureCollectionViewContainer.rootItem.setModeMenuButton("Edit", "Disabled");
            featureCollectionViewContainer.rootItem.setModeMenuButton("Export", "Disabled");
            featureCollectionViewContainer.rootItem.setModeMenuButton("Save", "Disabled");
        }
    }

    TreeItemModel {
        id: packageModel;
    }

    CollectionView {
        id: featureCollectionView;
        anchors.left: parent.left;
        anchors.right: packageMetaInfo.left;
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

        onSelectItem: {
            console.log("PackageView CollectionView onSelectItem", selectedId, name);

            var source = "../imtlicgui/EditFeatureDialog.qml";
            var parameters = {};
            parameters["featureId"] = selectedId;
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
//                multiDocViewItem.setItemSelectedIndex(featureCollectionView.selectedIndex);
            } else {
                treeView.visible = false;
            }

            featureCollectionViewContainer.updateFeaturesTreeView();
        }
    }

    function updateFeaturesTreeView(){
        console.log("PackageView updateFeaturesTreeView");
        //treeView.modelItems.Refresh();
//        featureCollectionViewContainer.hideCurrentFeatureTreeView();
//        featureCollectionViewContainer.updateStateCheckedCheckBox();
//        featureCollectionViewContainer.checkInActiveItems();
//        treeView.modelItems.Refresh();
    }

    function clearCheckedCheckBox() {
        var modelItems = treeView.modelItems;

        for (var i = 0; i < modelItems.GetItemsCount(); i++) {
            var childModel = modelItems.GetData("childItemModel", i);

            for (var j = 0; j < childModel.GetItemsCount(); j++) {

                if (childModel.GetData("stateChecked", j) === 2) {
                    childModel.SetData("stateChecked", 0, j);
                }
            }

            modelItems.SetData("childItemModel", childModel, i);
        }

        treeView.modelItems = modelItems;
        console.log("PackageView clearCheckedCheckBox", treeView.modelItems);
//        printModelItems(treeView.modelItems);
    }

    function printModelItems(modelItems) {
        //var modelItems = treeView.modelItems;

        for (var i = 0; i < modelItems.GetItemsCount(); i++) {
            var childModel = modelItems.GetData("childItemModel", i);
            console.log("Package Id", modelItems.GetData("Id", i));

            if (childModel) {
                for (var j = 0; j < childModel.GetItemsCount(); j++) {
                    console.log("\tFeature id ", childModel.GetData("Id", j));
                    console.log("\tstateChecked ", childModel.GetData("stateChecked", j));
                    console.log("\tisActive ", childModel.GetData("isActive", j));
                }
            }
        }
    }

    function checkInActiveItems() {
        console.log("PackageView checkInActiveItems");

        var curFeatureId = featureCollectionView.table.getSelectedId();
        var curPackageId = featureCollectionView.itemId;
        var modelItems = treeView.modelItems;

        if (!modelItems){
            return;
        }

        var dependsFeatures = featuresTreeView.findInAllRootFeaturesDependFeatureById(curPackageId, curFeatureId);

        for (var i = 0; i < modelItems.GetItemsCount(); i++) {
            var packageId = modelItems.GetData("Id", i);
            var childModel = modelItems.GetData("childItemModel", i);
            console.log("Current Package Id", packageId);
            if (!childModel) {
                continue;
            }

            for (var j = 0; j < childModel.GetItemsCount(); j++) {

                var id = childModel.GetData("Id", j);
                childModel.SetData("isActive", 1, j);
                for (var k = 0; k < dependsFeatures.length; k++) {
                    var data = dependsFeatures[k].split(".");
                    var pId = data[0];
                    var fId = data[1];

                    if (pId === packageId && fId === id) {
                        childModel.SetData("isActive", 0, j);
                    }
                }
            }
            modelItems.SetData("childItemModel", childModel, i);
        }
        treeView.modelItems = modelItems;
        //printModelItems(treeView.modelItems);
    }

    function updateStateCheckedCheckBox() {
        console.log("PackageView updateStateCheckedCheckBox", featuresTreeView);

        var curFeatureId = featureCollectionView.table.getSelectedId();
        var curPackageId = featureCollectionView.itemId;

        var modelItems = treeView.modelItems;
        var rootIndex = featuresTreeView.getIndexByRootFeatureId(curFeatureId);
       // console.log("Current feature Id", curFeatureId, rootIndex);

        if (!modelItems){
            return;
        }

        if (rootIndex !== -1) {
            //var packagesModel =  featuresTreeView.dependModel.GetData("Packages", rootIndex);

            for (var i = 0; i < modelItems.GetItemsCount(); i++) {
                var packageId = modelItems.GetData("Id", i);
                var childModel = modelItems.GetData("childItemModel", i);

               // console.log("\tPackageId", packageId);

                var packageIndex = featuresTreeView.getIndexByPackageId(rootIndex, packageId);

                if (!childModel) {
                    continue;
                }

                for (var j = 0; j < childModel.GetItemsCount(); j++) {

                    var id = childModel.GetData("Id", j);
                   // console.log("\t\tFeature Id", id);
                    if (featuresTreeView.dependModePackageGetIndexByFeatureId(rootIndex, packageIndex, id) !== -1) {
                        childModel.SetData("stateChecked", 2, j);
                        // console.log("\t\t\tState", 2);
                    } else {
                        childModel.SetData("stateChecked", 0, j);
                       // console.log("\t\t\tState", 0);
                    }
                }
                modelItems.SetData("childItemModel", childModel, i);
//                treeView.modelItems = modelItems;

            }
            treeView.modelItems = modelItems;
            //printModelItems(treeView.modelItems);
        } else {
            featureCollectionViewContainer.clearCheckedCheckBox();
        }
    }

    function hideCurrentFeatureTreeView() {
        console.log("PackageView hideCurrentFeatureTreeView");
        var modelItems = treeView.modelItems;

        if (!modelItems){
            return;
        }

        for (var i = 0; i < modelItems.GetItemsCount(); i++) {
            if (modelItems.GetData("Name", i) === featureCollectionViewContainer.itemId) {
                var childModelItems = modelItems.GetData("childItemModel", i);
                for (var j = 0; j < childModelItems.GetItemsCount(); j++) {
                    if (childModelItems.GetData("visible", j) === 0) {
                        childModelItems.SetData("visible", 1, j);
                    }

                    if (childModelItems.GetData("Id", j) === featureCollectionView.table.getSelectedId()) {
                        childModelItems.SetData("visible", 0, j);
                    }
                }
                modelItems.SetData("childItemModel", childModelItems, i);
                treeView.modelItems = modelItems;
                //printModelItems(treeView.modelItems);
                break;
            }
        }
    }

    GqlModel {
        id: saveModelPackage;

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
                packageModel.SetData("Id", newId);
                packageModel.SetData("Name", newId);
            } else {
                packageModel.SetData("Id", featureCollectionViewContainer.itemId );
                packageModel.SetData("Name", featureCollectionViewContainer.itemName);
            }

            packageModel.SetExternTreeModel("features", featureCollectionView.collectionViewModel.GetData("data"));
            packageModel.SetExternTreeModel("dependencies", featuresTreeView.dependModel);

            //featureCollectionViewContainer.model.SetIsArray(false);
            var jsonString = packageModel.toJSON();
            console.log("jsonString", jsonString)
            jsonString = jsonString.replace(/\"/g,"\\\\\\\"")
            console.log("jsonString", jsonString)

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
            console.log("State:", this.state, saveModelPackage);
            if (this.state === "Ready"){
                var dataModelLocal;
                if (saveModelPackage.ContainsKey("errors")){
                    dataModelLocal = saveModelPackage.GetData("errors");

                    dataModelLocal = dataModelLocal.GetData(featureCollectionViewContainer.gqlModelQueryType);
                    if (dataModelLocal){
                        console.log("Message errors");
                        var messageError = dataModelLocal.GetData("message");
                        featureCollectionViewContainer.openMessageDialog("Error Dialog", messageError);
                    }
                    return;
                }

                if (saveModelPackage.ContainsKey("data")){
                    dataModelLocal = saveModelPackage.GetData("data");

                    dataModelLocal = dataModelLocal.GetData(featureCollectionViewContainer.gqlModelQueryType);

                    if (dataModelLocal && dataModelLocal.ContainsKey(featureCollectionViewContainer.gqlModelQueryTypeNotification)){
                        dataModelLocal = dataModelLocal.GetData(featureCollectionViewContainer.gqlModelQueryTypeNotification);


                        var newId = dataModelLocal.GetData("Id");

                        featureCollectionViewContainer.itemId = newId;
                        featureCollectionViewContainer.itemName = newId;
                        console.log("PackageView updateTitleTab", newId);
                        featureCollectionViewContainer.rootItem.updateTitleTab(featureCollectionViewContainer.itemId, newId);
                    }
                }

            }
        }
    }

    Rectangle {
        id: packageMetaInfo;
        anchors.right: parent.right;
        height: parent.height;
        width: 300;
        color: "transparent";

        Rectangle {
            id: leftBorder;



            anchors.right: packageMetaInfo.left;
            anchors.top: packageMetaInfo.top;

            height: packageMetaInfo.height;
            width: 4;

            color: Style.backgroundColor;

            MouseArea {
                id: leftBorderMA;

                anchors.fill: parent;

                drag.target: featureCollectionViewContainer;

                acceptedButtons: Qt.LeftButton;
                cursorShape: Qt.SplitHCursor;

                onMouseXChanged: {
                    if(drag.active){

                        packageMetaInfo.width = packageMetaInfo.width - mouseX
//                        packageMetaInfo.x = packageMetaInfo.x + mouseX

                        if (packageMetaInfo.width <= titleHeader.width){
                            packageMetaInfo.width = 0;
                        }

                        if (featureCollectionViewContainer.width - packageMetaInfo.width <= 250){
                            packageMetaInfo.width = featureCollectionViewContainer.width - 250;
                        }

//                        if (packageMetaInfo.width > featureCollectionView.width && packageMetaInfo.width - featureCollectionView.width >= 800){
//                            packageMetaInfo.width = packageMetaInfo.width - featureCollectionView.width;
//                        }
                    }
                }
            }
        }

        Rectangle {
            id: headerTreeView;

            anchors.top: parent.top;

            width: parent.width;
            height: 35;

            color: Style.theme === "Light" ? "white": Style.backgroundColor;

            Text {
                id: titleHeader;

                anchors.verticalCenter: headerTreeView.verticalCenter;

                anchors.left: headerTreeView.left;
                anchors.leftMargin: 10;

                text: "Features";
                font.family: Style.fontFamilyBold;
                color: Style.textColor;
                font.pixelSize: Style.fontSize_small;
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

//            modelItems: featuresTreeView.modelTreeView;

            visible: false;

            clip: true;

            onItemTreeViewCheckBoxStateChanged: {
                console.log("PackageView TreeView onItemTreeViewCheckBoxStateChanged", state, packageId, featureId);

                var curFeatureId = featureCollectionView.table.getSelectedId();
                var curPackageId = featureCollectionView.itemId;

                var rootIndex = featuresTreeView.getIndexByRootFeatureId(curFeatureId);

                if (rootIndex === -1) {

                    rootIndex = featuresTreeView.dependModel.InsertNewItem();
                    featuresTreeView.dependModel.SetData("RootFeatureId", curFeatureId, rootIndex);
                    featuresTreeView.dependModel.SetData("RootPackageId", curPackageId, rootIndex);
                }

                var packageModel;
                if (featuresTreeView.rootFeatureIdHasDependPackage(rootIndex)) {
                    packageModel = featuresTreeView.dependModel.GetData("Packages", rootIndex);
                    //console.log("packages exist", rootIndex);
                }
                else {
                  //  console.log("packages not exist", rootIndex);
                    packageModel = featuresTreeView.dependModel.AddTreeModel("Packages", rootIndex);
                }

                var packageIndex = featuresTreeView.getPackageIndexByPackageId(packageId, rootIndex);

                if (packageIndex === -1) {
                    packageIndex = featuresTreeView.addNewPackageToRootFeatureByRootIndex(packageId, rootIndex);
                }

                var childModel;
                if (featuresTreeView.dependModelPackageHasChild(rootIndex, packageIndex)) {
                    childModel = packageModel.GetData("childItemModel", packageIndex);
                }
                else {
                    childModel = packageModel.AddTreeModel("childItemModel", packageIndex);
                }

                var childIndex = featuresTreeView.dependModePackageGetIndexByFeatureId(rootIndex, packageIndex, featureId);

                if (childIndex === -1) {
                    if (state === 2) {
                        childIndex = childModel.InsertNewItem();
                        childModel.SetData("Id", featureId, childIndex);
                        childModel.SetData("stateChecked", state, childIndex);
                    }

                } else {
                    if (state === 0) {
                      //  console.log("Feature Id remove ", childIndex);
                        childModel.RemoveItem(childIndex);
                    }
                }

                packageModel.SetData("childItemsModel", childModel, packageIndex);

                if (childModel.GetItemsCount() === 0) {
                    featuresTreeView.dependModelRemovePackageByIndex(rootIndex, packageIndex);
                }

                featuresTreeView.dependModel.SetData("Packages", packageModel, rootIndex);

                if (packageModel.GetItemsCount() === 0) {
                    featuresTreeView.dependModelRemoveRootFeatureByIndex(rootIndex);
                }

//                featuresTreeView.printInfo();
            }
        }

        FeaturesTreeView {
            id: featuresTreeView;
            packageItem: featureCollectionViewContainer;

            onDependModelChanged: {
                console.log( "PackageView FeaturesTreeView onDependModelChanged");
                featureCollectionViewContainer.updateFeaturesTreeView();
            }

            onModelTreeViewChanged: {
                console.log("PackageView FeaturesTreeView onModelTreeViewChanged");
                //featureCollectionViewContainer.updateFeaturesTreeView();

                treeView.modelItems = featuresTreeView.modelTreeView;
            }
        }
    }
}
