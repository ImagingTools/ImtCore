import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

Rectangle {
    id: menuPanel;
    width: 59;
//    width: 120;
    color: Style.backgroundColor;
    property string textColor: Style.textColor;
    property string fontName: "Helvetica";
    property alias model: lvPages.model;
    property string activePageId;
    property string activePageName;
    property string activeIcon;
    signal activePageChanged;

    function updateModels(){
        pagesModel.updateModel();
    }

//    Component.onCompleted: {
//        var item1 = this.createComponent('TreeItemModel.qml', this)
//        item1.append([{'color': 'red'}, {'color': 'black'}, {'color': 'blue'}])
//        console.log("item1",item1)
//        lvPages.model = item1
//    }


    ListView {
        id: lvPages;
        anchors.fill: parent;



//        delegate:  Rectangle{
//            width: 200; //menuPanel.width;
//            height: 100; //width;
//            color: "red";

//        }


        delegate:  MenuPanelButton{
            width: menuPanel.width;
            height: width;
            text:  model[PageEnum.NAME];
            textColor: Style.textColor;
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
        id: pagesModel;

        function updateModel(){
            var query = Gql.GqlRequest("query", "PagesData") ;

            var queryFields = Gql.GqlObject("items");
            queryFields.InsertField(PageEnum.ID);
            queryFields.InsertField(PageEnum.NAME);
            queryFields.InsertField(PageEnum.ICON);
            query.AddField(queryFields);
            var gqlData = query.GetQuery();
            this.SetGqlQuery(gqlData)
        }

        onStateChanged: {
            console.log("State:",this.state, pagesModel)
            if (this.state == "Ready"){
                var dataModelLocal = this.GetData("data");
                if(dataModelLocal.ContainsKey("PagesData")){
                    dataModelLocal = dataModelLocal.GetData("PagesData")
                    if(dataModelLocal !== null && dataModelLocal.ContainsKey("items")){
                        dataModelLocal = dataModelLocal.GetData("items")
                        lvPages.model = dataModelLocal

                        menuPanel.activePageId = dataModelLocal.GetData(PageEnum.ID);
                        menuPanel.activePageName = dataModelLocal.GetData(PageEnum.NAME);
                        menuPanel.activeIcon = dataModelLocal.GetData(PageEnum.ICON);

//                        lvPages.__processUpdates();
                    }
                    else if(this.ContainsKey("errors")){
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
