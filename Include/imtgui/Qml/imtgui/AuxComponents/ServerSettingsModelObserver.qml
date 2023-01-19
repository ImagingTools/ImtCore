import QtQuick 2.15
import Acf 1.0
import imtgui 1.0

TreeItemModelObserver {
    id: container;

    property DesignSchemaProvider designProvider: null;

    onModelChanged: {
        console.log("Changes:");
        for (let i = 0; i < changeList.length; i++){

            console.log("change", JSON.stringify(changeList[i]));
            let changeObj = changeList[i]
            let changeId = changeObj["id"];

            let ids = changeId.split('/')

            if (ids.includes("Language")){

            }
            else if (ids.includes("DesignSchema")){
                if (container.designProvider != null){
                    console.log("PreferenceObserver applyDesignSchema:");
                    container.designProvider.applyDesignSchema();
                }
            }
        }
        console.log();
    }
}
