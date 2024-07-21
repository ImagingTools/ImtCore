import QtQuick 2.12
import Acf 1.0
import imtguigql 1.0
import imtgui 1.0
import imtcontrols 1.0

QtObject {
    id: container;

    property TableViewParams tableViewParams: TableViewParams {}
    property string tableId: "";

    signal updated();

    function getModel(){
        if (tableId !== ""){
            getModelQuery.get();
        }
    }

    function saveModel(){
        saveModelQuery.set();
    }

    function clearModel(){
        if (container.tableViewParams){
            container.tableViewParams.clear();
        }
    }

    property GqlModel getModelQuery: GqlModel {
        function get(){
            var query = Gql.GqlRequest("query", "GetTableViewParams");

            var inputParams = Gql.GqlObject("input");
            inputParams.InsertField("TableId", container.tableId);
            query.AddParam(inputParams);

            var gqlData = query.GetQuery();
            this.setGqlQuery(gqlData);
        }

        onStateChanged: {
            if (this.state === "Ready") {
                console.log("GetTableViewParams", this.toJson());
                var dataModelLocal;

                if (container.getModelQuery.containsKey("errors")){
                    dataModelLocal = container.getModelQuery.getData("errors");

                    return;
                }

                if (container.getModelQuery.containsKey("data")){
                    dataModelLocal = container.getModelQuery.getData("data");

                    if (dataModelLocal.containsKey("GetTableViewParams")){
                        dataModelLocal = dataModelLocal.getData("GetTableViewParams");
                        container.tableViewParams.copyFromTreeModel(dataModelLocal);
                        container.updated();
                    }
                }
            }
        }
    }//GetSettings

    property GqlModel saveModelQuery: GqlModel {
        function set(){
            var query = Gql.GqlRequest("mutation", "SetTableViewParams");

            var inputParams = Gql.GqlObject("input");
            inputParams.InsertField("TableId", container.tableId);
            inputParams.InsertField("TableViewParams", container.tableViewParams.toJson());
            query.AddParam(inputParams);

            this.setGqlQuery(query.GetQuery());
        }

        onStateChanged: {
            if (this.state === "Ready") {
                if (this.containsKey("errors")){
                    return;
                }
            }
        }
    }//SaveSettings
}


