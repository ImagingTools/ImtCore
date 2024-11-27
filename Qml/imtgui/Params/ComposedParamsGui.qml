import QtQuick 2.0
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

Column {
    id: root;
    spacing: Style.size_largeMargin;
    property int contentWidth: 700;

    property alias model: bodyPanelRepeater.model;

    Repeater {
        id: bodyPanelRepeater;
        delegate: Loader {
            id: loader;
            property int rootWidth: root.width;
            Component.onCompleted: {
                if (model.Source && model.Source !== ""){
                    loader.source = model.Source;
                }
            }

            onRootWidthChanged: {
                checkWidth();
            }

            onLoaded: {
                checkWidth();

                if (item.modelData != undefined && item.modelData != null){
                    item.modelData = model;
                }
            }

            function checkWidth(){
                if (item){
                    if (item.width >= root.width){
                        item.width = root.width;
                    }
                    else{
                        item.width = root.contentWidth;
                    }
                }
            }
        }
    }
}//Column

