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

    AUX.TopPanel{
        id: topPanel
    }

    AUX.MenuPanel{
        id: menuPanel
        anchors.top: topPanel.bottom
        anchors.bottom: parent.bottom
        onActivePageChanged: {
            topPanel.title = activePageId
            console.log("onActivePageChanged", activePageId)
            commandsModel.SetQueryParam(PageEnum.ID,activePageId);
            commandsModel.SetQueryParam(CommandEnum.ID,"");
            commandsModel.needsReload()
        }
    }





}
