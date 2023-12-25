import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

QtObject {
    id: container;

    property SettingsProvider settingsProvider: null;

    property TreeItemModel languagesModel: TreeItemModel {}

    property string currentLanguage: context.language;

    onSettingsProviderChanged: {
        if (container.settingsProvider != null){
            container.settingsProvider.localModelChanged.connect(container.onLocalModelChanged);
            container.settingsProvider.serverSettingsSaved.connect(container.onServerSettingsSaved);
        }
    }

    function onServerSettingsSaved(){
        Events.sendEvent("OnLocalizationChanged", container.currentLanguage);
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

        for (let i = 0; i < localModel.GetItemsCount(); i++){
            let pageModel = localModel.GetModelFromItem(i);
            if (pageModel){
                let pageId = pageModel.GetData("Id");
                if (pageId == "General"){
                    let elements = pageModel.GetData("Parameters");

                    for (let j = 0; j < elements.GetItemsCount(); j++){
                        let elementId = elements.GetData("Id", j);
                        if (elementId == "Language"){
                            languagesModel.Copy(elements.GetData("Parameters", j))
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

//            Events.sendEvent("OnLocalizationChanged", langId);
        }
    }
}
