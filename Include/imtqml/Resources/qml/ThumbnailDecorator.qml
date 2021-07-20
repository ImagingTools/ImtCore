import QtQuick 2.0
import 'AuxComponents' as AUX
import Acf 1.0

Rectangle
{
    id: container
    anchors.fill: parent
    width: 300
    height: 200
//    color: "#f0f0f0"
    color: "white"
    property var pagesModel
//        property TreeItemModel pagesModel
    property var commandsModel
    property string fontName: mainFont.name
//    property TreeItemModel commandsModel

    function firstModelsInit() {
        console.log("firstModelsInit")
        pagesModel.needsReload()
        commandsModel.SetQueryParam(PageEnum.ID,"Packages");
        commandsModel.SetQueryParam(CommandEnum.ID,"");
        commandsModel.needsReload()
    }

    onPagesModelChanged: {
        if (pagesModel.ContainsKey(AppEnum.PAGES)){
            menuPanel.model = pagesModel.GetData(AppEnum.PAGES)
            console.log("menuPanel.model", menuPanel.model)
        }
    }

    onCommandsModelChanged: {
//        var pageId = commandsModel.GetData("pageId")
        //        console.log("pages", pages)
        if (commandsModel.ContainsKey(PageEnum.COMMANDS)){
            topPanel.model = commandsModel.GetData(PageEnum.COMMANDS)
            console.log("topPanel.model", topPanel.model)
        }
    }

    FontLoader{
        id: mainFont
        source: "../Fonts/Ubuntu-Regular.ttf"
    }

    AUX.TopPanel{
        id: topPanel
        fontName: container.fontName
    }

    AUX.MenuPanel{
        id: menuPanel
        anchors.top: topPanel.bottom
        anchors.bottom: parent.bottom
        textColor: container.textColor
        fontName: container.fontName
        onActivePageChanged: {
            topPanel.title = activePageId
            tabPanel.firstElementName = activePageId
            tabPanel.firstElementImageSource = activeIcon
            console.log("onActivePageChanged", activePageId)
            commandsModel.SetQueryParam(PageEnum.ID,activePageId);
            commandsModel.SetQueryParam(CommandEnum.ID,"");
            commandsModel.needsReload()
        }
    }

    AUX.TabPanel{
        id:tabPanel
        anchors.left: menuPanel.right
        anchors.right: parent.right
        anchors.top: topPanel.bottom
        visible: true//ПОМЕНЯТЬ!!!
        textColor: container.textColor
        fontName: container.fontName
    }


    AUX.Table{
        id: table
        anchors.left: menuPanel.right
        width: tabPanel.width/4*3
        anchors.top: tabPanel.bottom
        anchors.bottom: parent.bottom
        textColor: container.textColor
        fontName: container.fontName
    }


}
