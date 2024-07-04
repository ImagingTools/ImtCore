import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

Rectangle {
    id: menuPanel;

    color: Style.backgroundColor;

    radius: 7;

    clip: true;

    width: visible ? contentWidth : 0;

    property string textColor: Style.textColor;
    property string fontName: "Helvetica";
    property string activePageId;
    property string activePageName;
    property string activeIcon;
    property string firstElementImageSources: "";

    property int activePageIndex: -1;
    property int pagesCount: lvPages.count;

    property TreeItemModel model: TreeItemModel {};

    property int spacing: 0;

    property int count: lvPages.count;
    property int contentWidth: 80;

    signal activePageChanged;

    Component.onCompleted: {
        Events.subscribeEvent("MenuModelRequest", menuPanel.onMenuModelRequest);
        Events.subscribeEvent("ChangePage", menuPanel.changePage);
        Events.subscribeEvent("UpdatePageVisualStatus", menuPanel.updateVisualStatus);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("MenuModelRequest", menuPanel.onMenuModelRequest);
        Events.unSubscribeEvent("ChangePage", menuPanel.changePage);
        Events.unSubscribeEvent("UpdatePageVisualStatus", menuPanel.updateVisualStatus);
    }

    onActivePageIdChanged: {
        Events.sendEvent("ActivePageIdChanged", menuPanel.activePageId);
    }

    property bool block: false;
    onActivePageIndexChanged: {
        if (block || activePageIndex < 0){
            return;
        }

        let index = menuPanel.activePageIndex
        NavigationController.push(menuPanel, function(menuPanel){
            menuPanel.block = true;
            menuPanel.activePageIndex = index;
            menuPanel.block = false;
        }
        );
    }

    function onMenuModelRequest(ok){
        Events.sendEvent("MenuModelChanged", menuPanel.model);
    }

    function updateVisualStatus(data){
        if (!data){
            return;
        }

        if (!('Id' in data)){
            return;
        }
        let pageId = data["Id"];

        if ('Name' in data){
            let pageName = data["Name"];

            for (let i = 0; i < menuPanel.model.getItemsCount(); i++){
                let id = menuPanel.model.getData("Id", i);
                if (pageId == id){
                    menuPanel.model.setData("Name", pageName,  i);

                    break;
                }
            }
        }

        if ('Icon' in data){
            let pageIcon = data["Icon"];

            for (let i = 0; i < menuPanel.model.getItemsCount(); i++){
                let id = menuPanel.model.getData("Id", i);
                if (pageId == id){
                    menuPanel.model.setData("Icon", pageIcon,  i);

                    break;
                }
            }
        }
    }

    function clearModels(){
        lvPages.model = 0;
        menuPanel.activePageIndex = -1;
    }

    function changePage(pageId){
        console.log("MenuPanel onChangePage", pageId);
        for(var i = 0; i < lvPages.count; ++i){
            let item = lvPages.itemAtIndex(i);
            if(item.pageId === pageId){
                item.clicked();
            }
        }
    }

    onModelChanged: {
        console.log("MenuPanel onModelChanged", menuPanel.model.toJson())
        let pageIndex = menuPanel.activePageIndex;

        lvPages.model = 0;
        lvPages.model = menuPanel.model;

        Events.sendEvent("MenuModelChanged", menuPanel.model);

    }

    ListView {
        id: lvPages;

        anchors.fill: parent;

        boundsBehavior: Flickable.StopAtBounds;

        Keys.onPressed: {
            if (event.key == Qt.Key_Up){
                if (menuPanel.activePageIndex >= 1){
                    menuPanel.activePageIndex--;
                }
            }

            if (event.key == Qt.Key_Down){
                if (menuPanel.activePageIndex < lvPages.count - 1){
                    menuPanel.activePageIndex++;
                }
            }
        }

        delegate: MenuPanelButton {
            Component.onCompleted: {
                if (model.index === 0 && menuPanel.activePageIndex === -1){
                    this.clicked();
                }
            }

            property string pageName: model["Name"];
            onPageNameChanged: {
                Events.sendEvent("PageNameChanged", {"Id": model["Id"], "Name": model["Name"]})
            }

            text:  model["Name"];
            textColor: Style.textColor;
            fontName: menuPanel.fontName;

            menuPanelRef: menuPanel;

            iconSource: (highlighted || selected) ? "../../../" + Style.getIconPath(model["Icon"], "On", "Selected"):
                                                     "../../../" + Style.getIconPath(model["Icon"], "On", "Normal");

            selected: menuPanel.activePageIndex === model.index;
            property string pageId: model["Id"];

            onClicked: {
                menuPanel.activePageName = model["Name"];
                menuPanel.activeIcon = model["Icon"];
                menuPanel.activePageId = model["Id"];

                menuPanel.activePageIndex = model.index;
                lvPages.forceActiveFocus();
            }
        }
    }
}
