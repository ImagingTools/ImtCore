import QtQuick 2.15
import Acf 1.0
import imtqml 1.0

Item {
    id: container;

    property Item root;

    property TreeItemModel serverModel: null;
    property TreeItemModel localModel: null;

    signal serverSettingsSaved();
    signal localSettingsSaved();

    onServerModelChanged: {
        console.log("SettingsProvider onServerModelChanged", container.serverModel);

        container.rewriteModel(container.serverModel, container.localModel);

        container.saveLocalModel();
    }

    function clearModel(){
        container.serverModel.Clear();
    }

    function updateModel(){
        settingsQuery.getSettings();
    }

    function saveLocalModel(){
        console.log("SettingsProvider saveLocalModel", container.localModel.toJSON());

        container.root.settingsUpdate();

        container.localSettingsSaved();
    }

    function saveServerModel(){
        console.log("SettingsProvider saveServerModel", container.serverModel.toJSON());

        preferenceSaveQuery.save();
    }

    /**
        The server model rewrite pages from the local model.
    */
    function rewriteModel(fromModel, toModel){
        for (let i = 0; i < toModel.GetItemsCount(); i++){
            let pageId = toModel.GetData("Id", i);

            let index = -1;
            for (let j = 0; j < fromModel.GetItemsCount(); j++){
                let id = fromModel.GetData("Id", j);
                if (id == pageId){
                    index = j;
                    break;
                }
            }

            if (index >= 0){
                toModel.CopyItemDataFromModel(i, fromModel, index);
            }
        }
    }

    GqlModel {
        id: settingsQuery;

        function getSettings() {
            var query = Gql.GqlRequest("query", "GetSettings");

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

                    if (dataModelLocal.ContainsKey("GetSettings")){
                        dataModelLocal = dataModelLocal.GetData("GetSettings");

                        container.serverModel = dataModelLocal;
                    }
                }
            }
        }
    }//GetSettings

    GqlModel {
        id: preferenceSaveQuery;

        function save(){
            var query = Gql.GqlRequest("mutation", "SetSettings");

            var inputParams = Gql.GqlObject("input");
            var jsonString = container.serverModel.toJSON();
            jsonString = jsonString.replace(/\"/g,"\\\\\\\"");

            inputParams.InsertField("Item", jsonString);
            query.AddParam(inputParams);

            var gqlData = query.GetQuery();

            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, preferenceSaveQuery);
            if (this.state === "Ready") {

                if (this.ContainsKey("errors")){
                    return;
                }

                container.serverSettingsSaved();
            }
        }
    }//SaveSettings
}
