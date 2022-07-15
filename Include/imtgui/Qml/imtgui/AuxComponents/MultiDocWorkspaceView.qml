import QtQuick 2.12
import Acf 1.0

Rectangle {
    id: multiDocView;

    height: 100;
    width: 100;

    color: "transparent";

    property TabPanel tabPanel: tabPanelInternal;

    property Item activeItem;
    property Item activeCollectionItem;
    property Item rootItem;

    property var pagesSources: [];
    property var pagesItems: [];

    property alias pagesCount: docsData.count;
    property alias firstElementImageSource: tabPanelInternal.firstElementImageSource;

    property string operation;

    property int mainMargin: 0;

    onFocusChanged: {
        console.log("MultidocWorkspaceView onFocusChanged", multiDocView.focus);

        if (multiDocView.focus){
            multiDocView.activeItem.forceActiveFocus();
        }
    }

    Component.onCompleted: {
        console.log("MultidocWorkspaceView onCompleted", tabPanelInternal.selectedIndex)
        docsData.anchors.topMargin = tabPanelInternal.height;
    }

    function setFocus(){
        console.log("MultidocWorkspaceView setFocus");
        multiDocView.forceActiveFocus();
    }

    function menuActivated(menuId){
        multiDocView.activeItem.menuActivated(menuId);
    }

    function updateCommandId(){
        console.log("MultidocWorkspaceView updateCommandId", tabPanelInternal.selectedIndex)
        if (tabPanelInternal.selectedIndex > -1 && multiDocView.rootItem){
            var commandsId = pagesData.GetData("CommandsId", tabPanelInternal.selectedIndex);
            var itemId = pagesData.GetData("ItemId", tabPanelInternal.selectedIndex);
            multiDocView.rootItem.changeCommandsId(commandsId);
        }
    }

    function commandsChanged(commandsId){
        console.log("MultidocWorkspaceView commandsChanged")

        if (multiDocView.activeItem){
            multiDocView.activeItem.commandsChanged(commandsId);
        }
    }

    function tabIsOpened(itemId) {
        for (var i = 0; i < pagesData.GetItemsCount(); i++) {
            var pageId = pagesData.GetData("ItemId", i);
            if (pageId === itemId) {
                tabPanelInternal.selectedIndex = i;
                return true;
            }
        }
        return false;
    }

    function addToHeadersArray(itemId, title, source, commandsId, operation){
        console.log("MultidocWorkspaceView addToHeadersArray", title, source, itemId, commandsId, operation)
        if (operation) {
            multiDocView.operation = operation;
        }
        var findPage = false;
        if (itemId !== "" && multiDocView.tabIsOpened(itemId)) {
            findPage = true;
        }

        console.log("findPage", findPage);
        if (!findPage) {
            var index = pagesData.InsertNewItem();
            pagesData.SetData("ItemId", itemId, index);
            pagesData.SetData("Title", title, index);
            pagesData.SetData("Source", source, index);
            pagesData.SetData("CommandsId", commandsId, index);

            tabPanelInternal.selectedIndex = pagesData.GetItemsCount() - 1;
        }
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

    function reloadModelInOpenTab(index){
        let delegateItem = docsData.itemAtIndex(index);
        let itemLink = delegateItem.getItem();
        itemLink.reloadModel();
    }

    function getTabIndexById(tabId){
        console.log("MultidocWorkspaceView getTabIndexById", tabId);
        if (!pagesData){
            return;
        }

        for (var i = 1; i < pagesData.GetItemsCount(); i++){
            var id = pagesData.GetData("ItemId", i);
            if (id === tabId){
                return i;
            }
        }

        return -1;
    }

    function enableChanges(){
        console.log("MultidocWorkspaceView enableChanges");

        let delegateItem = docsData.itemAtIndex(tabPanelInternal.selectedIndex);
        let itemLink = delegateItem.getItem();
        delegateItem.updateTitleTab(itemLink.itemName, itemLink.itemName + "*");
//        delegateItem.updateTitleTab(itemLink.itemId, itemLink.itemId + "*");
        itemLink.wasChanged = true;

        multiDocView.rootItem.setModeMenuButton("Save", "Normal");
    }

    function disableChanges(){
        console.log("MultidocWorkspaceView disableChanges");

        let delegateItem = docsData.itemAtIndex(tabPanelInternal.selectedIndex);
        let itemLink = delegateItem.getItem();
//        delegateItem.updateTitleTab(itemLink.itemId, itemLink.itemId);
        delegateItem.updateTitleTab(itemLink.itemName, itemLink.itemName);
        itemLink.wasChanged = false;

        multiDocView.rootItem.setModeMenuButton("Save", "Disabled");
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
//            multiDocView.closeTab(index);
            tabPanelInternal.selectedIndex = index;
            let delegateItem = docsData.itemAtIndex(index);
            let itemLink = delegateItem.getItem();
            itemLink.menuActivated("Close");
        }

        onSelectedIndexChanged: {
            console.log("MultiDocWorkspaceView TabPanel onSelectedIndexChanged", tabPanelInternal.selectedIndex);
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

        onFocusChanged: {
            console.log("MultiDocWorkspaceView ListView onFocusChanged");
        }

        property int  currentIndex: -1;

        delegate: Rectangle {
            id: docsDataDeleg;

            width: visible ? docsData.width : 0;
            height: docsData.height;

            color: "transparent";
            visible: tabPanelInternal.selectedIndex === model.index;

            onVisibleChanged: {
                console.log("MultiDocView ListView onVisibleChanged", this.visible);

                if(this.visible){
                    multiDocView.activeItem = dataLoader.item;
                    multiDocView.activeItem.refresh();
                    multiDocView.updateCommandId();
                }
            }

            Component.onCompleted: {
                console.log("MultiDocView ListView onCompleted", tabPanelInternal.selectedIndex);
            }

            function setModeMenuButton(commandId, mode){
                console.log("MultiDocView ListView setModeMenuButton", commandId, mode);
                multiDocView.rootItem.setModeMenuButton(commandId, mode);
            }

            function updateTitleTab(itemId, name) {
                console.log("MultiDocView ListView updateTitleTab",itemId,  name);
                multiDocView.updateTitleTab(itemId, name, tabPanelInternal.selectedIndex);
            }

            function closeTab() {
                multiDocView.closeTab(tabPanelInternal.selectedIndex);
            }

            function getItem(){
                return dataLoader.item;
            }

            Loader {
                id: dataLoader;

                anchors.fill: parent;

                Component.onCompleted: {
                    dataLoader.source = model.Source;
                }

                onItemChanged: {
                    console.log("MultidocWorkspaceView dataLoader onItemChanged", dataLoader.source, docsDataDeleg)
                    if (dataLoader.item && dataLoader.source != ""){

                        console.log("docsDataDeleg", docsDataDeleg);
                        dataLoader.item.rootItem = docsDataDeleg;
                        console.log("dataLoader.item.rootItem", dataLoader.item.rootItem);

                        dataLoader.item.itemId = model.ItemId
                        console.log("dataLoader.item.itemId", dataLoader.item.itemId);
                        dataLoader.item.itemName = model.Title
                        dataLoader.item.operation = multiDocView.operation

                        docsData.currentIndex = model.index;

                        var dataModelLocal
                        if (pagesData.ContainsKey("DocsData",model.index)){
                            dataModelLocal = pagesData.GetData("DocsData", model.index);
                        }
                        else {
                            dataModelLocal = pagesData.AddTreeModel("DocsData", model.index)
                        }

                        dataLoader.item.multiDocViewItem = multiDocView;
                        dataLoader.item.model = dataModelLocal
//                        dataLoader.item.rootItem = docsDataDeleg;

                        if (tabPanelInternal.selectedIndex === model.index) {
                            multiDocView.activeItem = dataLoader.item;
                            multiDocView.updateCommandId();
                        }

//                        multiDocView.activeItem.forceActiveFocus();
                    }
                }

            }
        }
    }

}
