import QtQuick 2.12
import QtGraphicalEffects 1.12
import Acf 1.0
import imtqml 1.0

Item {
    id: popupMenuContainer;
    width: popupMenuContainer.itemWidth;

    property Item resultItem;
    property Item loaderDialog;
    property Item thumbnailItem;

    property var model;
    property real backgroundOpacity: 0;

    property bool backgroundExist: false;
    property bool clickBackgroundClose: true;
    property bool centered: false;
    property bool hasIcon: true;

    property int itemWidth: 100;
    property int itemHeight: 26;
    property int emptyItemCount: 0;

    function exit(index, name) {
        var parameters  = {};
        parameters["index"] = index;
        parameters["status"] = name;
        parameters["dialog"] = "PopupMenu";
        popupMenuContainer.resultItem.dialogResult(parameters);
    }

    onFocusChanged: {
        console.log("PopupMenuDialog onFocusChanged");
        mainBody.forceActiveFocus();
    }

    onModelChanged: {
        console.log("PopupMenuDialog ListView onModelChanged", popupMenuContainer.model.count);
        for (var i = 0; i < popupMenuContainer.model.count; i++) {
            if (popupMenuContainer.model.get(i).id === "") {
                popupMenuContainer.emptyItemCount++;
            }
        }
    }

    DropShadow {
       id: dropShadow;

       anchors.fill: mainBody;

       horizontalOffset: 2;
       verticalOffset: 2;

       radius: 4;
       samples: 10;
       color: Style.shadowColor;
       source: mainBody;
    }

    Rectangle {
        id: mainBody;

        width: parent.width;

        color: Style.baseColor;
        radius: 3;
        clip: true;

        border.width: 1;

        border.color: Style.alternateBaseColor;

        Keys.onPressed: {
            console.log("PopupMenuDialog mainBody keys pressed")
            if (event.key === Qt.Key_Up){
                console.log('Key up was pressed');

                if (popupMenuListView.currentIndex == 0){
                    popupMenuListView.currentIndex = popupMenuListView.count - 1;
                }
                else
                    popupMenuListView.currentIndex--;
            }
            else if (event.key === Qt.Key_Down){
                console.log('Key down was pressed');
                if (popupMenuListView.currentIndex == popupMenuListView.count){
                    popupMenuListView.currentIndex = 0;
                }
                else
                    popupMenuListView.currentIndex++;
            }
            else if (event.key === Qt.Key_Return){
                console.log('Key return was pressed');
                popupMenuListView.itemAtIndex(popupMenuListView.currentIndex).click();
            }
            else if (event.key === Qt.Key_Escape){
                console.log('Key esc was pressed');

                popupMenuContainer.resultItem.dialogResult();
                loaderDialog.closeItem();
            }
        }

        ListView {
            id: popupMenuListView;

            width: parent.width;
            height: parent.height;
            model: popupMenuContainer.model;

            onModelChanged: {
                console.log("PopupMenuDialog onModelChanged");

                var height = popupMenuListView.count * popupMenuContainer.itemHeight -
                        popupMenuContainer.emptyItemCount * popupMenuContainer.itemHeight;

                if (height > 130){
                    mainBody.height = 130;
                }
                else {
                    mainBody.height = height;
                }
            }

            delegate: Rectangle {
                id: delegateListViewPopup;

                width: popupMenuContainer.width;
                height: model.id !== "" ? popupMenuContainer.itemHeight : 0;

                color: "transparent";

                function click(){
                    popupMenuContainer.exit(model.index, model.name);
                    loaderDialog.closeItem();
                }

                Rectangle {
                    id: highlightRect;

                    anchors.horizontalCenter: parent.horizontalCenter;
                    anchors.verticalCenter: parent.verticalCenter;

                    width: parent.width - 2;
                    height: parent.height - 2;

                    color: Style.selectedColor;
                    visible: (model.index === popupMenuListView.currentIndex ||
                              delegateListViewPopupMA.containsMouse) && model.id !== "" && model.mode !== "Disabled";
                }

                Text {
                    anchors.left: popupMenuContainer.hasIcon ? iconDelegateListViewPopup.right : delegateListViewPopup.left;
                    anchors.verticalCenter: iconDelegateListViewPopup.verticalCenter;
                    anchors.leftMargin: 10;
                    anchors.rightMargin: 10;

                    text: model.name;
                    color: model.mode === "Disabled" ? Style.disabledInActiveTextColor : Style.textColor;
                    font.pixelSize: Style.fontSize_common;
                    font.family: Style.fontFamily;
                }

                Image {
                    id: iconDelegateListViewPopup;

                    anchors.left: delegateListViewPopup.left;
                    anchors.verticalCenter: delegateListViewPopup.verticalCenter;
                    anchors.leftMargin: 10;

                    width: 18;
                    height: width;

                    visible: popupMenuContainer.hasIcon && model.name !== "";
                    source: model.imageSource;
                    sourceSize.width: width;
                    sourceSize.height: height;
                }

                Rectangle {
                    anchors.horizontalCenter: parent.horizontalCenter;
                    anchors.bottom: delegateListViewPopup.bottom;

                    width: parent.width - 20;
                    height: 1;

                    visible: model.id === "";

                    color: Style.hover;
                }

                MouseArea {
                    id: delegateListViewPopupMA;

                    anchors.fill: parent;

                    hoverEnabled: true;
                    cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor;

                    visible: model.mode !== "Disabled";

                    onClicked: {
                        delegateListViewPopup.click();
                    }
                }
            }
        }
    }
}
