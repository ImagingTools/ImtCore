import QtQuick 2.12
import Acf 1.0


CollectionView {
    width: 100;
    height: 50;
//    color: "red";
    Component.onCompleted: {
        tabPanel.addToHeadersArray("Packages")
        table.addToHeadersArray("Name")
        table.addToHeadersArray("Description")
        table.addToHeadersArray("Added")
        table.addToHeadersArray("Last Modified")
    }


    GqlModel {
        id: commandsModel;

        function updateModel() {
            console.log( "updateModel");

            var query = Gql.GqlRequest("query", "FeaturePackageList");

            var queryFields = Gql.GqlObject("items");
            query.AddField(queryFields);

            var queryFields = Gql.GqlObject("ListCommanda");
            queryFields.InsertField(CommandEnum.ID);
            queryFields.InsertField(CommandEnum.NAME);
            queryFields.InsertField(CommandEnum.ICON);
            query.AddField(queryFields);

            var queryFields = Gql.GqlObject("EditCommands");
            queryFields.InsertField(CommandEnum.ID);
            queryFields.InsertField(CommandEnum.NAME);
            queryFields.InsertField(CommandEnum.ICON);
            query.AddField(queryFields);

            var gqlData = query.GetQuery();
//            console.log(gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, commandsModel);
            if (this.state === "Ready"){
                var dataModelLocal = this.GetData("data");
                if(dataModelLocal.ContainsKey("CommandsData")){
                    dataModelLocal = dataModelLocal.GetData("CommandsData");
                    if(dataModelLocal !== null && dataModelLocal.ContainsKey("items")){
                        dataModelLocal = dataModelLocal.GetData("items");
//                        console.log("items",dataModelLocal);
                        lvButtons.model = dataModelLocal;
                    }
                    else if(commandsModel.ContainsKey("errors")){
                        var errorsModel = commandsModel.GetData("errors");
                        if(errorsModel !== null && errorsModel.ContainsKey("CommandsData")){
                            console.log("message", errorsModel.GetData("CommandsData").GetData("message"));
                        }
                    }
                }
            }
        }
    }

}
