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
        console.log("onDecoratorSourceChanged", container.decoratorSource);
        loaderDecorator.source = container.decoratorSource;
    }

    onContentWidthChanged: {
        if (menuPanel.contentWidth != container.contentWidth){
            menuPanel.contentWidth = container.contentWidth;
        }
    }

    onSelectedChanged: {
        console.log("onSelectedChanged", container.selected);
        if (loaderDecorator.item){
            loaderDecorator.item.selected = container.selected;
        }
    }

    onHighlightedChanged: {
        if (loaderDecorator.item){
            loaderDecorator.item.highlighted = container.highlighted;
        }
    }

    onImageSourceChanged: {
        if (loaderDecorator.item){
            loaderDecorator.item.imageSource = container.imageSource;
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

                    console.log("container.contentWidth", container.contentWidth);
                    console.log("container.contentHeight", container.contentHeight);
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

        onCurrentIndexChanged: {

        }

        Repeater {
            id: subPagesRepeater;

            delegate: Item {
                id: subPageDelegate;

                width: subPagesColumn.width;
                height: 50;

                property bool selected: subPagesColumn.currentIndex == model.index;

                onSelectedChanged: {
                    if (subPagesDecorator.item){
                        subPagesDecorator.item.selected = subPageDelegate.selected;
                    }
                }

                Loader {
                    id: subPagesDecorator;

                    source: Style.subMenuButtonDecoratorPath;

                    onLoaded: {
                        subPagesDecorator.item.width = container.width;
                        subPagesDecorator.item.height = container.height;
                        subPagesDecorator.item.selected = subPageDelegate.selected;
                        subPagesDecorator.item.title = model["Name"];
                    }
                }

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
                            "CommandsId": model.PageId};
                    }
                }
            }
        }
    }
}
