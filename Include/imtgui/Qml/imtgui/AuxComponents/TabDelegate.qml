import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

Item{
    id: tabDelegate;

    height: 40;
//    width: loaderDecorator.width + closeButton.width;

    property bool selected: false;
    property bool firstElement: false;
    property bool lastElement: false;
    property bool autoWidth: true;
    property bool isCloseEnable: true;

    property string text;
    property string firstElementImageSource:  "";

    property string decoratorSource;

    signal clicked;
    signal closeSignal;

    onDecoratorSourceChanged: {
        loaderDecorator.source = tabDelegate.decoratorSource;
    }

    Loader {
        id: loaderDecorator;
        height: tabDelegate.height;

        onItemChanged: {
            tabDelegate.width = loaderDecorator.item.width;
            if(loaderDecorator.item.rootItem !== undefined){
                loaderDecorator.item.rootItem = tabDelegate;
            }
        }

        onWidthChanged: {
            if (loaderDecorator.item){
                tabDelegate.width = loaderDecorator.item.width;
            }
        }
    }

    MouseArea{
        id: ma;

        anchors.fill: tabDelegate;

        onClicked: {
            console.log("tabDelegate clicked ");
            tabDelegate.clicked();
        }
    }

    Item {
        id: closeButton;

//        anchors.right: parent.right;
        anchors.right: loaderDecorator.right;
        anchors.rightMargin: 8;
        anchors.verticalCenter: parent.verticalCenter;

        height: parent.height;
        width: closeImage.width;

        visible: !parent.firstElement && tabDelegate.isCloseEnable;

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

            enabled: tabDelegate.visible;
            hoverEnabled: enabled;
            cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor;

            onClicked: {
                tabDelegate.closeSignal();
            }
        }
    }
}
