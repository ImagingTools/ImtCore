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

        for (let i = 0; i < localModel.GetItemsCount(); i++){
            let pageModel = localModel.GetModelFromItem(i);
            if (pageModel){
                let pageId = pageModel.GetData("Id");
                if (String(pageId) == String("General")){
                    let elements = pageModel.GetData("Parameters");

                    for (let j = 0; j < elements.GetItemsCount(); j++){
                        let elementId = elements.GetData("Id", j);
                        if (String(elementId) == String("Language")){
                            languagesModel.Clear();
                            languagesModel.Copy(elements.GetData("Parameters", j))
                            languagesModel.Refresh();

                            break;
                        }
                    }
                    break;
                }
            }
        }
    }

    function getLanguageIdByIndex(index){
        if (settingsProvider == null || index < 0 || index >= container.languagesModel.GetItemsCount()){
            return "";
        }

        if (container.languagesModel.ContainsKey("Id", index)){
            let langId = container.languagesModel.GetData("Id", index);
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

        for (let i = 0; i < localModel.GetItemsCount(); i++){
            let pageModel = localModel.GetModelFromItem(i);

            if (pageModel){
                let pageId = pageModel.GetData("Id");
                if (pageId == "General"){
                    let elements = pageModel.GetData("Parameters");

                    for (let j = 0; j < elements.GetItemsCount(); j++){
                        let elementId = elements.GetData("Id", j);
                        if (elementId == "Language"){
                            let elementValue = elements.GetData("Value", j);
                            let parameters = elements.GetData("Parameters", j);
                            if (parameters){
                                let scheme = parameters.GetData("Id", elementValue);

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
        if (localizationChanged){
            Events.sendEvent("OnLocalizationChanged", container.currentLanguage);

            localizationChanged = false;
        }
    }
}


