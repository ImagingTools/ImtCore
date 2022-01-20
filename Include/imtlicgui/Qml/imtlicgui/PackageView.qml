import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0
//import imtlicgui 1.0

Item {
    id: featureCollectionViewContainer;
    anchors.fill: parent;
    property alias itemId: featureCollectionView.itemId;
    property alias itemName: featureCollectionView.itemName;
    property alias model: featureCollectionView.model;

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
                var value = parameters["value"];
                console.log("featureCollectionViewContainer dialogResult", value);
                docsDataDeleg.updateTitleTab(value);
                saveModel.updateModel(value);
            }
        }
        else if (parameters["status"] === "yes") {

            if (featureCollectionView.model.ContainsKey("data")) {
                var dataModelLocal = featureCollectionView.model.GetData("data");
                dataModelLocal.RemoveItem(featureCollectionView.table.selectedIndex);

                featureCollectionView.model.SetData("data", dataModelLocal);
                featureCollectionView.model.Refresh();
                featureCollectionView.refresh();
            }
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
                parameters["resultItem"] = featureCollectionViewContainer;
                thubnailDecoratorContainer.openDialog(source, parameters);
            }
            else {
                saveModel.updateModel()
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
            docsDataDeleg.closeTab();
           //thubnailDecoratorContainer.openDialog(source, parameters);
        }
        else {
           featureCollectionView.menuActivated(menuId)
        }
    }

    function commandsChanged(commandsId){
        console.log("PackageView commandsChanged", commandsId);
        if (commandsId !== "PackageEdit") {
            return;
        }

        if (featureCollectionView.selectedIndex > -1) {
            docsDataDeleg.setModeMenuButton("Remove", "Normal");
            docsDataDeleg.setModeMenuButton("Edit", "Normal");
            docsDataDeleg.setModeMenuButton("Import", "Normal");
            docsDataDeleg.setModeMenuButton("Export", "Normal");
            docsDataDeleg.setModeMenuButton("Save", "Normal");
            docsDataDeleg.setModeMenuButton("Close", "Normal");
        } else {
            docsDataDeleg.setModeMenuButton("Remove", "Disabled");
            docsDataDeleg.setModeMenuButton("Edit", "Disabled");
            //docsDataDeleg.setModeMenuButton("Import", "Disabled");
            docsDataDeleg.setModeMenuButton("Export", "Disabled");
            docsDataDeleg.setModeMenuButton("Save", "Disabled");
            //docsDataDeleg.setModeMenuButton("Close", "Disabled");
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

        onSelectItem: {
            console.log("PackageView CollectionView onSelectItem", itemId, name);

            var source = "../imtlicgui/EditFeatureDialog.qml";
            var parameters = {};
            parameters["featureId"] = itemId;
            parameters["featureName"] = name;
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
        id: saveModel;

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
            console.log("State:", this.state, saveModel);
            if (this.state === "Ready"){

                var dataModelLocal = model.GetData("data");
                if(dataModelLocal.ContainsKey("addedNotification")){
                    dataModelLocal = dataModelLocal.GetData("addedNotification");
                                            featureCollectionView.refresh();
                    if(dataModelLocal !== null && dataModelLocal.ContainsKey("Id") && featureCollectionViewContainer.itemId === ""){
                        featureCollectionViewContainer.itemId = dataModelLocal.GetData("Id");
                    }
                    else if(saveModel.ContainsKey("errors")){
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
        width: 150;
        color: "transparent";
    }
}
