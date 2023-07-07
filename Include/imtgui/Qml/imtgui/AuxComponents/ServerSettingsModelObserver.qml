import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

TreeItemModelObserver {
    id: container;

    property DesignSchemaProvider designProvider: null;
    property LanguageProvider languageProvider: null;

    onModelChanged: {
        console.log("ServerSettingsModelObserver onModelChanged");
        for (let i = 0; i < changeList.length; i++){
            let changeObj = changeList[i]
            let changeId = changeObj["id"];

            let ids = changeId.split('/')
            if (ids.includes("Language")){
                context.language = languageProvider.getLanguage();
            }
            else if (ids.includes("DesignSchema")){
                if (container.designProvider != null){
                    container.designProvider.applyDesignSchema();
                }
            }
            else if (ids.includes("Database")){
                Events.sendEvent("UpdateModels");
            }
        }
    }
}
