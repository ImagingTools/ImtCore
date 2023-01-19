import QtQuick 2.15
import Acf 1.0
import imtqml 1.0

Item {
    id: container;

    property SettingsProvider settingsProvider: null;

    function applyDesignSchema(design){
        if (design == undefined){
            design = getDesignSchema();
        }

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
}
