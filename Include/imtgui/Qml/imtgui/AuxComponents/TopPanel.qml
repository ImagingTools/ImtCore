import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

Rectangle {
    id: topPanel;

    color: Style.backgroundColor;

    property bool loginDialog: false;

    Component.onCompleted: {
        console.log("TopPanel onCompleted");
    }

    function updateModels(){
        modelUserMode.getUserMode();
    }

    onWidthChanged: {
         if (topPanelDecoratorLoader.item){
             topPanelDecoratorLoader.item.width = topPanel.width;
         }
    }

    Loader {
        id: topPanelDecoratorLoader;

        source: Style.topPanelDecoratorPath;

        onSourceChanged: {
            console.log("TopPanel onSourceChanged", source);
        }

        onItemChanged: {
            if (topPanelDecoratorLoader.item){
                topPanelDecoratorLoader.item.width = topPanel.width;
                topPanelDecoratorLoader.item.height = topPanel.height;
                topPanelDecoratorLoader.item.topPanel = topPanel;
            }
        }
    }

    GqlModel{
        id: modelUserMode;
        function getUserMode() {
            var query = Gql.GqlRequest("query", "GetUserMode");

            var queryFields = Gql.GqlObject("items");
            queryFields.InsertField("UserMode");
            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            console.log("TopPanel GqlModel getUserMode query ", gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, modelUserMode);
            if (this.state === "Ready"){
                var dataModelLocal;

                if (modelUserMode.ContainsKey("errors")){
                    return;
                }

                if (modelUserMode.ContainsKey("data")){
                    dataModelLocal = modelUserMode.GetData("data")
                    if (dataModelLocal.ContainsKey("GetUserMode")){
                        dataModelLocal = dataModelLocal.GetData("GetUserMode");
                        if(dataModelLocal.ContainsKey("items")){
                            dataModelLocal = dataModelLocal.GetData("items");
                            if(dataModelLocal.ContainsKey("Value")){
                                dataModelLocal = dataModelLocal.GetData("Value");
                                if (dataModelLocal == 0){
                                    loginDialog = false
                                }
                                else{
                                    loginDialog = true
                                }
                                if (dataModelLocal == 2){
                                    var source = "AuxComponents/LoginDialog.qml";
                                    var parameters = {};
                                    parameters["localSettings"] = thumbnailDecoratorContainer.localSettings;
                                    thumbnailDecoratorContainer.openDialog(source, parameters);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

