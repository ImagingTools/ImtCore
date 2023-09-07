import QtQuick 2.12
import Acf 1.0

TreeItemModelObserver {
    id: container;

    property DesignSchemaProvider designProvider: null;
    property LanguageProvider languageProvider: null;
    property SettingsProvider settingsProvider: null;

    onModelChanged: {
        console.log("SettingsObserver onModelChanged");

        for (let i = 0; i < changeList.length; i++){
            let changeObj = changeList[i]
            let changeId = changeObj["id"];

            console.log("changeId", changeId)

            let ids = changeId.split('/')
            if (ids.includes("Language")){
                let newLang = changeObj["newVal"];
                container.onLanguageChanged(newLang);
            }
            else if (ids.includes("DesignSchema")){
                let newVal = changeObj["newVal"];
                container.onDesignSchemaChanged(newVal);
            }
            else if (ids.includes("Database")){
                container.onDatabaseAccessSettingsChanged();
            }
            else if (ids.includes("ServerUrl")){
                let newServerUrl = changeObj["newVal"];
                container.onServerUrlChanged(newServerUrl);
            }
        }
    }

    function onDesignSchemaChanged(newVal){
        if (container.settingsProvider != null){
            let schema = container.designProvider.getSchemaByIndex(newVal);
            container.settingsProvider.setDesignSchema(schema);
        }

        if (container.designProvider != null && container.settingsProvider != null){
            container.designProvider.applyDesignSchema();
        }
    }

    function onServerUrlChanged(newVal){
        if (container.settingsProvider != null){
            container.settingsProvider.setServerUrl(newVal);
            Events.sendEvent("Logout");
        }
    }

    function onLanguageChanged(newVal){
        if (container.settingsProvider != null){
            let lang = container.languageProvider.getLanguageIdByIndex(newVal);
            container.settingsProvider.setLanguage(lang);
        }

        if (container.languageProvider != null){
            let language = languageProvider.getLanguage();
            Events.sendEvent("OnLocalizationChanged", language);
            context.language = language;
        }
    }

    function onDatabaseAccessSettingsChanged(){
        Events.sendEvent("UpdateModels");
    }
}
