import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

Rectangle {
    id: menuPanel;

    color: Style.baseColor;

    radius: 7;

    clip: true;

    property string textColor: Style.textColor;
    property string fontName: "Helvetica";
    property string activePageId;
    property string activePageName;
    property string activeIcon;
    property string firstElementImageSources: [];

    property alias model: lvPages.model;

    property var pagesSources: [];

    property int activePageIndex: -1;
    property int pagesCount: lvPages.count;

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
        anchors.topMargin: thubnailDecoratorContainer.mainMargin;
        anchors.bottomMargin: thubnailDecoratorContainer.mainMargin;

        contentWidth: columnPages.width;
        contentHeight: columnPages.height + 50;
        boundsBehavior: Flickable.StopAtBounds;

        clip: true;

        Column {
            id: columnPages;

//            anchors.top: parent.top;
//            anchors.topMargin: thubnailDecoratorContainer.mainMargin;
//            anchors.bottom: parent.bottom;
//            anchors.bottomMargin: thubnailDecoratorContainer.mainMargin;

            width: parent.width;

            Repeater {
                id: lvPages;
                property int currentIndex: 0;
                delegate:  MenuPanelButton{
                    text:  model[PageEnum.NAME];
                    textColor: Style.textColor;
                    fontName: menuPanel.fontName;
                    imageSource: (highlighted || selected) ? "../../../" + "Icons/" + Style.theme + "/" + model[PageEnum.ICON] + "_" + "On" + "_" + "Selected" + ".svg":
                                                             "../../../" + "Icons/" + Style.theme + "/" + model[PageEnum.ICON] + "_" + "On" + "_" + "Normal" + ".svg";

                    selected: lvPages.currentIndex === model.index ? true : false;

                    decoratorSource : Style.menuButtonDecoratorPath;

                    onClicked: {
                        lvPages.currentIndex = model.index;
                        menuPanel.activePageName = model[PageEnum.NAME];
                        menuPanel.activeIcon = model[PageEnum.ICON];
                        menuPanel.activePageIndex = model.index;
                        menuPanel.activePageId = model[PageEnum.ID];

                        if (!menuPanel.focus){
                            menuPanel.forceActiveFocus();
                        }
                    }
                }
            }
        }

    }

//    ListView {
//        id: lvPages;

////        width: parent.width;
//        anchors.fill: parent;

//        boundsBehavior: Flickable.StopAtBounds;

//        delegate:  MenuPanelButton{
//            text:  model[PageEnum.NAME];
//            textColor: Style.textColor;
//            fontName: menuPanel.fontName;
//            imageSource: (highlighted || selected) ? "../../../" + "Icons/" + Style.theme + "/" + model[PageEnum.ICON] + "_" + "On" + "_" + "Selected" + ".svg":
//                                                     "../../../" + "Icons/" + Style.theme + "/" + model[PageEnum.ICON] + "_" + "On" + "_" + "Normal" + ".svg";

//            selected: lvPages.currentIndex === model.index ? true : false;

//            decoratorSource : Style.menuButtonDecoratorPath;
////            decoratorSource: model['DecoratorSource'];

//            onClicked: {
//                lvPages.currentIndex = model.index;
//                menuPanel.activePageName = model[PageEnum.NAME];
//                menuPanel.activeIcon = model[PageEnum.ICON];
//                menuPanel.activePageIndex = model.index;
//                menuPanel.activePageId = model[PageEnum.ID];

//                if (!menuPanel.focus){
//                    menuPanel.forceActiveFocus();
//                }
//            }
//        }
//    }

    Timer {
        id: pageIndexTimer;

        interval: 10;

        property int pageIndex: -1;

        onTriggered: {
            menuPanel.activePageIndex  = pageIndexTimer.pageIndex;
        }
    }

    function transformModel(modelInput){
        if (!modelInput){
            return;
        }

        for (let i = 0; i < modelInput.GetItemsCount(); i++){
            let subPages = modelInput.GetData("SubPages", i);
            if (subPages){

            }
        }
    }

    GqlModel {
        id: pagesModel;

        function updateModel(){
            var query = Gql.GqlRequest("query", "PagesData") ;

            var inputParams = Gql.GqlObject("input");
            inputParams.InsertField("LanguageId");
            inputParams.InsertFieldArgument("LanguageId", Style.language);
            query.AddParam(inputParams);

            var queryFields = Gql.GqlObject("items");
            queryFields.InsertField(PageEnum.ID);
            queryFields.InsertField(PageEnum.NAME);
            queryFields.InsertField(PageEnum.ICON);
            queryFields.InsertField(PageEnum.SOURCE);
            query.AddField(queryFields);
            var gqlData = query.GetQuery();

            console.log("MenuPanel PagesData", gqlData);
            this.SetGqlQuery(gqlData)
        }

        onStateChanged: {
            console.log("State:",this.state, pagesModel)
            if (this.state == "Ready"){
               // console.log("pagesModel toJSON", pagesModel.toJSON());
                var dataModelLocal = this.GetData("data");

                //refreshButton.enabled = true;

                if (!dataModelLocal){
                    thubnailDecoratorContainer.setInvalidConnection(true);
                    return;
                }

                thubnailDecoratorContainer.setInvalidConnection(false);
//                Style.changeSchemeDesign("");
                if(dataModelLocal.ContainsKey("PagesData")){
                    dataModelLocal = dataModelLocal.GetData("PagesData")
                    console.log("dataModelLocal", dataModelLocal);
                    if(dataModelLocal !== null && dataModelLocal.ContainsKey("items")){
                        dataModelLocal = dataModelLocal.GetData("items")
                        lvPages.model = dataModelLocal

                        menuPanel.activePageId = dataModelLocal.GetData(PageEnum.ID);
                        menuPanel.activePageName = dataModelLocal.GetData(PageEnum.NAME);
                        menuPanel.activeIcon = dataModelLocal.GetData(PageEnum.ICON);

                        //menuPanel.activePageIndex = 0;
//                        Style.changeSchemeDesign("");
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
