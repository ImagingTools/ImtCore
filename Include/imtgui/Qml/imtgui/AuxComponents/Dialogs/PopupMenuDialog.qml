import QtQuick 2.12
import QtGraphicalEffects 1.12
import Acf 1.0
import imtgui 1.0
import imtqml 1.0

Rectangle {
    id: popupMenuContainer;

    width: itemWidth;
    height: popupMenuListView.height;

    property var model;
    property Item root;

    property int itemWidth: 200;
    property int itemHeight: 26;

    property int textSize: Style.fontSize_common;
    property string fontColor: Style.textColor;

    property bool hiddenBackground: true;

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

    signal finished(string commandId, int index);

    onFinished: {
        root.closeDialog();
    }

    onModelChanged: {
        popupMenuListView.model = model;
    }

    onRootChanged: {
        /**
            Opacity of the background = 0
        */
        if(popupMenuContainer.hiddenBackground){
            root.backgroundItem.opacity = 0;
        }


        /**
            Close the dialog by clicking on the background
        */
        root.backgroundItem.backgroundAreaItem.clicked.connect(root.closeDialog);
    }

    Rectangle {
        id: itemBody;

        width: popupMenuContainer.width;
        height: popupMenuListView.height;

        color: Style.baseColor;

        border.width: 1;
        border.color: Style.alternateBaseColor;

        ListView {
            id: popupMenuListView;

            width: popupMenuContainer.width;
//            height: (parent.countVisibleItem == -1 || parent.countVisibleItem > popupMenuListView.count) ?
//                        popupMenuListView.count * popupMenuContainer.itemHeight :
//                        parent.countVisibleItem * popupMenuContainer.itemHeight;
            height: Math.min(5 * popupMenuContainer.itemHeight, contentHeight);

            boundsBehavior: Flickable.StopAtBounds;
            clip: true;

            delegate: popupMenuContainer.delegate;
        }//ListView
    }//ItemListView

    DropShadow {
       id: dropShadow;

       anchors.fill: itemBody;

       horizontalOffset: 2;
       verticalOffset: 2;

       radius: 4;
       color: Style.shadowColor;

       source: itemBody;
    }
}
