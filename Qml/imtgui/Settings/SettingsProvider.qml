import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtgui 1.0
import imtguigql 1.0

QtObject {
    id: container;

    property TreeItemModel serverModel: null;
    property TreeItemModel localModel: null;

    property var applicationInfoProvider;

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
                for (let i = 0; i < externModel.getItemsCount(); i++){
                    let pageId = externModel.getData("Id", i);
                    let pageName = externModel.getData("Name", i);
                    let pageParameters = externModel.getData("Parameters", i);

                    let index = container.private_.getPageIndexByPageId(pageId)
                    if (index < 0){
                        index = container.private_.representationModel.insertNewItem();
                    }

                    container.private_.representationModel.setData("Id", pageId, index);
                    container.private_.representationModel.setData("Name", pageName, index);

                    if(pageParameters !== null){
                        let parametersModel = container.private_.representationModel.addTreeModel("Parameters", index);
                        parametersModel.copy(pageParameters)
                    }
                }
            }
        }

        function getPageIndexByPageId(pageId){
            for (let i = 0; i < container.private_.representationModel.getItemsCount(); i++){
                let id = container.private_.representationModel.getData("Id", i);
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

            for (let i = 0; i < container.localModel.getItemsCount(); i++){
                let id = String(container.localModel.getData("Id", i));
                if (id === rootKey){
                    let parametersModel = container.localModel.getData("Parameters", i)
                    if (parametersModel){
                        for (let j = 0; j < parametersModel.getItemsCount(); j++){
                            let parameterId = String(parametersModel.getData("Id", j));
                            if (parameterId === key){
                                parametersModel.setData("Value", value, j);
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

            for (let i = 0; i < container.serverModel.getItemsCount(); i++){
                let id = String(container.serverModel.getData("Id", i));
                if (id === rootKey){
                    let parametersModel = container.serverModel.getData("Parameters", i)
                    if (parametersModel){
                        for (let j = 0; j < parametersModel.getItemsCount(); j++){
                            let parameterId = String(parametersModel.getData("Id", j));
                            if (parameterId === key){
                                parametersModel.setData("Value", value, j);
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
            for (let i = 0; i < container.serverModel.getItemsCount(); i++){
                let id = String(container.serverModel.getData("Id", i));
                if (id === rootKey){
                    let parametersModel = container.serverModel.getData("Parameters", i)

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
        if (container.localModel){
            container.localModel.modelChanged.connect(container.onLocalModelDataChanged);
        }
    }

    function onLocalModelDataChanged(){
    }

    function clearModel(){
        if (container.serverModel){
            container.serverModel.clear();
        }
    }

    function getRepresentationModel(){
        private_.representationModel.clear();

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
        let model = container.serverModel;

        if (!model){
            model = container.localModel;
        }

        if (model){
            for (let i = 0; i < model.getItemsCount(); i++){
                let id = String(model.getData("Id", i));
                if (id == String("General")){
                    let parametersModel = model.getData("Parameters", i)
                    if (parametersModel){
                        for (let j = 0; j < parametersModel.getItemsCount(); j++){
                            let parameterId = String(parametersModel.getData("Id", j));
                            if (parameterId == String("DesignSchema")){
                                let languageParametersModel = parametersModel.getData("Parameters", j);
                                for (let k = 0; k < languageParametersModel.getItemsCount(); k++){
                                    let langId = languageParametersModel.getData("Id", k);
                                    if (String(langId) == String(schema)){
                                        let currentValue = parametersModel.getData("Value", j)
                                        if (currentValue === k){
                                            return;
                                        }

                                        parametersModel.setData("Value", k, j);
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

    function setLanguage(language){
        let model = container.serverModel;

        if (!model){
            model = container.localModel;
        }

        if (model){
            for (let i = 0; i < model.getItemsCount(); i++){
                let id = String(model.getData("Id", i));
                if (id == String("General")){
                    let parametersModel = model.getData("Parameters", i)
                    if (parametersModel){
                        for (let j = 0; j < parametersModel.getItemsCount(); j++){
                            let parameterId = String(parametersModel.getData("Id", j));
                            if (parameterId == String("Language")){
                                let languageParametersModel = parametersModel.getData("Parameters", j);
                                if (languageParametersModel){
                                    for (let k = 0; k < languageParametersModel.getItemsCount(); k++){
                                        let langId = languageParametersModel.getData("Id", k);
                                        if (String(langId) == String(language)){
                                            let currentValue = parametersModel.getData("Value", j)
                                            if (currentValue === k){
                                                return;
                                            }

                                            parametersModel.setData("Value", k, j);
                                            break;
                                        }
                                    }
                                    break;
                                }
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

        for (let i = 0; i < toModel.getItemsCount(); i++){
            let pageId = toModel.getData("Id", i);
            let index = -1;
            for (let j = 0; j < fromModel.getItemsCount(); j++){
                let id = fromModel.getData("Id", j);
                if (id == pageId){
                    index = j;

                    break;
                }
            }

            if (index >= 0){
                toModel.copyItemDataFromModel(i, fromModel, index);
            }
        }
    }

    function getValue(parameterId){
        let settingsModel = getRepresentationModel();

        return findValue(settingsModel, parameterId);
    }

    function findValue(model, parameterId){
        for (let i = 0; i < model.getItemsCount(); i++){
            let paramId = model.getData("Id", i);
            if (String(paramId) === String(parameterId)){
                return model.getData("Value", i)
            }

            if (model.containsKey("Parameters", i)){
                let parameters = model.getData("Parameters", i);

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
            var query = Gql.GqlRequest("query", "GetSettings");

            var gqlData = query.GetQuery();

            this.setGqlQuery(gqlData);
        }

        onStateChanged: {
            if (this.state === "Ready") {
                var dataModelLocal;
                console.log("GetSettings", this.toJson());

                if (container.settingsQuery.containsKey("errors")){
                    dataModelLocal = container.settingsQuery.getData("errors");

                    return;
                }

                if (container.settingsQuery.containsKey("data")){
                    dataModelLocal = container.settingsQuery.getData("data");

                    if (dataModelLocal.containsKey("GetSettings")){
                        dataModelLocal = dataModelLocal.getData("GetSettings");

                        container.serverModel = dataModelLocal;
                    }
                }
            }
        }
    }//GetSettings

    property GqlModel preferenceSaveQuery: GqlModel {
        function save(){
            var query = Gql.GqlRequest("mutation", "SetSettings");

            var inputParams = Gql.GqlObject("input");
            var jsonString = container.serverModel.toJson();

            inputParams.InsertField("Item", jsonString);
            query.AddParam(inputParams);

            var gqlData = query.GetQuery();

            this.setGqlQuery(gqlData);
        }

        onStateChanged: {
            if (this.state === "Ready") {
                if (this.containsKey("errors")){
                    return;
                }

                container.serverSettingsSaved();
            }
        }
    }//SaveSettings
}
