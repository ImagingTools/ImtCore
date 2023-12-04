import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

BasicTableView {
    id: treeViewRoot;

    property int shiftLevel: 15;

    property bool tristate: false;

    rowDelegate: Component {
        TreeViewItemDelegateBase {
            id: treeViewDeleg;

            width: root.width;

            root: treeViewRoot;
        }
    }

    function addRow(row){
        treeViewRoot.insertRow([rowModel.count], row);
    }

    Component {
        id: modelComponent;
        ListModel {}
    }

    function setRow(indexes, row){
        if (!indexes || indexes.length === 0){
            return;
        }

        let localModel = rowModel;
        for (let i = 0; i < indexes.length - 1; i++){
            let index = indexes[i];
            if (localModel.count <= index){
                return;
            }
            localModel = localModel.get(index).ChildModel;
        }

        let lastIndex = indexes[indexes.length - 1];
        localModel.set(lastIndex, row);
    }

    function getRow(indexes){
        if (!indexes || indexes.length === 0){
            console.error("BasicTreeView::getRow() - invalid indexes", indexes)
            return;
        }

        let localModel = rowModel;
        for (let i = 0; i < indexes.length - 1; i++){
            let index = indexes[i];
            if (localModel.count <= index){
                console.error("BasicTreeView::insertRow() - invalid index ", index, "from the indexes", indexes);
                return;
            }
            localModel = localModel.get(index).ChildModel;
        }

        let lastIndex = indexes[indexes.length - 1];

        return localModel.get(lastIndex);
    }

    function insertRow(indexes, row, parent){
        if (!parent){
            parent = null;
        }

        if (!indexes || indexes.length == 0){
            console.error("BasicTreeView::insertRow() - invalid indexes", indexes)
            return;
        }

        let hasKey = "ChildModel" in row;
        if (!hasKey){
            row["ChildModel"] = modelComponent.createObject(treeViewRoot.rowModel);
        }

        hasKey = "CheckState" in row;
        if (!hasKey){
            row["CheckState"] = Qt.Unchecked;
        }

        hasKey = "Visible" in row;
        if (!hasKey){
            row["Visible"] = true;
        }

        hasKey = "Selected" in row;
        if (!hasKey){
            row["Selected"] = false;
        }

        hasKey = "Active" in row;
        if (!hasKey){
            row["Active"] = true;
        }

        hasKey = "CheckBoxVisible" in row;
        if (!hasKey){
            row["CheckBoxVisible"] = true;
        }

        hasKey = "IsOpen" in row;
        if (!hasKey){
            row["IsOpen"] = true;
        }

        let localModel = rowModel;

        for (let i = 0; i < indexes.length - 1; i++){
            let index = indexes[i];
            if (localModel.count <= index){
                console.error("BasicTreeView::insertRow() - invalid index ", index, "from the indexes", indexes);
                return;
            }

            localModel = localModel.get(index).ChildModel;
        }

        row["Level"] = indexes.length - 1;

        let lastIndex = indexes[indexes.length - 1];
        localModel.insert(lastIndex, row);

        treeViewRoot.rowAdded();
    }

    function removeRow(indexes){
        let localModel = rowModel;

        for (let i = 0; i < indexes.length - 1; i++){
            let index = indexes[i];
            if (localModel.count <= index){
                console.error("BasicTreeView::insertRow() - invalid index ", index, "from the indexes", indexes);
                return;
            }

            localModel = localModel.get(index).ChildModel;
        }

        let lastIndex = indexes[indexes.length - 1];

        localModel.remove(lastIndex);

        treeViewRoot.rowRemoved();
    }

    function __checkState(delegates, state){
        let result = true;

        for (let i = 0; i < delegates.length; i++){
            let childrenDelegate = delegates[i];
            if (childrenDelegate.checkState !== state){
                result = false;
                break;
            }
        }

        return result;
    }
}
