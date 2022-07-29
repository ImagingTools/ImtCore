import QtQuick 2.12
import Acf 1.0

Item {
    id: multiDocView;

    anchors.fill: parent;

    property TabPanel tabPanel: tabPanelInternal;

    property Item activeItem;
    property Item rootItem;

    property alias pagesCount: docsData.count;
    property alias firstElementImageSource: tabPanelInternal.firstElementImageSource;

    property string operation;

    property int mainMargin: 0;

    Component.onCompleted: {
        console.log("MultidocWorkspaceView onCompleted", tabPanelInternal.selectedIndex)
        docsData.anchors.topMargin = tabPanelInternal.height;
    }

    onFocusChanged: {
        console.log("MultidocWorkspaceView onFocusChanged", multiDocView.focus);

        if (multiDocView.focus){
            multiDocView.activeItem.forceActiveFocus();
        }
    }

    function setFocus(){
        console.log("MultidocWorkspaceView setFocus");
        multiDocView.forceActiveFocus();
    }

    function addPage(page){
        console.log("MultidocWorkspaceView addPage", page)

        let itemId = page["Id"];

        let pageIndex = multiDocView.getTabIndexById(itemId);
        if (pageIndex < 0){
            var index = pagesData.InsertNewItem();
            pagesData.SetData("ItemId", itemId, index);
            pagesData.SetData("Title", page["Name"], index);
            pagesData.SetData("Source", page["Source"], index);
            pagesData.SetData("CommandsId", page["CommandsId"], index);

            pageIndex = index;
        }

        tabPanelInternal.selectedIndex = pageIndex;
    }

    function closeTab(index) {
        console.log("MultiDocWorkspaceView closeTab", index);
        pagesData.RemoveItem(index)
        if (tabPanelInternal.selectedIndex >= index){
            tabPanelInternal.selectedIndex--;
        }
    }

    function updateTitleTab(itemId, title, index) {
        console.log("MultidocWorkspaceView updateTitleTab()", itemId, title, index);
        pagesData.SetData("Title", title, index);
        pagesData.SetData("ItemId", itemId, index);
    }

    function getTabIndexById(tabId){
        console.log("MultidocWorkspaceView getTabIndexById", tabId);

        for (var i = 1; i < pagesData.GetItemsCount(); i++){
            var id = pagesData.GetData("ItemId", i);
            if (id === tabId){
                return i;
            }
        }

        return -1;
    }

    TreeItemModel {
        id: pagesData;
    }

    TabPanel {
        id: tabPanelInternal;

        anchors.left: parent.left;
        anchors.leftMargin: multiDocView.mainMargin;
        anchors.right: parent.right;
        anchors.rightMargin: multiDocView.mainMargin;

        visible: true;
        model: pagesData;

        onCloseItem: {
            console.log("MultiDocWorkspaceView TabPanel onCloseItem", index)
            multiDocView.closeTab(index);
        }

        onRightClicked: {
            if (tabPanelInternal.selectedIndex < pagesData.GetItemsCount() - 1) {
                tabPanelInternal.selectedIndex++;
                tabPanelInternal.viewTabInListView(tabPanelInternal.selectedIndex);
            }
        }

        onLeftClicked: {
            if (tabPanelInternal.selectedIndex > 0) {
                tabPanelInternal.selectedIndex--;
                tabPanelInternal.viewTabInListView(tabPanelInternal.selectedIndex);
            }
        }
    }

    ListView {
        id: docsData;

        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.top: parent.top;
        anchors.topMargin: tabPanelInternal.height;
        anchors.bottom: parent.bottom;

        clip: true;
        boundsBehavior: Flickable.StopAtBounds;
        orientation: ListView.Horizontal;
        model: pagesData;

        property int  currentIndex: -1;

        delegate: Item {
            id: docsDataDeleg;

            width: visible ? docsData.width : 0;
            height: docsData.height;

            visible: tabPanelInternal.selectedIndex == model.index;

            onVisibleChanged: {
                if(docsDataDeleg.visible){
                    multiDocView.activeItem = dataLoader.item;
                }
            }

            function getItem(){
                return dataLoader.item;
            }

            Loader {
                id: dataLoader;

                anchors.fill: parent;

                Component.onCompleted: {
                    console.log("dataLoader onCompleted", model.Source);
                    dataLoader.source = model.Source;
                }

                onItemChanged: {
                    console.log("MultidocWorkspaceView dataLoader onItemChanged", dataLoader.source, docsDataDeleg)
                    if (dataLoader.item){
                        dataLoader.item.rootItem = multiDocView;
                        dataLoader.item.itemId = model.ItemId
                        dataLoader.item.itemName = model.Title
                        dataLoader.item.commandsId = model.CommandsId

                        docsData.currentIndex = model.index;
                    }
                }
            }
        }
    }
}
