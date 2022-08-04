import QtQuick 2.12
import Acf 1.0

Item {
    id: container;

    width: container.contentWidth;
    height: subPagesColumn.visible ? container.contentHeight + subPagesColumn.height : container.contentHeight;

    property string text: "Test";
    property string imageSource: "../Icons/FeaturePackage.svg";
    property string textColor: "#335777";//"#191970"
    property string fontName: "";

    property bool enabled:  true;
    property bool selected: false;
    property bool highlighted: ma.containsMouse && container.enabled;

    property real imageDecrease: 0.63;
    property real imageSelectedCoeff: 0.73;
    property real fontSize: 11;

    property int contentWidth;
    property int contentHeight;

    property string decoratorSource;
    signal clicked;

    onDecoratorSourceChanged: {
        loaderDecorator.source = container.decoratorSource;
    }

    onContentWidthChanged: {
        if (menuPanel.width != container.contentWidth){
            menuPanel.width = container.contentWidth;
        }
    }

    onSelectedChanged: {
        loaderDecorator.item.selected = container.selected;
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

        Loader {
            id: loaderDecorator;

            onItemChanged: {
                if (loaderDecorator.item){
                    loaderDecorator.item.imageSource = container.imageSource;
                    loaderDecorator.item.highlighted = container.highlighted;
                    loaderDecorator.item.selected = container.selected;
                    loaderDecorator.item.title = container.text;

                    container.contentWidth = loaderDecorator.item.width;
                    container.contentHeight = loaderDecorator.item.height;
                }
            }
        }

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

        Repeater {
            id: subPagesRepeater;

            delegate: Item {
                id: subPageDelegate;

                width: subPagesColumn.width;
                height: 50;

                property bool selected: subPagesColumn.currentIndex == model.index;

                Component.onCompleted: {
                    subPagesDecorator.source = Style.subMenuButtonDecoratorPath;
                }

                onSelectedChanged: {
                    subPagesDecorator.item.selected = subPageDelegate.selected;
                }

                Loader {
                    id: subPagesDecorator;
                    onItemChanged: {
                        if (subPagesDecorator.item){
                            subPagesDecorator.item.width = container.width;
                            subPagesDecorator.item.height = container.height;
                            subPagesDecorator.item.selected = subPageDelegate.selected;
                            subPagesDecorator.item.title = model["Name"];
                        }
                    }
                }

                MouseArea{
                    anchors.fill: parent;

                    hoverEnabled: enabled;
                    cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor;

                    onClicked: {
                        subPagesColumn.currentIndex = model.index;
                    }
                }
            }
        }
    }
}
