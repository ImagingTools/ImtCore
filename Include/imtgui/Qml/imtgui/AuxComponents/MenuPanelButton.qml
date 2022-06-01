import QtQuick 2.12
import Acf 1.0

Item {
    id: container;

    width: 64;
    height: 64;
//    height: 50 + subPagesColumn.height;
//    height: subPagesColumn.visible ? 50 + subPagesColumn.height : 50;
//    height: 200;

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


    property string decoratorSource;
    signal clicked;

    onDecoratorSourceChanged: {
        console.log("container.decoratorSource", container.decoratorSource);
//        loaderDecorator.item = container.decoratorItem;
        loaderDecorator.source = container.decoratorSource;
    }

    onSelectedChanged: {
        loaderDecorator.item.selected = container.selected;
    }

    Component.onCompleted: {
        if (model.SubPages){
            console.log("model.SubPages", model.SubPages, model.Name);
            subPagesRepeater.model = model.SubPages;
        }
    }

    Rectangle {
        id: mainRect;

        anchors.top: container.top;

        width: container.width;
        height: 50;

        color: "transparent";

        Loader {
            id: loaderDecorator;

            onItemChanged: {
                loaderDecorator.item.width = container.width;
                loaderDecorator.item.height = container.height;
                loaderDecorator.item.imageSource = container.imageSource;
                loaderDecorator.item.highlighted = container.highlighted;
                loaderDecorator.item.selected = container.selected;
                loaderDecorator.item.title = container.text;
            }
        }

        MouseArea{
            id: ma;

            anchors.fill: parent;

            hoverEnabled: enabled;
            cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor;

            onClicked: {
                container.clicked();
//                subPagesColumn.visible = !subPagesColumn.visible;
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
                    subPagesDecorator.source = container.decoratorSource;
                }

                onSelectedChanged: {
                    subPagesDecorator.item.selected = subPageDelegate.selected;
                }

                Loader {
                    id: subPagesDecorator;
                    onItemChanged: {
                        console.log("model.Icon", model.Icon);
                        console.log("model.Name", model[PageEnum.NAME]);
                        subPagesDecorator.item.width = container.width;
                        subPagesDecorator.item.height = container.height;

//                        subPagesDecorator.item.imageSource = "../../../" + "Icons/" + Style.theme + "/" + model[PageEnum.ICON] + "_" + "On" + "_" + "Normal" + ".svg"
//                        subPagesDecorator.item.imageSource = model.Icon;
                        subPagesDecorator.item.selected = subPageDelegate.selected;
                        subPagesDecorator.item.title = model[PageEnum.NAME];
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

//    Image {
//        id: image;

//        anchors.horizontalCenter: parent.horizontalCenter;
//        anchors.top: parent.top;
//        anchors.topMargin: 5;

//        width: container.highlighted || container.selected ? parent.width * imageSelectedCoeff : parent.width * imageDecrease;
//        height:  container.highlighted || container.selected ? parent.height * imageSelectedCoeff : parent.height * imageDecrease;

//        fillMode: Image.PreserveAspectFit;

//        source: container.imageSource;
//        sourceSize.height: height;
//        sourceSize.width: width;
//    }

//    Rectangle {
//        anchors.top: parent.top;
//        anchors.bottom: parent.bottom;
//        anchors.left: parent.left;

//        width: 5;

//        color: Style.iconColorOnSelected;
//        visible: container.selected;
//    }

//    Text {
//        id: description;

//        anchors.bottom: parent.bottom;
//        anchors.horizontalCenter: container.horizontalCenter;

//        text: container.text;
//        color: container.selected ? Style.iconColorOnSelected: Style.textColor;
//        font.pixelSize: container.fontSize;
//        font.family: Style.fontFamily;
//    }
}
