import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtgui 1.0

QtObject {
    id: container;

    property SettingsProvider settingsProvider: null;
    property TreeItemModel languagesModel: TreeItemModel {}
    property string currentLanguage: context.language;
    property bool localizationChanged: false;

    Component.onDestruction: {
        if (container.settingsProvider != null){
            container.settingsProvider.localModelChanged.disconnect(container.updateLanguageModel);
            container.settingsProvider.serverModelChanged.disconnect(container.updateLanguageModel);
        }
    }

    onSettingsProviderChanged: {
        if (container.settingsProvider != null){
            container.settingsProvider.localModelChanged.connect(container.updateLanguageModel);
            container.settingsProvider.serverModelChanged.connect(container.updateLanguageModel);
            container.settingsProvider.serverSettingsSaved.connect(container.onServerSettingsSaved);
        }
    }

    function updateLanguageModel(){
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
                if (String(pageId) == String("General")){
                    let elements = pageModel.getData("Parameters");

                    for (let j = 0; j < elements.getItemsCount(); j++){
                        let elementId = elements.getData("Id", j);
                        if (String(elementId) == String("Language")){
                            languagesModel.clear();
                            languagesModel.copy(elements.getData("Parameters", j))
                            languagesModel.refresh();

                            break;
                        }
                    }
                    break;
                }
            }
        }
    }

    function getLanguageIdByIndex(index){
        if (settingsProvider == null || index < 0 || index >= container.languagesModel.getItemsCount()){
            return "";
        }

        if (container.languagesModel.containsKey("Id", index)){
            let langId = container.languagesModel.getData("Id", index);
            return langId;
        }

        return "";
    }

    function getLanguage(){
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
                        if (elementId == "Language"){
                            let elementValue = elements.getData("Value", j);
                            let parameters = elements.getData("Parameters", j);
                            if (parameters){
                                let scheme = parameters.getData("Id", elementValue);

                                return scheme;
                            }
                            else{
                                return null;
                            }
                        }
                    }
                }
            }
        }

        return null;
    }

    function setLanguage(langId){
        console.log("setLanguage", langId);
        console.log("context.language", context.language);
        if (context.language !== langId){
            context.language = langId;

            if (container.settingsProvider != null){
                container.settingsProvider.setLanguage(langId);
            }

            if (!settingsProvider.serverModel){
                Events.sendEvent("OnLocalizationChanged", langId);
            }

            localizationChanged = true;
        }
    }

    function onServerSettingsSaved(){
        let lang = getLanguage();

        if (localizationChanged){
            Events.sendEvent("OnLocalizationChanged", container.currentLanguage);

            localizationChanged = false;
        }
    }
}


