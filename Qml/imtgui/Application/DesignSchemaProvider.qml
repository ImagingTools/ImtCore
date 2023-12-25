import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

QtObject {
    id: container;

    property SettingsProvider settingsProvider: null;
    property TreeItemModel schemesModel: TreeItemModel {}

    onSettingsProviderChanged: {
        console.log("DesignProvider onSettingsProviderChanged", settingsProvider);
        if (container.settingsProvider != null){
            container.settingsProvider.localModelChanged.connect(container.onLocalModelChanged);
        }
    }

    function onLocalModelChanged(){

        if (settingsProvider == null){
            return;
        }

        let localModel = settingsProvider.serverModel;

        if (localModel == null){
            localModel = settingsProvider.localModel;
        }

        if (localModel == null){
            return;
        }

        console.log("localModel", localModel.toJSON());

        for (let i = 0; i < localModel.GetItemsCount(); i++){
            let pageModel = localModel.GetModelFromItem(i);
            if (pageModel){
                let pageId = pageModel.GetData("Id");
                if (pageId == "General"){
                    let elements = pageModel.GetData("Parameters");

                    for (let j = 0; j < elements.GetItemsCount(); j++){
                        let elementId = elements.GetData("Id", j);
                        if (elementId == "DesignSchema"){
                            schemesModel.Copy(elements.GetData("Parameters", j))
                            break;
                        }
                    }
                    break;
                }
            }
        }
    }

    function applyDesignSchema(design){
        console.log("applyDesignSchema");
        if (design == undefined){
            design = getDesignSchema();
        }

        if (design != null){
            Style.getDesignScheme(design);
        }
    }

    function applyCachedDesignSchema(){
        console.log("applyCachedDesignSchema");
        let design = container.getDesignSchema();
        console.log("design", design);

        if (design != null){
            Style.getDesignScheme(design);
        }
    }

    function getDesignSchema(){
        if (settingsProvider == null){
            return null;
        }

        let localModel = settingsProvider.serverModel;

        if (localModel == null){
            localModel = settingsProvider.localModel;
        }

        if (localModel == null){
            return null;
        }

        for (let i = 0; i < localModel.GetItemsCount(); i++){
            let pageModel = localModel.GetModelFromItem(i);

            if (pageModel){
                let pageId = pageModel.GetData("Id");
                if (pageId == "General"){
                    let elements = pageModel.GetData("Parameters");

                    for (let j = 0; j < elements.GetItemsCount(); j++){
                        let elementId = elements.GetData("Id", j);
                        if (elementId == "DesignSchema"){
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

    function getSchemaByIndex(index){
        if (settingsProvider == null || index < 0 || index >= container.schemesModel.GetItemsCount()){
            return "";
        }

        if (container.schemesModel.ContainsKey("Id", index)){
            let schema = container.schemesModel.GetData("Id", index);
            return schema;
        }

        return "";
    }

    function getDesignSchemaIndex(designId){
        if (settingsProvider == null){
            return -1;
        }

        let localModel = settingsProvider.serverModel;

        if (localModel == null){
            localModel = settingsProvider.localModel;
        }

        if (localModel == null){
            return -1;
        }

        for (let i = 0; i < localModel.GetItemsCount(); i++){
            let pageModel = localModel.GetModelFromItem(i);
            if (pageModel){
                let pageId = pageModel.GetData("Id");
                if (pageId == "General"){
                    let elements = pageModel.GetData("Parameters");

                    for (let j = 0; j < elements.GetItemsCount(); j++){
                        let elementId = elements.GetData("Id", j);
                        if (elementId == "DesignSchema"){
                            let parameters = elements.GetData("Parameters", j);

                            for (let k = 0; k < parameters.GetItemsCount(); k++){
                                let id = parameters.GetData("Id", k);
                                if (String(id) == String(designId)){
                                    return k;
                                }
                            }

                            return -1;
                        }
                    }
                }
            }
        }

        return -1;
    }
}
