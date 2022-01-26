import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

Rectangle {
    id: menuPanel;
    width: 75;
    color: Style.backgroundColor;
    property string textColor: Style.textColor;
    property string fontName: "Helvetica";
    property alias model: lvPages.model;
    property string activePageId;
    property string activePageName;
    property string activeIcon;
    property var pagesSources: [];
    property string firstElementImageSources: [];
    property int activePageIndex: -1;
    property int pagesCount: 0;
    signal activePageChanged;

    function updateModels(){
        pagesModel.updateModel();
    }

    ListView {
        id: lvPages;
        anchors.fill: parent;
        boundsBehavior: Flickable.StopAtBounds;
        delegate:  MenuPanelButton{
            width: menuPanel.width;
            height: width * 0.88;
            text:  model[PageEnum.NAME];
            textColor: Style.textColor;
            fontName: menuPanel.fontName;
            imageSource: (highlighted || selected) ? "../../../" + "Icons/" + Style.theme + "/" + model[PageEnum.ICON] + "_" + "On" + "_" + "Selected" + ".svg":
                                                     "../../../" + "Icons/" + Style.theme + "/" + model[PageEnum.ICON] + "_" + "On" + "_" + "Normal" + ".svg";

            selected: lvPages.currentIndex === model.index ? true : false;

            onClicked: {
                lvPages.currentIndex = model.index;              
                menuPanel.activePageName = model[PageEnum.NAME];
                menuPanel.activeIcon = model[PageEnum.ICON];
                menuPanel.activePageIndex = model.index;
                 menuPanel.activePageId = model[PageEnum.ID];

                console.log("menuPanel.activePageName ", menuPanel.activePageName);
                console.log("menuPanel.activePageId ", menuPanel.activePageId);
                console.log("menuPanel.activeIcon ", model[PageEnum.ICON]);
            }
        }

    }

    Timer {
        id: pageIndexTimer;
        property int pageIndex: -1;
        interval: 10;
        onTriggered: {
            menuPanel.activePageIndex  = pageIndexTimer.pageIndex;
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
            queryFields.InsertField(PageEnum.SOURCE);
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
                        console.log("PageModel pagesData !");
                        lvPages.model = dataModelLocal

                        menuPanel.activePageId = dataModelLocal.GetData(PageEnum.ID);
                        menuPanel.activePageName = dataModelLocal.GetData(PageEnum.NAME);
                        menuPanel.activeIcon = dataModelLocal.GetData(PageEnum.ICON);

                        pageIndexTimer.pageIndex = 0;
                        pageIndexTimer.start();

//                        menuPanel.activePageIndex = 0;
                        console.log("PageModel pagesData end!");
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
