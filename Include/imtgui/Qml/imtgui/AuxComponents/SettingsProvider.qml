import QtQuick 2.15
import Acf 1.0
import imtqml 1.0

Item {
    id: container;

    property Item root;

    property TreeItemModel serverModel: TreeItemModel {}
    property TreeItemModel localModel: TreeItemModel {}

    onServerModelChanged: {
        console.log("SettingsProvider onServerModelChanged", serverModel);
//        updateLocalModel();
//        applyDesignScheme();
    }

    onLocalModelChanged: {
        console.log("SettingsProvider onLocalModelChanged", localModel.toJSON());
        timer.start();
    }

    Timer {
        id: timer;

        interval: 100;

        onTriggered: {
//            container.applyDesignScheme();
        }
    }

    function clearModel(){
        serverModel.Clear();
    }

    function updateModel(){
        settingsQuery.getSettings();
    }

    function saveLocalModel(){
        console.log("SettingsProvider saveLocalModel");

        root.settingsUpdate();
    }

    function saveServerModel(pageIds){
        preferenceSaveQuery.save();
    }

    function applyDesignScheme(){
        let design = getDesignScheme();
        if (design){
            Style.getDesignScheme(design);
        }
    }

    /**
        The server model rewrite pages from the local model.
    */
    function updateLocalModel(){
        for (let i = 0; i < localModel.GetItemsCount(); i++){
            let pageId = localModel.GetData("Id", i);

            let index = -1;
            for (let j = 0; j < serverModel.GetItemsCount(); j++){
                let id = serverModel.GetData("Id", j);
                if (id == pageId){
                    index = j;
                    break;
                }
            }

            if (index >= 0){
                localModel.CopyItemDataFromModel(i, serverModel, index);
            }
        }
    }

    GqlModel {
        id: settingsQuery;

        function getSettings() {
            var query = Gql.GqlRequest("query", "Get");

            var queryFields = Gql.GqlObject("ModelIds");
            queryFields.InsertField("Settings");
            query.AddField(queryFields);

            var gqlData = query.GetQuery();

            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, settingsQuery);

            if (this.state === "Ready") {
                var dataModelLocal;

                if (settingsQuery.ContainsKey("errors")){
                    dataModelLocal = settingsQuery.GetData("errors");

                    return;
                }

                if (settingsQuery.ContainsKey("data")){
                    dataModelLocal = settingsQuery.GetData("data");

                    if (dataModelLocal.ContainsKey("Get")){
                        dataModelLocal = dataModelLocal.GetData("Get");

                        container.serverModel = dataModelLocal;
                    }
                }
            }
        }
    }//GetSettings

    GqlModel {
        id: preferenceSaveQuery;

        function save(){
            var query = Gql.GqlRequest("mutation", "SaveSettings");

            var inputParams = Gql.GqlObject("input");
            var jsonString = container.serverModel.toJSON();
            jsonString = jsonString.replace(/\"/g,"\\\\\\\"")

            inputParams.InsertField ("Item", jsonString);
            query.AddParam(inputParams);

            var queryFields = Gql.GqlObject("Save");
            queryFields.InsertField("Id");
            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            console.log("Preference GqlModel SaveSettings query ", gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, preferenceSaveQuery);
            if (this.state === "Ready") {

                if (this.ContainsKey("errors")){
                    return;
                }
            }
        }
    }//SaveSettings
}
