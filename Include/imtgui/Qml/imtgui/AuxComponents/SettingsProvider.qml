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
        updateLocalModel();
        applyDesignScheme();
    }

    onLocalModelChanged: {
        console.log("SettingsProvider onLocalModelChanged", localModel);
        timer.start();
    }

    Timer {
        id: timer;

        interval: 100;

        onTriggered: {
            container.applyDesignScheme();
        }
    }

    function clearModel(){
        serverModel.Clear();
    }

    function updateModel(){
        settingsQuery.getSettings();
    }

    function saveLocalModel(pageIds){
        console.log("SettingsProvider saveLocalModel", pageIds);
        for (let pageId of pageIds){
            if (pageId == "General" && serverModel.GetItemsCount() == 0){
                applyDesignScheme();
                root.settingsUpdate(pageId);
            }
            else if (pageId == "General" && serverModel.GetItemsCount() > 0){
                continue;
            }

            root.settingsUpdate(pageId);
        }
    }

    function saveServerModel(pageIds){
        console.log("SettingsProvider saveServerModel", pageIds);
        for (let pageId of pageIds){
            if (pageId == "General"){
                updateLocalModel();
                applyDesignScheme();

//                console.log("localModel", localModel.toJSON());
                root.settingsUpdate(pageId);
            }
        }

        preferenceSaveQuery.save();
    }

    function applyDesignScheme(){
        let design = getDesignScheme();
        if (design){
            Style.getDesignScheme(design);
        }
    }

    function getDesignScheme(){
        for (let i = 0; i < localModel.GetItemsCount(); i++){
            let pageModel = localModel.GetModelFromItem(i);

            if (pageModel){
                let pageId = pageModel.GetData("Id");
                if (pageId == "General"){
                    let elements = pageModel.GetData("Elements");

                    for (let j = 0; j < elements.GetItemsCount(); j++){
                        let elementId = elements.GetData("Id", j);
                        if (elementId == "Mode"){
                            let elementValue = elements.GetData("Value", j);
                            let parameters = elements.GetData("Parameters", j);

                            let scheme = parameters.GetData("Id", elementValue);

                            return scheme;
                        }
                    }
                }
            }
        }

        return null;
    }

    function getSelectedLanguage(){
        for (let i = 0; i < localModel.GetItemsCount(); i++){
            let pageModel = localModel.GetModelFromItem(i);

            if (pageModel){
                let pageId = pageModel.GetData("Id");
                if (pageId == "General"){
                    let elements = pageModel.GetData("Elements");

                    for (let j = 0; j < elements.GetItemsCount(); j++){
                        let elementId = elements.GetData("Id", j);
                        if (elementId == "Language"){
                            let elementValue = elements.GetData("Value", j);
                            let parameters = elements.GetData("Parameters", j);

                            let scheme = parameters.GetData("Id", elementValue);

                            return scheme;
                        }
                    }
                }
            }
        }

        return null;
    }

    function getInstanceMask(){
        console.log("getInstanceMask");
        for (let i = 0; i < serverModel.GetItemsCount(); i++){
            let pageModel = serverModel.GetModelFromItem(i);

            if (pageModel){
                let pageId = pageModel.GetData("Id");
                if (pageId == "Server"){
                    let elements = pageModel.GetData("Elements");

                    for (let j = 0; j < elements.GetItemsCount(); j++){
                        let elementId = elements.GetData("Id", j);
                        if (elementId == "InstanceMask"){
                            let elementValue = elements.GetData("Value", j);
                            return elementValue;
                        }
                    }
                }
            }
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
            var query = Gql.GqlRequest("query", "GetSettings");

//            var inputParams = Gql.GqlObject("input");
//            inputParams.InsertField ("LanguageId", Style.language);
//            query.AddParam(inputParams);

            var queryFields = Gql.GqlObject("Get");
            queryFields.InsertField("Id");
            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            console.log("PreferenceDialog GqlModel getSettings query ", gqlData);
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
