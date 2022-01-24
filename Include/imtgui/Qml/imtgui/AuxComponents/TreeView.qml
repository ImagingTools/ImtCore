import QtQuick 2.0
import Acf 1.0

Rectangle {
    id: container;
    color: Style.baseColor;

    Component.onCompleted: {
        var childItemModel;
        treeItemModel.SetIsArray(true);

        treeItemModel.InsertNewItem();
        treeItemModel.SetData("Name", "Feature 1");
        treeItemModel.SetData("stateChecked", 0);
        treeItemModel.SetData("level", 0);

        childItemModel = treeItemModel.AddTreeModel("childItemModel");
        childItemModel.SetData("Name", "A",0);
        childItemModel.SetData("stateChecked", 0, 0);
        childItemModel.SetData("level", 1, 0);

        childItemModel.InsertNewItem();
        childItemModel.SetData("Name", "B", 1);
        childItemModel.SetData("stateChecked", 0, 1);
        childItemModel.SetData("level", 1, 1);

        var index = treeItemModel.InsertNewItem();
        treeItemModel.SetData("Name", "Feature 2", index);
        treeItemModel.SetData("stateChecked", 0, index);
        treeItemModel.SetData("level", 0, index);

        childItemModel = treeItemModel.AddTreeModel("childItemModel", 1);
        childItemModel.SetData("Name", "A", 1);
        childItemModel.SetData("stateChecked", 0, 1);
        childItemModel.SetData("level", 1, 1);
        mainTreeView.model = treeItemModel;
    }

    TreeItemModel {
        id: treeItemModel;
    }

    ListView {
        id: mainTreeView;
        anchors.fill: container;
        delegate: TreeItemDelegate {
            width: parent.width;
        }
    }

    GqlModel {
        id: treeModel;

        function loadModel() {
            console.log( "treeModel updateModel");

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
            console.log("TreeView query ", gqlData);
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
}
