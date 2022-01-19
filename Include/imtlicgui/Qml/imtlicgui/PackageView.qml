import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

Item {
    id: featureCollectionViewContainer;
    anchors.fill: parent;
    property alias itemId: featureCollectionView.itemId;
    property alias itemName: featureCollectionView.itemName;
    property alias model: featureCollectionView.model;

    onItemIdChanged: {
        console.log("PackageView onItemIdChanged", featureCollectionViewContainer.itemId)
    }

    function refresh() {
        featureCollectionView.refresh();
    }

    function dialogResult(parameters) {
         console.log(parameters["status"]);

        if (parameters["status"] === "ok") {
            var value = parameters["value"];
            console.log("featureCollectionViewContainer dialogResult", value);

            saveModel.updateModel(value);
        }
    }

    function menuActivated(menuId) {
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
                parameters["message"] = "Enter name";
                parameters["nameDialog"] = "Input name";
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
            parameters["resultItem"] = collectionViewContainer;

            thubnailDecoratorContainer.openDialog(source, parameters);
        }
        else {
           featureCollectionView.menuActivated(menuId)
        }
    }

    function commandsChanged(commandsId){
        if (commandsId !== "PackageEdit") {
            return;
        }

        if (featureCollectionView.selectedIndex > -1) {
            docsDataDeleg.setModeMenuButton("Remove", "Active");
            docsDataDeleg.setModeMenuButton("Edit", "Active");
            docsDataDeleg.setModeMenuButton("Import", "Active");
            docsDataDeleg.setModeMenuButton("Export", "Active");
            docsDataDeleg.setModeMenuButton("Save", "Active");
            docsDataDeleg.setModeMenuButton("Close", "Active");
        } else {
            docsDataDeleg.setModeMenuButton("Remove", "Disabled");
            docsDataDeleg.setModeMenuButton("Edit", "Disabled");
            docsDataDeleg.setModeMenuButton("Import", "Disabled");
            docsDataDeleg.setModeMenuButton("Export", "Disabled");
            docsDataDeleg.setModeMenuButton("Save", "Disabled");
            docsDataDeleg.setModeMenuButton("Close", "Disabled");
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
            featureCollectionView.gqlModelRemove = "FeatureRemove";
        }

        onItemIdChanged: {
            console.log("PackageView CollectionView onItemIdChanged", featureCollectionView.itemId)
            if (featureCollectionView.itemId){
                featureCollectionView.gqlModelInfo = "PackageInfo"
                featureCollectionView.gqlModelItems = "FeatureList"
                featureCollectionView.gqlModelRemove = "FeatureRemove";
            }
        }

        onSelectItem: {
            console.log("PackageView CollectionView onSelectItem", itemId, name);
            editFeatureDialog.visible = true;
            editFeatureDialog.featureId = itemId;
            editFeatureDialog.featureName = name;
            console.log("featureCollectionView.itemId", featureCollectionView.itemId);
            console.log("featureCollectionView.itemName", featureCollectionView.itemName);
        }

        onSelectedIndexChanged: {
            console.log("featurePackageCollectionView onSelectedIndexChanged", featureCollectionView.selectedIndex);
            if (featureCollectionView.selectedIndex > -1){
                featureCollectionViewContainer.commandsChanged("PackageEdit")
            }
        }
    }

    MouseArea {
        id: maPackageView;
        anchors.fill: parent;
        visible: editFeatureDialog.visible;
    }

    EditFeatureDialog {
        id: editFeatureDialog;
        visible: false;

        anchors.verticalCenter: featureCollectionViewContainer.verticalCenter;
        anchors.horizontalCenter: featureCollectionViewContainer.horizontalCenter;

        onOkClicked: {
            var dataModelLocal = featureCollectionView.model.GetData("data");
            console.log("PackageView onClicked ", dataModelLocal.GetItemsCount())
            dataModelLocal.SetData("Id", newId, featureCollectionView.selectedIndex);//
            dataModelLocal.SetData("Name", newName, featureCollectionView.selectedIndex);


//            for (var i = 0; i < dataModelLocal.GetItemsCount(); i++) {
//                 console.log(dataModelLocal.GetData("FeatureId", i), featureCollectionView.itemId);
//                if (dataModelLocal.GetData("FeatureId", i) === featureCollectionView.itemId) {
//                    console.log("PackageView onClicked ", dataModelLocal.GetData("FeatureId", i), newId)
//                    dataModelLocal.SetData("FeatureId", newId, i);
//                    dataModelLocal.SetData("FeatureName", newName, i);
//                    break;
//                }
//            }
            featureCollectionView.model.SetData("data", dataModelLocal);
            featureCollectionView.refresh();
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

            if (newId !== "") {
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
