import QtQuick 2.12
import Qt5Compat.GraphicalEffects 6.0
import QtGraphicalEffects 1.12
import Acf 1.0
import imtcontrols 1.0

DecoratorBase {
    id: root;

    property int itemHeight: !baseElement ? 0 : baseElement.itemHeight;
    property int itemWidth: !baseElement ? 0 : baseElement.itemWidth;
    property int shownItemsCount: !baseElement ? 0 : baseElement.shownItemsCount;
    property var delegate: !baseElement ? null : baseElement.delegate;
    property alias contentY: popupMenuListView.contentY;
    property bool moveToEnd: !baseElement ? false : baseElement.moveToEnd;
    property int moveToIndex: !baseElement ? -1 : baseElement.moveToIndex;
    property var model: !baseElement ? -1 : baseElement.model;

    property int selectedIndex: !baseElement ? -1 : baseElement.selectedIndex;

    property alias topContentLoaderSourceComp: topContentLoader.sourceComponent;
    property alias bottomContentLoaderSourceComp: bottomContentLoader.sourceComponent;

//    property alias bottomContentLoaderSourceComp: repeater;

    onModelChanged: {
        popupMenuListView.model = root.model;

        if(root.moveToEnd){
            popupMenuListView.positionViewAtEnd();
        }

        if(root.moveToIndex >= 0 && root.moveToIndex < popupMenuListView.count){
            popupMenuListView.positionViewAtIndex(root.moveToIndex, ListView.Beginning);
        }
    }

    onMoveToIndexChanged: {
        if(root.moveToIndex >= 0 && root.moveToIndex < popupMenuListView.count){
            popupMenuListView.positionViewAtIndex(root.moveToIndex, ListView.Beginning);
        }
    }

    Loader {
        id: topContentLoader;
    }

    Rectangle {
        id: itemBody;

        anchors.top: topContentLoader.bottom;

        width: root.width;
        height: popupMenuListView.height;

        color: Style.baseColor;

        border.width: 1;
        border.color: Style.alternateBaseColor;

        CustomScrollbar {
            id: scrollbar;

            z: 100;

            anchors.right: popupMenuListView.right;
            anchors.bottom: popupMenuListView.bottom;

            secondSize: !root.baseElement ? 0 :
                                            !root.baseElement.visibleScrollBar ? 0 : Style.isMobile == undefined ? 10 : Style.isMobile ? 4 : 10;
            targetItem: popupMenuListView;
            canFade: Style.isMobile == undefined ? false : Style.isMobile;
        }

        Flickable {
            id: popupMenuListView;

            width: root.width;
            height: Math.min(root.shownItemsCount * root.itemHeight, contentHeight)

            contentWidth: width;
            contentHeight: column.height;

            boundsBehavior: Flickable.StopAtBounds;
            clip: true;

            property alias model: repeater.model;
            property alias count: repeater.count;

            function positionViewAtIndex(index, mode){
                if (index >= 0 && index < repeater.count){
                    let y = index * root.itemHeight;

                    popupMenuListView.contentY = y;
                }
            }

            function positionViewAtEnd(){
                if (repeater.count > 0){
                    let lastIndex = repeater.count - 1;

                    positionViewAtIndex(lastIndex, ListView.Beginning);
                }
            }

            Column {
                id: column;
                width: parent.width;

                Repeater {
                    id: repeater;

                    delegate: root.delegate;

                    onItemAdded: {
                        console.log("repeater onItemAdded", item);
                        console.log("item.height", item.height);
                        console.log("repeater count", repeater.count);
                        console.log("column", repeater.height);
                    }
                }
            }
        }

        MouseArea{
            anchors.fill: parent;
            hoverEnabled: true;
            visible: root.baseElement ? root.baseElement.hoverBlocked : true;

            onPositionChanged: {
//                popupMenuContainer.hoverBlocked = false;
                if(root.baseElement){
                    root.baseElement.hoverBlocked = false;
                }
            }
        }
    }//ItemListView

    Loader {
        id: bottomContentLoader;
    }

    DropShadow {
        id: dropShadow;

        anchors.fill: itemBody;

		z: itemBody.z-1

        horizontalOffset: 2;
        verticalOffset: 2;

        radius: 4;
        color: Style.shadowColor;

        source: itemBody;
    }

    function contentYCorrection(down_){
        var contentY = popupMenuListView.contentY;
        var itemHeight = root.itemHeight;
        var visibleCount = root.shownItemsCount;
        var index = root.selectedIndex;

        if(root.baseElement){
            if(root.selectedIndex >= 0){

                if(down_){
                    if((index+1) * itemHeight > contentY + visibleCount * itemHeight){
                        popupMenuListView.contentY = (index+1) * itemHeight - visibleCount * itemHeight
                    }
                }
                else {
                    if(index * itemHeight < contentY){
                        popupMenuListView.contentY = index * itemHeight
                    }
                }
            }
        }
    }
}
