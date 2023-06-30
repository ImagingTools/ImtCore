import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

TreeItemModelObserver {
    id: container;

    property DesignSchemaProvider designProvider: null;
    property LanguageProvider languageProvider: null;

    onModelChanged: {
        console.log("LocalSettingsModelObserver changes:");
        for (let i = 0; i < changeList.length; i++){

            console.log("change", JSON.stringify(changeList[i]));
            let changeObj = changeList[i]
            let changeId = changeObj["id"];

            let ids = changeId.split('/')

            if (ids.includes("Language")){
                console.log("Language Changes");
                if (container.languageProvider != null){
                    console.log("context.language", context.language);
                    console.log("context.language", languageProvider.getLanguage());
                    context.language = languageProvider.getLanguage();
                }
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
