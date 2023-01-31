import QtQuick 2.15
import Acf 1.0
import imtgui 1.0

TreeItemModelObserver {
    id: container;

    property DesignSchemaProvider designProvider: null;

    property Item root;

    onModelChanged: {
        console.log("SettingsModelObserver onModelChanged");
        for (let i = 0; i < changeList.length; i++){
            let changeObj = changeList[i]
            let changeId = changeObj["id"];

            let ids = changeId.split('/')
            console.log("ids", ids);

            if (ids.includes("Language")){
                container.root.updateAllModels();
            }
            else if (ids.includes("DesignSchema")){
                if (container.designProvider != null){
                    container.designProvider.applyDesignSchema();
                }
            }
            else if (ids.includes("Database")){
                container.root.updateAllModels();
            }
        }
    }
}
