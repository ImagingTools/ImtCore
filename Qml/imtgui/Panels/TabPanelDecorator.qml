import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

DecoratorBase {
    id: tabPanelDecorator;

    width: texttabDelegate.width + imagetabDelegate.width + 40;
    height: baseElement ? baseElement.height : 50


    property string firstElementImageSource: tabPanelDecorator.baseElement ? tabPanelDecorator.baseElement.firstElementImageSource : "";
    onFirstElementImageSourceChanged: {
        if (tabPanelDecorator.firstElementImageSource !== ""){
            firsElementImage.source = "../../../" + Style.getIconPath(tabPanelDecorator.firstElementImageSource, Icon.State.On, Icon.Mode.Normal);
        }
    }

    onWidthChanged: {
        console.log("tabPanelDecorator onWidthChanged", width);
    }

    Rectangle {
        anchors.fill: tabPanelDecorator;
        color: tabPanelDecorator.baseElement ? tabPanelDecorator.baseElement.selected ? Style.baseColor: "transparent": "transparent";
    }

    Rectangle{
        id: selection;

        anchors.bottom: tabPanelDecorator.bottom;
        anchors.left: tabPanelDecorator.left;
        anchors.right: tabPanelDecorator.right;

        height: 2;

        color: Style.tabSelectedColor;
        visible: tabPanelDecorator.baseElement ? tabPanelDecorator.baseElement.selected: false;
    }

    Item {
        id: imagetabDelegate;

        height: tabPanelDecorator.height;
        width: visible ? height : 1;

        visible: tabPanelDecorator.baseElement ? tabPanelDecorator.baseElement.firstElement: false;

        Image {
            id: firsElementImage;

            anchors.centerIn: imagetabDelegate;

            width: imagetabDelegate.width * 0.6;
            height: imagetabDelegate.height * 0.6;

            sourceSize.width: width;
            sourceSize.height: height;

            fillMode: Image.PreserveAspectFit;
        }
    }

    Item {
        id: texttabDelegate;

        anchors.top: tabPanelDecorator.top;
        anchors.bottom: tabPanelDecorator.bottom;
        anchors.left: imagetabDelegate.right;
        anchors.leftMargin: tabPanelDecorator.baseElement ? tabPanelDecorator.baseElement.firstElement ? 0 :10 : 0;

        width: text.width;

        Text {
            id: text;

            anchors.centerIn: texttabDelegate;

            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
            text: tabPanelDecorator.baseElement ? tabPanelDecorator.baseElement.text : "";
        }
    }

    Item {
        id: closeButton;
        anchors.right: tabPanelDecorator.right;
        anchors.rightMargin: 8;
        anchors.verticalCenter: tabPanelDecorator.verticalCenter;

        height: tabPanelDecorator.height;
        width: closeImage.width;

        visible: !tabPanelDecorator.baseElement.firstElement && tabPanelDecorator.baseElement.isCloseEnable;

        Image {
            id: closeImage;

            anchors.centerIn: closeButton;

            width: 13;
            height: width;

            sourceSize.width: width;
            sourceSize.height: height;

            fillMode: Image.PreserveAspectFit;
            source: "../../../" + Style.getIconPath("Icons/Close", Icon.State.On, Icon.Mode.Normal);
        }

        MouseArea{
            id: closeMA;

            anchors.fill: closeButton;

            enabled: closeButton.visible;
            hoverEnabled: enabled;
            cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor;

            onClicked: {
                tabPanelDecorator.baseElement.closeSignal();
            }
        }
    }
}

