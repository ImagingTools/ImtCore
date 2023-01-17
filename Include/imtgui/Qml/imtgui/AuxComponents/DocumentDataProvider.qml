import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

Item {
    id: container;

    property TreeItemModel documentModel: TreeItemModel {};

    property string commandId;

    function updateModel(externInputParams){
        if (container.commandId == ""){
            console.error( "ERROR: DocumentDataProvider commandId is empty!");

            return;
        }

        itemModel.updateModel(externInputParams);
    }

    GqlModel {
        id: itemModel;

        function updateModel(externInputParams) {
            console.log("itemModel updateModel", container.commandId + "Item");
            var query = Gql.GqlRequest("query", container.commandId + "Item");

            var inputParams = Gql.GqlObject("input");

            let keys = Object.keys(externInputParams)
            for (let key of keys){
                inputParams.InsertField(key, externInputParams[key]);
            }

            query.AddParam(inputParams);

            var queryFields = Gql.GqlObject("item");
            queryFields.InsertField("Id");
            queryFields.InsertField("Name");
            query.AddField(queryFields);

            var gqlData = query.GetQuery();

            console.log("itemModel gqlData", gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, itemModel);
            if (this.state === "Ready"){
                var dataModelLocal;

                if (itemModel.ContainsKey("errors")){
//                    modalDialogManager.openDialog(inputDialog, {"message": qsTr("Please enter the name of the installation:")});
                    return;
                }

                dataModelLocal = itemModel.GetData("data");
                if(dataModelLocal.ContainsKey(commandId + "Item")){
                    dataModelLocal = dataModelLocal.GetData(container.commandId + "Item");
                    documentModel = dataModelLocal;
                }
            }
        }
    }//GqlModel itemModel
}
