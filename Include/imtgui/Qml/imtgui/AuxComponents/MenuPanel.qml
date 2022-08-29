import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

Rectangle {
    id: menuPanel;

    color: Style.backgroundColor;

    radius: 7;

    clip: true;

    property string textColor: Style.textColor;
    property string fontName: "Helvetica";
    property string activePageId;
    property string activePageName;
    property string activeIcon;
    property string firstElementImageSources: [];

    property alias model: lvPages.model;

    property int activePageIndex: -1;
    property int pagesCount: lvPages.count;

    property int spacing: 0;

    signal activePageChanged;

    function updateModels(){
        pagesModel.updateModel();
    }

    onActivePageIndexChanged: {
        console.log("MenuPanel onActivePageIndexChanged", menuPanel.activePageIndex);
        lvPages.currentIndex = menuPanel.activePageIndex;
    }

    Flickable {
        anchors.fill: parent;
        anchors.topMargin: thumbnailDecoratorContainer.mainMargin;
        anchors.bottomMargin: thumbnailDecoratorContainer.mainMargin;

        contentWidth: columnPages.width;
        contentHeight: columnPages.height + 50;
        boundsBehavior: Flickable.StopAtBounds;

        clip: true;

        Column {
            id: columnPages;
            spacing: menuPanel.spacing;
            width: parent.width;

            Repeater {
                id: lvPages;
                property int currentIndex: 0;
                delegate:  MenuPanelButton{
                    text:  model["Name"];
                    textColor: Style.textColor;
                    fontName: menuPanel.fontName;
                    imageSource: (highlighted || selected) ? "../../../" + "Icons/" + Style.theme + "/" + model["Icon"] + "_" + "On" + "_" + "Selected" + ".svg":
                                                             "../../../" + "Icons/" + Style.theme + "/" + model["Icon"] + "_" + "On" + "_" + "Normal" + ".svg";

                    selected: lvPages.currentIndex === model.index;

                    decoratorSource : Style.menuButtonDecoratorPath;

                    onClicked: {
                        lvPages.currentIndex = model.index;
                        menuPanel.activePageName = model["Name"];
                        menuPanel.activeIcon = model["Icon"];
                        menuPanel.activePageIndex = model.index;
                        menuPanel.activePageId = model["PageId"];
                    }
                }
            }
        }

    }

    Timer {
        id: pageIndexTimer;

        interval: 10;

        property int pageIndex: -1;

        onTriggered: {
            menuPanel.activePageIndex  = pageIndexTimer.pageIndex;
        }
    }

    GqlModel {
        id: pagesModel;

        function updateModel(){
            var query = Gql.GqlRequest("query", "PagesData") ;

            var inputParams = Gql.GqlObject("input");
            inputParams.InsertField("LanguageId", Style.language);
            query.AddParam(inputParams);

            var queryFields = Gql.GqlObject("items");
            queryFields.InsertField("PageId");
            queryFields.InsertField("Name");
            queryFields.InsertField("Icon");
            queryFields.InsertField("Source");
            queryFields.InsertField("StartItem");
            query.AddField(queryFields);
            var gqlData = query.GetQuery();

            console.log("MenuPanel PagesData", gqlData);
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

                        menuPanel.activePageId = dataModelLocal.GetData("PageId");
                        menuPanel.activePageName = dataModelLocal.GetData("Name");
                        menuPanel.activeIcon = dataModelLocal.GetData("Icon");


                        pageIndexTimer.pageIndex = 0;
                        pageIndexTimer.start();
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
