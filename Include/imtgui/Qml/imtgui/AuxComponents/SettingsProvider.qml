import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

QtObject {
    id: container;

    property TreeItemModel serverModel: null;
    property TreeItemModel localModel: null;

    property var applicationInfoProvider;

    Component.onCompleted: {
//        Events.subscribeEvent("OnLocalizationChanged", container.updateModel);
        Events.subscribeEvent("UpdateSettings", container.updateModel);
    }

    Component.onDestruction: {
//        Events.unSubscribeEvent("OnLocalizationChanged", container.updateModel);
        Events.unSubscribeEvent("UpdateSettings", container.updateModel);
    }

    onApplicationInfoProviderChanged: {
        if (container.applicationInfoProvider){
            container.aboutApplicationProvider.applicationInfoProvider = container.applicationInfoProvider;
        }
    }

    property PageAboutProvider aboutApplicationProvider: PageAboutProvider{}

    property QtObject private_: QtObject {
        property TreeItemModel representationModel: TreeItemModel {};

        function mergeWithExternModel(externModel){
            if (externModel != undefined && externModel != null){
                for (let i = 0; i < externModel.GetItemsCount(); i++){
                    let pageId = externModel.GetData("Id", i);
                    let pageName = externModel.GetData("Name", i);
                    let pageParameters = externModel.GetData("Parameters", i);

                    let index = container.private_.getPageIndexByPageId(pageId)
                    if (index < 0){
                        index = container.private_.representationModel.InsertNewItem();
                    }

                    container.private_.representationModel.SetData("Id", pageId, index);
                    container.private_.representationModel.SetData("Name", pageName, index);

                    if(pageParameters !== null){
                        let parametersModel = container.private_.representationModel.AddTreeModel("Parameters", index);
                        parametersModel.Copy(pageParameters)
                    }
                }
            }
        }

        function getPageIndexByPageId(pageId){
            for (let i = 0; i < container.private_.representationModel.GetItemsCount(); i++){
                let id = container.private_.representationModel.GetData("Id", i);
                if (String(id) === String(pageId)){
                    return i;
                }
            }

            return -1;
        }

        function setValueToLocalModel(rootKey, key, value){
            if (!container.localModel){
                return;
            }

            for (let i = 0; i < container.localModel.GetItemsCount(); i++){
                let id = String(container.localModel.GetData("Id", i));
                if (id === rootKey){
                    let parametersModel = container.localModel.GetData("Parameters", i)
                    if (parametersModel){
                        for (let j = 0; j < parametersModel.GetItemsCount(); j++){
                            let parameterId = String(parametersModel.GetData("Id", j));
                            if (parameterId === key){
                                parametersModel.SetData("Value", value, j);
                                break;
                            }
                        }
                    }

                    break;
                }
            }
        }

        function setValueToServerModel(rootKey, key, value){
            if (!container.serverModel){
                return;
            }

            for (let i = 0; i < container.serverModel.GetItemsCount(); i++){
                let id = String(container.serverModel.GetData("Id", i));
                if (id === rootKey){
                    let parametersModel = container.serverModel.GetData("Parameters", i)
                    if (parametersModel){
                        for (let j = 0; j < parametersModel.GetItemsCount(); j++){
                            let parameterId = String(parametersModel.GetData("Id", j));
                            if (parameterId === key){
                                parametersModel.SetData("Value", value, j);
                                break;
                            }
                        }
                    }
                    break;
                }
            }
        }

        function getParameters(rootKey, key){
            if (!container.serverModel){
                return null;
            }
            for (let i = 0; i < container.serverModel.GetItemsCount(); i++){
                let id = String(container.serverModel.GetData("Id", i));
                if (id === rootKey){
                    let parametersModel = container.serverModel.GetData("Parameters", i)

                    return parametersModel;
                }
            }

            return null;
        }
    }

    property bool modelsCompleted: serverModel && localModel;

    signal serverSettingsSaved();
    signal localSettingsSaved();

    onServerModelChanged: {
        container.cacheServerModel();
    }

    onLocalModelChanged: {
        console.log("SettingsProvider onLocalModelChanged", container.localModel.toJSON());

        if (container.localModel){
            container.localModel.dataChanged.connect(container.onLocalModelDataChanged);
        }
    }

    function onLocalModelDataChanged(){
        console.log("onLocalModelDataChanged", container.localModel.toJSON());
    }

    function clearModel(){
        if (container.serverModel){
            container.serverModel.Clear();
        }
    }

    function getRepresentationModel(){
        console.log("getRepresentationModel");
        private_.representationModel.Clear();

        private_.mergeWithExternModel(container.serverModel);
        private_.mergeWithExternModel(container.localModel);

        if (container.aboutApplicationProvider != null){
            let representationModel = container.aboutApplicationProvider.getRepresentationModel();

            private_.mergeWithExternModel(representationModel);
        }

        return private_.representationModel;
    }

    function updateModel(){
        settingsQuery.getSettings();
    }

    function saveLocalModel(){
        container.localSettingsSaved();
    }

    function saveServerModel(){
        preferenceSaveQuery.save();

        saveLocalModel();
    }

    function setDesignSchema(schema){
        console.log("setDesignSchema", schema);

        let model = container.serverModel;

        if (!model){
            model = container.localModel;
        }

        if (model){
            for (let i = 0; i < model.GetItemsCount(); i++){
                let id = String(model.GetData("Id", i));
                if (id == String("General")){
                    let parametersModel = model.GetData("Parameters", i)
                    if (parametersModel){
                        for (let j = 0; j < parametersModel.GetItemsCount(); j++){
                            let parameterId = String(parametersModel.GetData("Id", j));
                            if (parameterId == String("DesignSchema")){
                                let languageParametersModel = parametersModel.GetData("Parameters", j);
                                for (let k = 0; k < languageParametersModel.GetItemsCount(); k++){
                                    let langId = languageParametersModel.GetData("Id", k);
                                    if (String(langId) == String(schema)){
                                        parametersModel.SetData("Value", k, j);
                                        break;
                                    }
                                }
                                break;
                            }
                        }
                    }
                    break;
                }
            }
        }

        console.log("model", model.toJSON());

        if (container.serverModel){
            container.cacheServerModel();

            preferenceSaveQuery.save();
        }
        else{
            container.localSettingsSaved();
        }
    }

    function setLanguage(language){
        let model = container.serverModel;

        if (!model){
            model = container.localModel;
        }

        if (model){
            for (let i = 0; i < model.GetItemsCount(); i++){
                let id = String(model.GetData("Id", i));
                if (id == String("General")){
                    let parametersModel = model.GetData("Parameters", i)
                    if (parametersModel){
                        for (let j = 0; j < parametersModel.GetItemsCount(); j++){
                            let parameterId = String(parametersModel.GetData("Id", j));
                            if (parameterId == String("Language")){
                                let languageParametersModel = parametersModel.GetData("Parameters", j);
                                for (let k = 0; k < languageParametersModel.GetItemsCount(); k++){
                                    let langId = languageParametersModel.GetData("Id", k);
                                    if (String(langId) == String(language)){
                                        parametersModel.SetData("Value", k, j);
                                        break;
                                    }
                                }
                                break;
                            }
                        }
                    }
                    break;
                }
            }
        }

        if (container.serverModel){
            container.cacheServerModel();

            preferenceSaveQuery.save();
        }
        else{
            container.localSettingsSaved();
        }
    }

    function setServerUrl(serverUrl){
        private_.setValueToLocalModel("NetworkSettings", "ServerUrl", serverUrl);

        container.localSettingsSaved();
    }

    function setWebSocketServerUrl(webSocketServerUrl){
        private_.setValueToLocalModel("NetworkSettings", "WebSocketServerUrl", webSocketServerUrl);

        container.localSettingsSaved();
    }

    function cacheServerModel(){
        container.rewriteModel(container.serverModel, container.localModel);
        container.localSettingsSaved();
    }

    function rewriteModel(fromModel, toModel){
        if (!fromModel || !toModel){
            return;
        }

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

    function getValue(parameterId){
        console.log("getValue", parameterId);
        let settingsModel = getRepresentationModel();
        console.log("settingsModel", settingsModel.toJSON());

        return findValue(settingsModel, parameterId);
    }

    function findValue(model, parameterId){
        for (let i = 0; i < model.GetItemsCount(); i++){
            let paramId = model.GetData("Id", i);
            if (String(paramId) === String(parameterId)){
                return model.GetData("Value", i)
            }

            if (model.ContainsKey("Parameters", i)){
                let parameters = model.GetData("Parameters", i);

                let result = findValue(parameters, parameterId);
                if (result !== ""){
                    return result;
                }
            }
        }

        return "";
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
            console.log("State:", this.state, container.preferenceSaveQuery);
            if (this.state === "Ready") {
                if (this.ContainsKey("errors")){
                    return;
                }

                container.serverSettingsSaved();
            }
        }
    }//SaveSettings
}
