import QtQuick 2.12
import Acf 1.0

Rectangle {
    id: filterContainer;

    height: 30;

    color: Style.baseColor;

    property alias loaderDecorator: loaderDecoratorObj;
    property string decoratorSource;

    property alias prefixLoaderComp: prefixLoader.sourceComponent;

    signal textFilterChanged(int index, string text);
    signal closed();

    signal decoratorLoaded();

    onDecoratorSourceChanged: {
        loaderDecoratorObj.source = filterContainer.decoratorSource;
    }

    onVisibleChanged: {
        filterContainer.focus = filterContainer.visible;
        if (filterContainer.visible){
            loaderDecoratorObj.item.focus = true;
        }
    }

    Keys.onPressed: {
        if (event.key === Qt.Key_Escape){
            filterContainer.parent.visible = false;
        }
    }

    Loader {
        id: prefixLoader;

        anchors.right: loaderDecoratorObj.left;
        anchors.rightMargin: Style.margin;
        anchors.verticalCenter: parent.verticalCenter;
    }

    Loader {
        id: loaderDecoratorObj;

        anchors.right: parent.right;
        anchors.rightMargin: Style.margin;
        anchors.verticalCenter: parent.verticalCenter;

        height: filterContainer.height;

        function textChanged(index, text){
            filterContainer.textFilterChanged(index, text);
        }

        function onClosed(){
            filterContainer.closed();
        }

        onLoaded: {
            if(loaderDecoratorObj.item.rootLoader !==undefined){
                loaderDecoratorObj.item.rootLoader = loaderDecoratorObj;
            }

            filterContainer.decoratorLoaded();
        }
    }

    function setTextInputWidth(width){
        if (loaderDecoratorObj.item && loaderDecoratorObj.item.textInputWidth !==undefined){
            loaderDecoratorObj.item.textInputWidth = width;
        }
    }
}
