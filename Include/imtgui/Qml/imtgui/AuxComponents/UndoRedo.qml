import QtQuick 2.0
import Acf 1.0;

Item {
    id: undoRedo;

//    property TreeItemModel storageModel;
    property int currentIndex: 0;

    TreeItemModel {
        id: storageModel;
    }

    TreeItemModel {
        id: testModel;
    }

    Component.onCompleted: {
        console.log("UndoRedo onCompleted");
        testModel.InsertNewItem();
        testModel.SetData("Id", "TestId", 0);
        testModel.SetData("Name", "TestName", 0);

        let addModel = testModel.AddTreeModel("Model", 0);
        addModel.SetData("Id", "TestId");
        addModel.SetData("Name", "TestName");

        undoRedo.setCopyModel(testModel, emptyModel);

    }

    function next(){

    }

    function prev(){

    }

    TreeItemModel {
        id: emptyModel;
    }

    function setCopyModel(fromModel, toModel){
        console.log("UndoRedo setCopyModel", fromModel.toJSON());
        for (let i = 0; i < fromModel.GetItemsCount(); i++){
            let keys = fromModel.GetKeys(i);
            for (let j = 0; j < keys.length; j++){
                console.log(i, keys[j], fromModel.GetData(keys[j], i))
                let data = fromModel.GetData(keys[j], i);
                if (typeof data === 'object'){
                    let childModel = toModel.AddTreeModel(keys[j], i)
                    console.log("childModel", data.toJSON());
                    undoRedo.setCopyModel(data, childModel);
                }
                else{
                    toModel.SetData(keys[j], data, i);
                }
            }
            toModel.InsertNewItem();
        }
    }

    function getCopyModel(fromModel, toModel){
        console.log("UndoRedo getCopyModel", fromModel.toJSON());


    }
}
