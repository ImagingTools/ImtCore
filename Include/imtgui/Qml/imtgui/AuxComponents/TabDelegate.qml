import QtQuick 2.12
import Acf 1.0

Item{
    id: tabDelegate;

    height: 40;

    property bool selected: false;
    property bool firstElement: false;
    property bool lastElement: false;
    property bool autoWidth: true;
    property bool isCloseEnable: true;

    property string text;
    property string firstElementImageSource:  "../../../" + "Icons/" + Style.theme + "/" + "Workflow" + "_On_Normal.svg";

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

            console.log("loaderDecorator.item.width", loaderDecorator.item.width);

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

        anchors.right: loaderDecorator.right;
        anchors.rightMargin: 5;
        anchors.verticalCenter: parent.verticalCenter;

        height: parent.height * 0.3;
        width: height;

        visible: !parent.firstElement && tabDelegate.isCloseEnable;

        Image {
            id: closeImage;

            anchors.centerIn: parent;

            width: parent.width;
            height: parent.height;

            sourceSize.width: width;
            sourceSize.height: height;

            fillMode: Image.PreserveAspectFit;
            source: "../../../" + "Icons/" + Style.theme + "/" + "Close" + "_On_Normal.svg";
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
