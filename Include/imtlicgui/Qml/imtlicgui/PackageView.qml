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
    property alias model: featureCollectionView.model;

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
    }

    function openMessageDialog(source, parameters) {
        parameters["resultItem"] = featureCollectionViewContainer;
        thubnailDecoratorContainer.openDialog(source, parameters);
    }

    ListModel {
        id: contextMenuModel;

        Component.onCompleted: {
            contextMenuModel.append({"id": "Edit", "name": "Edit", "imageSource": "../../../Icons/Light/Edit_On_Normal.svg", "mode": "Normal"});
            contextMenuModel.append({"id": "Remove", "name": "Remove", "imageSource": "../../../Icons/Light/Remove_On_Normal.svg", "mode": "Normal"});
            contextMenuModel.append({"id": "", "name": "", "imageSource": "", "mode": "Normal"});
            contextMenuModel.append({"id": "SetDescription", "name": "Set Description...", "imageSource": "", "mode": "Normal"});
//            contextMenuModel.append({"id": "Rename", "name": "Rename", "imageSource": "", "mode": "Normal"});
        }
    }

    function getMenuButtonsX() {
        return featureCollectionViewContainer.contextMenuX + 75;
    }

    function getMenuButtonsY() {
        return featureCollectionViewContainer.contextMenuY + 132;
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
        var dataModelLocal = featureCollectionView.model.GetData("data")
        for (var i = 0; i < dataModelLocal.GetItemsCount(); i++) {
            if (dataModelLocal.GetData("Id", i) === "") {
                return dataModelLocal.GetData("Name", i);
            }
        }
        return "";
    }

    function dialogResult(parameters) {
         console.log("PackageView dialogResult", parameters["status"]);

        if (parameters["status"] === "ok") {

            if (parameters["dialog"] === "EditFeature") {
                var dataModelLocal = featureCollectionView.model.GetData("data");
                console.log("PackageView onClicked ", dataModelLocal.GetItemsCount())
                dataModelLocal.SetData("Id", parameters["newFeatureId"] , featureCollectionView.selectedIndex);//
                dataModelLocal.SetData("Name", parameters["newFeatureName"], featureCollectionView.selectedIndex);
                featureCollectionView.model.SetData("data", dataModelLocal);
                featureCollectionView.refresh();
            }
            else if (parameters["dialog"] === "InputDialog") {
                if (parameters["typeOperation"] === "Save") {
                    var value = parameters["value"];
                    console.log("featureCollectionViewContainer dialogResult", value);
                    featureCollectionViewContainer.rootItem.updateTitleTab(featureCollectionViewContainer.itemId, value);
                    saveModelPackage.updateModel(value);
                }
                else if (parameters["typeOperation"] === "SetDescription") {
                    var value = parameters["value"];
                    var dataModelLocal = featureCollectionView.model.GetData("data");

                    dataModelLocal.SetData("Description", value, featureCollectionView.table.selectedIndex);
                    featureCollectionView.model.SetData("data", dataModelLocal);

                    featureCollectionView.refresh();
                }
            }
        }
        else if (parameters["status"] === "yes") {

            if (featureCollectionView.model.ContainsKey("data")) {
                var dataModelLocal = featureCollectionView.model.GetData("data");
                dataModelLocal.RemoveItem(featureCollectionView.table.selectedIndex);
                featureCollectionView.model.SetData("data", dataModelLocal);
                featureCollectionView.model.Refresh();
                featureCollectionView.refresh();

                featureCollectionView.table.selectedIndex = -1;
                featureCollectionViewContainer.commandsChanged("PackageEdit");
            }
        }
        else if (parameters["status"] === "Edit") {
//            var dataModelLocal = featureCollectionView.model.GetData("data");
//            var id = dataModelLocal.GetData("Id", featureCollectionView.table.selectedIndex);
//            var name = dataModelLocal.GetData("Id", featureCollectionView.table.selectedIndex);
//            featureCollectionView.selectItem(id, name);
            featureCollectionViewContainer.menuActivated("Edit");
        }
        else if (parameters["status"] === "Remove") {
            featureCollectionViewContainer.menuActivated("Remove");
        }
        else if (parameters["status"] === "Set Description...") {
            var source = "AuxComponents/InputDialog.qml";
            var parameters = {};
            parameters["message"] = "Please enter the description of the feature: ";
            parameters["nameDialog"] = "Set description";
            parameters["typeOperation"] = "SetDescription";
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

            if (featureCollectionView.itemId == "") {
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

                    var source = "AuxComponents/MessageDialog.qml";
                    var parameters = {};
                    parameters["message"] = emptyId + " has an empty id !";
                    parameters["noButtonVisible"] = false;
                    parameters["textOkButton"] = "Ok";
                    parameters["nameDialog"] = "ErrorDialog";

                    featureCollectionViewContainer.openMessageDialog(source, parameters);

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

    function commandsChanged(commandsId){
        console.log("PackageView commandsChanged", commandsId, featureCollectionViewContainer.rootItem);
        if (commandsId !== "PackageEdit") {
            return;
        }

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

        onRightButtonMouseClicked: {
            console.log("PackageView CollectionView AuxTable onRightButtonMouseClicked");
            featureCollectionViewContainer.openContextMenu(item, mouseX, mouseY);
        }

        onSelectItem: {
            console.log("PackageView CollectionView onSelectItem", itemId, name);

            var source = "../imtlicgui/EditFeatureDialog.qml";
            var parameters = {};
            parameters["featureId"] = itemId;
            parameters["featureName"] = name;
            parameters["collectionViewFeatures"] = featureCollectionView;
            parameters["resultItem"] = featureCollectionViewContainer;

            thubnailDecoratorContainer.openDialog(source, parameters);
        }

        onSelectedIndexChanged: {
            console.log("PackageView CollectionView onSelectedIndexChanged", featureCollectionView.selectedIndex);
            if (featureCollectionView.selectedIndex > -1){
                featureCollectionViewContainer.commandsChanged("PackageEdit")
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
                query = Gql.GqlRequest("query", "PackageUpdate");
                inputParams.InsertField("Id");
                inputParams.InsertFieldArgument("Id", featureCollectionViewContainer.itemId);
                queryFields = Gql.GqlObject("updatedNotification");
            }
            else{
                query = Gql.GqlRequest("query", "PackageAdd");
                queryFields = Gql.GqlObject("addedNotification");
            }
            query.AddParam(inputParams);

            if (newId !== undefined && newId !== "") {
                //console.log();
                featureCollectionViewContainer.itemId = newId;
                featureCollectionViewContainer.itemName = newId;
            }

            packageModel.SetData("Id", featureCollectionViewContainer.itemId)
            packageModel.SetData("Name", featureCollectionViewContainer.itemName)
            packageModel.SetExternTreeModel("features", featureCollectionView.model.GetData("data"));

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

                var dataModelLocal = model.GetData("data");
                if(dataModelLocal.ContainsKey("addedNotification")){
                    dataModelLocal = dataModelLocal.GetData("addedNotification");
                                            featureCollectionView.refresh();
                    if(dataModelLocal !== null && dataModelLocal.ContainsKey("Id") && featureCollectionViewContainer.itemId === ""){
                        featureCollectionViewContainer.itemId = dataModelLocal.GetData("Id");
                    }
                    else if(saveModelPackage.ContainsKey("errors")){
                        var errorsModel = accountItemModel.GetData("errors");
                        if(errorsModel !== null && errorsModel.ContainsKey(containerContactInfo.gqlModelItems)){
                            console.log("message", errorsModel.GetData(featureCollectionViewContainer.gqlModelItems).GetData("message"));
                        }
                    }
                }
            }
        }
    }

    Rectangle {
        id: packageMetaInfo;
        anchors.right: parent.right;
        height: parent.height;
        width: 200;
        color: "transparent";

        Rectangle {
            id: leftBorder;

            anchors.right: packageMetaInfo.left;
            anchors.top: packageMetaInfo.top;

            height: packageMetaInfo.height;
            width: 3;

            color: Style.backgroundColor;
        }

        Rectangle {
            id: headerTreeView;
            anchors.top: parent.top;
            width: parent.width;
            height: 35;
//            color: "transparent";
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
            height: 500;
            modelItems: featuresTreeView.model;
        }

        FeaturesTreeView {
            id: featuresTreeView;
        }
    }
}
