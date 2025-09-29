import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

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

        let index = root.getStateIndex(statusId);
        if (index >= 0){
            root.availableModel.clear();

            let data = root.states[statusId];

            for (let i = 0; i < data.length; i++){
                let id = data[i];
                let name = root.getStateName(id);

                let j = root.availableModel.insertNewItem();

                root.availableModel.setData("id", id, j);
                root.availableModel.setData("name", name, j);
            }

            return root.availableModel;
        }

        return null;
    }

    function getStateIndex(statusId){
        if (!root.stateModel){
            return -1;
        }

        for (let i = 0; i < root.stateModel.getItemsCount(); i++){
            let id = root.stateModel.getData("id", i);
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

        for (let i = 0; i < root.stateModel.getItemsCount(); i++){
            let id = root.stateModel.getData("id", i);
            if (id === statusId){
                let name = root.stateModel.getData("name", i);
                return name;
            }
        }

        return "";
    }
}
