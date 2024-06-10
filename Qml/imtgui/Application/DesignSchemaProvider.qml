import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

QtObject {
    id: container;

    property SettingsProvider settingsProvider: null;
    property TreeItemModel schemesModel: TreeItemModel {}

    onSettingsProviderChanged: {
        console.log("DesignProvider onSettingsProviderChanged", settingsProvider);
        if (container.settingsProvider != null){
            container.settingsProvider.localModelChanged.connect(container.modelUpdated);
            container.settingsProvider.serverModelChanged.connect(container.modelUpdated);
        }
    }

    function modelUpdated(){
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

        for (let i = 0; i < localModel.getItemsCount(); i++){
            let pageModel = localModel.getModelFromItem(i);
            if (pageModel){
                let pageId = pageModel.getData("Id");
                if (pageId == "General"){
                    let elements = pageModel.getData("Parameters");

                    for (let j = 0; j < elements.getItemsCount(); j++){
                        let elementId = elements.getData("Id", j);
                        if (elementId == "DesignSchema"){
                            schemesModel.copy(elements.getData("Parameters", j))
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
        let design = container.getDesignSchema();
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

        for (let i = 0; i < localModel.getItemsCount(); i++){
            let pageModel = localModel.getModelFromItem(i);

            if (pageModel){
                let pageId = pageModel.getData("Id");
                if (pageId == "General"){
                    let elements = pageModel.getData("Parameters");

                    for (let j = 0; j < elements.getItemsCount(); j++){
                        let elementId = elements.getData("Id", j);
                        if (elementId == "DesignSchema"){
                            let elementValue = elements.getData("Value", j);
                            let parameters = elements.getData("Parameters", j);

                            let scheme = parameters.getData("Id", elementValue);

                            return scheme;
                        }
                    }
                }
            }
        }

        return null;
    }

    function getSchemaByIndex(index){
        if (settingsProvider == null || index < 0 || index >= container.schemesModel.getItemsCount()){
            return "";
        }

        if (container.schemesModel.containsKey("Id", index)){
            let schema = container.schemesModel.getData("Id", index);
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

        for (let i = 0; i < localModel.getItemsCount(); i++){
            let pageModel = localModel.getModelFromItem(i);
            if (pageModel){
                let pageId = pageModel.getData("Id");
                if (pageId == "General"){
                    let elements = pageModel.getData("Parameters");

                    for (let j = 0; j < elements.getItemsCount(); j++){
                        let elementId = elements.getData("Id", j);
                        if (elementId == "DesignSchema"){
                            let parameters = elements.getData("Parameters", j);

                            for (let k = 0; k < parameters.getItemsCount(); k++){
                                let id = parameters.getData("Id", k);
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
