import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

QtObject {
    id: container;

    property TreeItemModel serverModel: null;
    property TreeItemModel localModel: null;

    property bool modelsCompleted: serverModel && localModel;

    signal serverSettingsSaved();
    signal localSettingsSaved();

    Component.onCompleted: {
        Events.subscribeEvent("UpdateSettings", container.updateModel);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("UpdateSettings", container.updateModel);
    }

    onServerModelChanged: {
        console.log("SettingsProvider onServerModelChanged", container.serverModel);
    }

    onLocalModelChanged: {
        console.log("SettingsProvider onLocalModelChanged", container.localModel);
    }

    onModelsCompletedChanged: {
        console.log("onModelsCompletedChanged", modelsCompleted);
        if(container.modelsCompleted){
            container.saveLocalModel();
        }
    }

    function clearModel(){
        if (container.serverModel){
            container.serverModel.Clear();
        }
    }

    function updateModel(){
        settingsQuery.getSettings();
    }

    function saveLocalModel(){
        console.log("SettingsProvider saveLocalModel", container.localModel.toJSON());
        container.rewriteModel(container.serverModel, container.localModel);

        container.localSettingsSaved();
    }

    function saveServerModel(){
        console.log("SettingsProvider saveServerModel", container.serverModel.toJSON());

        preferenceSaveQuery.save();

        saveLocalModel();
    }

    function rewriteModel(fromModel, toModel){
        for (let i = 0; i < toModel.GetItemsCount(); i++){
            let pageId = toModel.GetData("Id", i);
            let index = -1;
            for (let j = 0; j < fromModel.GetItemsCount(); j++){
                let id = fromModel.GetData("Id", j);
                if (id == pageId){
                    console.log("id === pageId", id);
                    index = j;

                    break;
                }
            }

            if (index >= 0){
                toModel.CopyItemDataFromModel(i, fromModel, index);
            }
        }
    }

    property GqlModel settingsQuery: GqlModel {
        function getSettings() {
            console.log("GetSettings");

            var query = Gql.GqlRequest("query", "GetSettings");

            var gqlData = query.GetQuery();

            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, container.settingsQuery);

            if (this.state === "Ready") {
                var dataModelLocal;

                if (container.settingsQuery.ContainsKey("errors")){
                    dataModelLocal = container.settingsQuery.GetData("errors");

                    return;
                }

                if (container.settingsQuery.ContainsKey("data")){
                    dataModelLocal = container.settingsQuery.GetData("data");

                    if (dataModelLocal.ContainsKey("GetSettings")){
                        dataModelLocal = dataModelLocal.GetData("GetSettings");

                        container.serverModel = dataModelLocal;
                    }
                }
            }
        }
    }//GetSettings

    property GqlModel preferenceSaveQuery: GqlModel {
        function save(){
            console.log("SetSettings");

            var query = Gql.GqlRequest("mutation", "SetSettings");

            var inputParams = Gql.GqlObject("input");
            var jsonString = container.serverModel.toJSON();

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
