import QtQuick 2.12
import Acf 1.0
import imtqml 1.0



Rectangle {
    id: menuPanel;
    width: 59;
    color: "#e6e6e8";
    property string textColor: "#335777";
    property string fontName: "Helvetica";
    property alias model: lvPages.model;
    property string activePageId;
    property string activePageName;
    property string activeIcon;
    signal activePageChanged;

    function updateModels(){
        pagesModel.updateModel();
    }


    ListView {
        id: lvPages;
        anchors.fill: parent;

        delegate:  MenuPanelButton{
            width: menuPanel.width;
            height: width;
            text:  model[PageEnum.NAME];
            textColor: menuPanel.textColor;
            fontName: menuPanel.fontName;
            imageSource: model[PageEnum.ICON];
//            imageSourceDisabled: model[PageEnum.ICON_OFF_SELECTED];
//            imageSourceSelected: model[PageEnum.ICON_ON_SELECTED];
            selected: lvPages.currentIndex === model.index ? true : false;

            onClicked: {
                lvPages.currentIndex = model.index;
                menuPanel.activePageId = model[PageEnum.ID];
                menuPanel.activePageName = model[PageEnum.NAME];
                menuPanel.activeIcon = model[PageEnum.ICON];
            }
        }

    }

    GqlModel {
        id: pagesModel
        Component.onCompleted: {
//            updateModel();
        }

        function updateModel(){
            console.log( "updateModel")


            var query = Gql.GqlRequest("query", "PagesData") ;

            var queryFields = Gql.GqlObject("items");
            queryFields.InsertField(PageEnum.ID);
            queryFields.InsertField(PageEnum.NAME);
            queryFields.InsertField(PageEnum.ICON);
            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            console.log(gqlData);
            pagesModel.SetGqlQuery(gqlData)
        }

        onStateChanged: {
            console.log("State:",state, pagesModel)
            if (state == "Ready"){
//                            console.log(registrationModel.GetData("data"))
//                            console.log(registrationModel.GetData("errors"))
                var dataModel = pagesModel.GetData("data");
                if(dataModel.ContainsKey("PagesData")){
                    dataModel = dataModel.GetData("PagesData")
                    console.log("PagesData", dataModel)
                    if(dataModel !== null && dataModel.ContainsKey("items")){
                        dataModel = dataModel.GetData("items")
                        console.log("items",dataModel)
                        lvPages.model = dataModel
                        dataModel.Refresh()
                        menuPanel.activePageId = dataModel.GetData(PageEnum.ID);
                        menuPanel.activePageName = dataModel.GetData(PageEnum.NAME);
                        menuPanel.activeIcon = dataModel.GetData(PageEnum.ICON);
                    }
                    else if(registrationModel.ContainsKey("errors")){
                        var errorsModel = pagesModel.GetData("errors");
                        if(errorsModel !== null && errorsModel.ContainsKey("PagesData")){
                            console.log("message", errorsModel.GetData("PagesData").GetData("message"))
                        }
                    }
                }
            }
        }
    }

}
