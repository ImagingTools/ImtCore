import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtauthgui 1.0

Rectangle {
    id: filterContainer;

    color: Style.backgroundColor;

    property alias loaderDecorator: loaderDecoratorObj;
    property string decoratorSource;

    signal textFilterChanged(int index, string text);
    signal closed();


    onDecoratorSourceChanged: {
        console.log("loaderDecorator.source", filterContainer.decoratorSource);
        loaderDecoratorObj.source = filterContainer.decoratorSource;
    }

    Loader {
        id: loaderDecoratorObj;

        anchors.fill: parent;

        function textChanged(index, text){
            filterContainer.textFilterChanged(index, text);
        }

        function onClosed(){
            filterContainer.closed();
        }

        onItemChanged: {
            if (loaderDecoratorObj.item){
                loaderDecoratorObj.item.width = filterContainer.width;
                filterContainer.height = loaderDecoratorObj.item.height;
                if(loaderDecoratorObj.item.rootLoader !==undefined){
                    loaderDecoratorObj.item.rootLoader = loaderDecoratorObj;
                }
            }
        }
    }
}
