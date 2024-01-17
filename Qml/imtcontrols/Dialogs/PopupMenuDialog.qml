import QtQuick 2.12
import Qt5Compat.GraphicalEffects 6.0
import QtGraphicalEffects 1.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

Item {
    id: popupMenuContainer;

    width: itemWidth;
    height: popupMenuListView.height;

    property var model;
    property Item root: null;
    property Item rootItem: null;

    property int itemWidth: 200;
    property int itemHeight: 26;
    property int shownItemsCount: 5;

    property int textSize: Style.fontSize_common;
    property string fontColor: Style.textColor;

    property bool hiddenBackground: true;
    property bool visibleScrollBar: true;
    property bool moveToEnd: false;
    property int moveToIndex: -1;

    property alias contentY: popupMenuListView.contentY;

    property bool forceFocus: false;

    // ID for display in combo box delegates
    property string nameId: "Name";

    property Component delegate: PopupMenuDelegate{
        width: popupMenuContainer.width;
        height: popupMenuContainer.itemHeight;
        textSize: popupMenuContainer.textSize;
        fontColor: popupMenuContainer.fontColor;

        rootItem: popupMenuContainer;
    }

    /**
        Count of the visible item, if value = -1 then count unlimited
    */
    property int countVisibleItem: -1;

    property int selectedIndex: -1;

    property bool hoverBlocked: true;

    property alias topContentLoaderSourceComp: topContentLoader.sourceComponent;
    property alias bottomContentLoaderSourceComp: bottomContentLoader.sourceComponent;

    signal finished(string commandId, int index);
    signal started();

    //forShortcutEnter
    Component.onCompleted: {
        popupMenuContainer.forceActiveFocus();

        Events.subscribeEvent("AppSizeChanged", onAppSizeChanged);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("DialogBackgroundClicked", popupMenuContainer.onBackgroundClicked)
        Events.unSubscribeEvent("AppSizeChanged", onAppSizeChanged);
    }

    onFinished: {
    }

    onMoveToIndexChanged: {
        if(popupMenuContainer.moveToIndex >= 0 && popupMenuContainer.moveToIndex < popupMenuListView.count){
            popupMenuListView.positionViewAtIndex(popupMenuContainer.moveToIndex, ListView.Beginning);
        }
    }

    onModelChanged: {
        popupMenuListView.model = popupMenuContainer.model;

        if(popupMenuContainer.moveToEnd){
            popupMenuListView.positionViewAtEnd();
        }
        if(popupMenuContainer.moveToIndex >= 0 && popupMenuContainer.moveToIndex < popupMenuListView.count){
            popupMenuListView.positionViewAtIndex(popupMenuContainer.moveToIndex, ListView.Beginning);
        }
    }

    onRootChanged: {
        console.log("PopupMenu onRootChanged", root)
        /**
            Opacity of the background = 0
        */
        if(popupMenuContainer.hiddenBackground){
            popupMenuContainer.root.backgroundItem.opacity = 0;
        }

        /**
            Close the dialog by clicking on the background
        */
        Events.subscribeEvent("DialogBackgroundClicked", popupMenuContainer.onBackgroundClicked)
    }

    function onAppSizeChanged(parameters){
        onBackgroundClicked();
    }

    function onBackgroundClicked(){
        console.log("PopupMenuDialog onBackgroundClicked");
        if (rootItem){
            popupMenuContainer.finished('', rootItem.currentIndex);
        }
    }

    onContentYChanged: {
        popupMenuContainer.rootItem.contentY = popupMenuContainer.contentY;
    }

    Loader {
        id: topContentLoader;
    }

    Rectangle {
        id: itemBody;

        anchors.top: topContentLoader.bottom;

        width: popupMenuContainer.width;
        height: popupMenuListView.height;

        color: Style.baseColor;

        border.width: 1;
        border.color: Style.alternateBaseColor;

        CustomScrollbar {
            id: scrollbar;

            z: 100;

            anchors.right: popupMenuListView.right;
            anchors.bottom: popupMenuListView.bottom;

            backgroundColor: Style.baseColor;

            secondSize: !popupMenuContainer.visibleScrollBar ? 0 : Style.isMobile == undefined ? 10 : Style.isMobile ? 4 : 10;
            targetItem: popupMenuListView;
            canFade: Style.isMobile == undefined ? false : Style.isMobile;
        }

        ListView {
            id: popupMenuListView;

            width: popupMenuContainer.width;
            height: Math.min(popupMenuContainer.shownItemsCount * popupMenuContainer.itemHeight, contentHeight);

            boundsBehavior: Flickable.StopAtBounds;
            clip: true;

            delegate: popupMenuContainer.delegate;

            cacheBuffer: count * popupMenuContainer.itemHeight;
        }//ListView

        MouseArea{
            anchors.fill: parent;
            hoverEnabled: true;
            visible: popupMenuContainer.rootItem ? popupMenuContainer.rootItem.hoverBlocked :
                                                   popupMenuContainer.hoverBlocked;
            onPositionChanged: {
                popupMenuContainer.hoverBlocked = false;
                if(popupMenuContainer.rootItem){
                    popupMenuContainer.rootItem.hoverBlocked = false;
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
        if(popupMenuContainer.rootItem){
            if(popupMenuContainer.rootItem.selectedIndex >=0){
                var contentY = popupMenuListView.contentY;
                var itemHeight = popupMenuContainer.itemHeight;
                var visibleCount = popupMenuContainer.shownItemsCount;
                var index = popupMenuContainer.rootItem.selectedIndex;
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

    Shortcut {
        sequence: "Escape";
        enabled: true;
        onActivated: {
            if (popupMenuContainer.rootItem){
                popupMenuContainer.finished('', popupMenuContainer.rootItem.currentIndex);
            }
            popupMenuContainer.root.closeDialog();
        }
    }

    Shortcut {
        sequence: "Return";
        enabled: true;
        onActivated: {
            if(popupMenuContainer.rootItem){
                if(popupMenuContainer.rootItem.selectedIndex >=0){
                    var id = popupMenuContainer.model.GetData("Id", popupMenuContainer.rootItem.selectedIndex);
                    popupMenuContainer.rootItem.finished(id, popupMenuContainer.rootItem.selectedIndex);
                }
            }
        }
    }

    Shortcut {
        sequence: "Up";
        enabled: true;
        onActivated: {
            popupMenuContainer.hoverBlocked = true;
            if(popupMenuContainer.rootItem){
                popupMenuContainer.rootItem.hoverBlocked = true;
                if(popupMenuContainer.rootItem.selectedIndex > 0){
                    popupMenuContainer.rootItem.selectedIndex--;
                    popupMenuContainer.contentYCorrection(false);
                }
            }

        }
    }
    Shortcut {
        sequence: "Down";
        enabled: true;
        onActivated: {
            popupMenuContainer.hoverBlocked = true;
            if(popupMenuContainer.rootItem){
                popupMenuContainer.rootItem.hoverBlocked = true;
                if(popupMenuContainer.rootItem.selectedIndex < popupMenuContainer.model.GetItemsCount() - 1){
                    popupMenuContainer.rootItem.selectedIndex++;
                    popupMenuContainer.contentYCorrection(true);

                }
            }
        }
    }
}
