import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

ControlBase {
    id: container;

    width: container.contentWidth;
    height: subPagesColumn.visible ? container.contentHeight + subPagesColumn.height : container.contentHeight;

    property string text: "Test";
    property string imageSource: "";
    property string textColor: "#335777";//"#191970"
    property string fontName: "";

    property bool enabled:  true;
    property bool selected: false;
    property bool highlighted: ma.containsMouse && container.enabled;

    property real imageDecrease: 0.63;
    property real imageSelectedCoeff: 0.73;
    property real fontSize: 11;

    property int contentWidth: menuPanelButtonDecorator.width
    property int contentHeight: menuPanelButtonDecorator.height

    property string decoratorSource;

    signal clicked;

    onDecoratorSourceChanged: {
        console.log("onDecoratorSourceChanged", container.decoratorSource);
        loaderDecorator.source = container.decoratorSource;
    }

    onContentWidthChanged: {
        if (menuPanel.contentWidth != container.contentWidth){
            menuPanel.contentWidth = container.contentWidth;
        }
    }


    Component.onCompleted: {
        if (model.SubPages){
            subPagesRepeater.model = model.SubPages;
        }
    }

    MenuPanelButtonDecorator {
        id: menuPanelButtonDecorator
        imageSource: container.imageSource
        highlighted: container.highlighted
        selected: container.selected
        title: container.text
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

                        console.log("pagesManager.activeItem", pagesManager.activeItem);
                        pagesManager.activeItem.startPageObj = {"Id": model.PageId,
                            "Name": model.Name,
                            "Source": model.StartItem,
                            "CommandId": model.PageId};
                    }
                }
            }
        }
    }
}
