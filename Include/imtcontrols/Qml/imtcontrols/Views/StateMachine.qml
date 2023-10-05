import QtQuick 2.12
import Acf 1.0

QtObject {
    id: root;

    property TreeItemModel stateModel: null;
    property TreeItemModel availableModel: TreeItemModel {}

    property var states: ({})

    function registerModel(model){
        root.stateModel = model;
    }

    // states - states to which you can switch from state
    function addState(state, states){
        root.states[state] = states;
    }

    function getAvailableModel(statusId){
        if (!root.stateModel){
            return null;
        }

        console.log("getAvailableModel", statusId);
        let index = root.getStateIndex(statusId);
        if (index >= 0){
            root.availableModel.Clear();

            let data = root.states[statusId];

            for (let i = 0; i < data.length; i++){
                let id = data[i];
                let name = root.getStateName(id);

                let j = root.availableModel.InsertNewItem();

                root.availableModel.SetData("Id", id, j);
                root.availableModel.SetData("Name", name, j);
            }

            return root.availableModel;
        }

        return null;
    }

    function getStateIndex(statusId){
        if (!root.stateModel){
            return -1;
        }

        for (let i = 0; i < root.stateModel.GetItemsCount(); i++){
            let id = root.stateModel.GetData("Id", i);
            if (id === statusId){
                return i;
            }
        }

        return -1;
    }

    function getStateName(statusId){
        if (!root.stateModel){
            return "";
        }

        for (let i = 0; i < root.stateModel.GetItemsCount(); i++){
            let id = root.stateModel.GetData("Id", i);
            if (id === statusId){
                let name = root.stateModel.GetData("Name", i);
                return name;
            }
        }

        return "";
    }
}
