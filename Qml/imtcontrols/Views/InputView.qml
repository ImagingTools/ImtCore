import QtQuick 2.12
import Acf 1.0;
import imtcontrols 1.0

Item{
    id: inputView;

    property TreeItemModel model: TreeItemModel{};
    property TreeItemModel dataModel: TreeItemModel{};
    property int spacing: 20;
    property alias contentHeight: list.contentHeight;
    property bool fitToWidth: false;

    onModelChanged: {
        console.log("INPUT_MODEL:: ", model.toJSON())
    }

    ListView{
        id: list;

        anchors.fill: parent;

        clip: true;
        spacing: inputView.spacing;
        boundsBehavior: Flickable.StopAtBounds;
        model: inputView.model;
        delegate:
            Loader{
            id: loader;

            source: model.Source;
            //sourceComponent: model.index % 2 ? green : orange;
            onLoaded: {
                if(inputView.fitToWidth){
                    item.width = inputView.width;
                }
                if(item.dataModel !==undefined){
                    item.dataModel = inputView.dataModel;
                }

                let keys = [];
                keys = inputView.model.GetKeys(model.index);
                for(let i = 0; i < keys.length; i++){
                    if(item[keys[i]] !== undefined){
                        item[keys[i]] = inputView.model.GetData(keys[i], model.index);
                    }
                }

            }
        }
    }//list


}

