import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

ControlBase {
    id: tabDelegate;

    // height: 40;

//    width: loaderDecorator.width + closeButton.width;

    property bool selected: false;
    property bool firstElement: false;
    property bool lastElement: false;
    property bool autoWidth: true;
    property bool isCloseEnable: true;

    property string text;
    property string firstElementImageSource:  "../../../" + "Icons/" + Style.theme + "/" + "Workflow" + "_On_Normal.svg";

    // property string decoratorSource;
    decoratorComponent: Style.tabPanelDecorator

    signal clicked;
    signal closeSignal;

    // onDecoratorSourceChanged: {
    //     loaderDecorator.source = tabDelegate.decoratorSource;
    // }

    // Loader {
    //     id: loaderDecorator;
    //     height: tabDelegate.height;

    //     onItemChanged: {
    //         if (loaderDecorator.item){
    //             tabDelegate.width = loaderDecorator.item.width;
    //             if(loaderDecorator.item.rootItem !== undefined){
    //                 loaderDecorator.item.rootItem = tabDelegate;
    //             }
    //         }
    //     }

    //     onWidthChanged: {
    //         if (loaderDecorator.item){
    //             tabDelegate.width = loaderDecorator.item.width;
    //         }
    //     }

    //     onLoaded: {
    //         console.log("tabDelegate Loader onLoaded", tabDelegate.height);
    //     }
    // }

    MouseArea{
        id: ma;

        anchors.fill: tabDelegate;

        onClicked: {
            console.log("tabDelegate clicked ");
            tabDelegate.clicked();
        }
    }


}
