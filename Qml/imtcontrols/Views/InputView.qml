import QtQuick 2.12
import Acf 1.0;
import imtcontrols 1.0

Item{
    id: inputView;

    property TreeItemModel model: TreeItemModel{};
    property TreeItemModel dataModel: TreeItemModel{};
    property int spacing: 20;
    property alias contentHeight: list.contentHeight;
    property alias contentWidth: list.contentWidth;
    property bool fitToWidth: false;
    property int orientation: Qt.Vertical;

    onModelChanged: {
        //console.log("INPUT_MODEL:: ", model.ToJson())
    }

    ListView{
        id: list;

        anchors.fill: parent;

        clip: true;
        spacing: inputView.spacing;
        boundsBehavior: Flickable.StopAtBounds;
        orientation: inputView.orientation == Qt.Vertical ? ListView.Vertical : ListView.Horizontal
        cacheBuffer: 5000;
        model: inputView.model;
        delegate:
            Loader{
            id: loader;

            source: model.Source;
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
                    //console.log("KEY::", keys[i], item[keys[i]])
                    if(item[keys[i]] !==undefined){
                        if(inputView.model.IsTreeModel(keys[i], model.index)){
                            console.log("IS_MODEL:: ", keys[i])
                            item[keys[i]] = inputView.model.GetTreeItemModel(keys[i], model.index);
                        }
                        else {
                            item[keys[i]] = inputView.model.GetData(keys[i], model.index);
                        }
                    }
                }

            }
        }
    }//list

}

