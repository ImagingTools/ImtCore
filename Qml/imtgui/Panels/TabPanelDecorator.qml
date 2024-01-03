import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

Item {
    id: tabPanelDecorator;
    width: texttabDelegate.width + imagetabDelegate.width + 40;
    height: baseElement ? baseElement.height : 50

    property Item baseElement: null;

    property string firstElementImageSource: tabPanelDecorator.baseElement ? tabPanelDecorator.baseElement.firstElementImageSource : "";
    onFirstElementImageSourceChanged: {
        if (tabPanelDecorator.firstElementImageSource !== ""){
            firsElementImage.source = "../../../" + Style.getIconPath(tabPanelDecorator.firstElementImageSource, Icon.State.On, Icon.Mode.Normal);
        }
    }

    Rectangle {
        anchors.fill: parent;
        color: tabPanelDecorator.baseElement ? tabPanelDecorator.baseElement.selected ? Style.baseColor: "transparent": "transparent";
    }

    Rectangle{
        id: selection;

        anchors.bottom: parent.bottom;
        anchors.left: parent.left;
        anchors.right: parent.right;

        height: 2;

        color: Style.tabSelectedColor;
        visible: tabPanelDecorator.baseElement ? tabPanelDecorator.baseElement.selected: false;
    }

    Item {
        id: imagetabDelegate;

        height: parent.height;
        width: visible ? height : 1;

        visible: tabPanelDecorator.baseElement ? tabPanelDecorator.baseElement.firstElement: false;

        Image {
            id: firsElementImage;

            anchors.centerIn: parent;

            width: parent.width * 0.6;
            height: parent.height * 0.6;

            sourceSize.width: width;
            sourceSize.height: height;

            fillMode: Image.PreserveAspectFit;
        }
    }

    Item {
        id: texttabDelegate;

        anchors.top: parent.top;
        anchors.bottom: parent.bottom;
        anchors.left: imagetabDelegate.right;
        anchors.leftMargin: tabPanelDecorator.baseElement ? tabPanelDecorator.baseElement.firstElement ? 0 :10 : 0;

        width: text.width;

        Text {
            id: text;

            anchors.centerIn: parent;

            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
            text: tabPanelDecorator.baseElement ? tabPanelDecorator.baseElement.text : "";
        }
    }

    Item {
        id: closeButton;
        anchors.right: parent.right;
        anchors.rightMargin: 8;
        anchors.verticalCenter: parent.verticalCenter;

        height: parent.height;
        width: closeImage.width;

        visible: !tabPanelDecorator.baseElement.firstElement && tabPanelDecorator.baseElement.isCloseEnable;

        Image {
            id: closeImage;

            anchors.centerIn: parent;

            width: 13;
            height: width;

            sourceSize.width: width;
            sourceSize.height: height;

            fillMode: Image.PreserveAspectFit;
            source: "../../../" + Style.getIconPath("Icons/Close", Icon.State.On, Icon.Mode.Normal);
        }

        MouseArea{
            id: closeMA;

            anchors.fill: parent;

            enabled: closeButton.visible;
            hoverEnabled: enabled;
            cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor;

            onClicked: {
                tabPanelDecorator.baseElement.closeSignal();
            }
        }
    }
}

