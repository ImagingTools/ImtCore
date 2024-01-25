import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

ControlBase {
    id: container;

    width: container.contentWidth;
    height: subPagesColumn.visible ? container.contentHeight + subPagesColumn.height : container.contentHeight;

    decorator: Style.menuPanelButtonDecorator;

    property string text: "Test";
    property string iconSource: !container.baseElement ? "" : container.baseElement.iconSource;
    property string textColor: "#335777";//"#191970"
    property string fontName: "";

    property bool enabled:  true;
    property bool selected: false;
    property bool highlighted: ma.containsMouse && container.enabled;

    property real imageDecrease: 0.63;
    property real imageSelectedCoeff: 0.73;
    property real fontSize: 11;

    property int contentWidth: 80;
    property int contentHeight: 80;

    property string decoratorSource;

    property Item menuPanelRef: null;

    signal clicked;

    onContentWidthChanged: {
        if (!menuPanelRef){
            return;
        }

        if (menuPanelRef.contentWidth != container.contentWidth){
            menuPanelRef.contentWidth = container.contentWidth;
        }
    }

    Component.onCompleted: {
        if (model.SubPages){
            subPagesRepeater.model = model.SubPages;
        }
    }

    Rectangle {
        id: mainRect;

        anchors.top: container.top;

        width: container.contentWidth;
        height: container.contentHeight;

        color: "transparent";

        MouseArea{
            id: ma;

            anchors.fill: parent;

            hoverEnabled: enabled;
            cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor;

            onClicked: {
                container.clicked();
            }
        }
    }

    Column {
        id: subPagesColumn;

        anchors.top: mainRect.bottom;

        visible: container.selected;

        width: parent.width;

        property int currentIndex: 0;

        onCurrentIndexChanged: {

        }

        Repeater {
            id: subPagesRepeater;

            delegate: SubMenuPanelButtonDecorator {
                id: subPageDelegate;

                width: subPagesColumn.width;
                height: container.height

                selected: subPagesColumn.currentIndex == model.index;
                title: model["Name"]


                MouseArea{
                    anchors.fill: parent;

                    hoverEnabled: enabled;
                    cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor;

                    onClicked: {
                        subPagesColumn.currentIndex = model.index;

//                        console.log("pagesManager.activeItem", pagesManager.activeItem);
//                        pagesManager.activeItem.startPageObj = {"Id": model.PageId,
//                            "Name": model.Name,
//                            "Source": model.StartItem,
//                            "CommandId": model.PageId};
                    }
                }
            }
        }
    }
}
