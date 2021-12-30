import QtQuick 2.12
import Acf 1.0
//import '../UxAdvancedComponents' as AUX


Rectangle {
    id: multiDocView;
    height: 100;
    width: 100;
    color: "transparent";
    property TabPanel tabPanel: tabPanelInternal;
    property Item activeItem;
    property var pagesSources: [];
    property var pagesItems: [];

    function addToHeadersArray(itemId, title, source){
        console.log("addToHeadersArray", title,source,itemId)
        multiDocView.pagesItems.push(itemId);
        multiDocView.pagesSources.push(source);
        tabPanelInternal.addToHeadersArray(title);
    }

    TabPanel {
        id:tabPanelInternal;
        anchors.left: parent.left;
        anchors.right: parent.right;
        visible: true;
    }

    Repeater {
        id: pagesData;
//            anchors.fill: parent;
        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.top: tabPanelInternal.bottom;
        anchors.bottom: parent.bottom;
        clip: true;
//            boundsBehavior: Flickable.StopAtBounds;
//            orientation: ListView.Horizontal;
//            spacing: 0;
        model: tabPanelInternal.count;
        delegate: Rectangle {
            id: pagesDeleg;
            anchors.top: tabPanelInternal.bottom;
            anchors.bottom: multiDocView.bottom;
            width: multiDocView.width;
//            height: multiDocView.height;
            color: "transparent";
            visible: tabPanelInternal.selectedIndex === model.index;
            onVisibleChanged: {
                multiDocView.activeItem = this;
            }

            Loader {
                id: loader;
                anchors.fill: parent;
                Component.onCompleted: {
                    loader.source = multiDocView.pagesSources[model.index];
                    console.log("MultiDoc source",loader.source)
                }
                onItemChanged: {
                    if (loader.item){
                        loader.item.itemId = multiDocView.pagesItems[model.index];
//                        loader.item.gqlModelInfo = multiDocView.pagesModelInfo
//                        loader.item.gqlModelItems = multiDocView.pagesModelItems
                    }
                }

            }
        }
    }

}
