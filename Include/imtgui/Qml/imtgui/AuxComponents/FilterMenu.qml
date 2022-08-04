import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtauthgui 1.0

Rectangle {
    id: filterContainer;

    color: Style.backgroundColor;

    signal textFilterChanged(int index, string text);
    signal closed();

    property string decoratorSource;

    onDecoratorSourceChanged: {
        console.log("loaderDecorator.source", filterContainer.decoratorSource);
        loaderDecorator.source = filterContainer.decoratorSource;
    }

    onVisibleChanged: {
        console.log("filterContainer onVisibleChanged", filterContainer.width, filterContainer.height);
    }

    Loader {
        id: loaderDecorator;

        anchors.fill: parent;

        function textChanged(index, text){
            filterContainer.textFilterChanged(index, text);
        }

        function onClosed(){
            filterContainer.closed();
        }

        onItemChanged: {
            if (loaderDecorator.item){
                loaderDecorator.item.width = filterContainer.width;
                filterContainer.height = loaderDecorator.item.height;
            }
        }
    }
}
