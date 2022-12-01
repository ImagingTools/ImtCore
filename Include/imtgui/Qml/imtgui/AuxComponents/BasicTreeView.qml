import QtQuick 2.12
import Acf 1.0

BasicTableView {
    id: treeViewRoot;

    property int shiftLevel: 15;

    property bool tristate: false;

    rowDelegate: TreeViewItemDelegateBase {
        width: root.width;

        onParentCheckStateChanged: {
            console.log("onParentCheckedChanged")

            model.CheckState = data.CheckState;

            for (let i = 0; i < childrenDelegates.length; i++){
                childrenDelegates[i].parentCheckStateChanged(model);
            }
        }

        onChildrenCheckStateChanged: {
            console.log("onChildrenCheckedChanged")

            let isAllChecked = root.__checkState(childrenDelegates, Qt.Checked);
            let isAllUnchecked = root.__checkState(childrenDelegates, Qt.Unchecked);

            if (isAllChecked){
                if (model.CheckState != Qt.Checked){
                    model.CheckState = Qt.Checked
                }
            }
            else if (isAllUnchecked){
                if (model.CheckState != Qt.Unchecked){
                    model.CheckState = Qt.Unchecked
                }
            }
            else if (!isAllChecked && !isAllUnchecked){
                if (model.CheckState != Qt.PartiallyChecked){
                    model.CheckState = Qt.PartiallyChecked
                }
            }

            if (parentDelegate){
                parentDelegate.childrenCheckStateChanged(model)
            }
        }
    }

    function addRow(row){
        insertRow([rowModel.count], row);
    }

//    function addRow(row){
//        let count = rowModel.GetItemsCount();
//        insertRow([count], row);
//    }

    function insertRow(indexes, row){
        console.log("insertRow")
        if (!indexes || indexes.length == 0){
            console.error("BasicTreeView::insertRow() - invalid indexes", indexes)
            return;
        }

        let hasKey = "ChildModel" in row;
        if (!hasKey){
            row["ChildModel"] = [];
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

        let model = rowModel;

        for (let i = 0; i < indexes.length - 1; i++){
            let index = indexes[i]
            if (model.count <= index){
                console.error("BasicTreeView::insertRow() - invalid index ", index, "from the indexes", indexes)
                return;
            }

            model = model.get(index).ChildModel
        }

        row["Level"] = indexes.length - 1;

        let lastIndex = indexes[indexes.length - 1];

        model.insert(lastIndex, row)

        rowAdded();
    }

//    function insertRow(indexes, row){
//        console.log("insertRow")
//        if (!indexes || indexes.length == 0){
//            console.error("BasicTreeView::insertRow() - invalid indexes", indexes)
//            return;
//        }

//        let hasKey = "CheckState" in row;
//        if (!hasKey){
//            row["CheckState"] = Qt.Unchecked;
//        }

//        hasKey = "Visible" in row;
//        if (!hasKey){
//            row["Visible"] = true;
//        }

//        hasKey = "Selected" in row;
//        if (!hasKey){
//            row["Selected"] = false;
//        }

//        hasKey = "Active" in row;
//        if (!hasKey){
//            row["Active"] = true;
//        }

//        hasKey = "CheckBoxVisible" in row;
//        if (!hasKey){
//            row["CheckBoxVisible"] = true;
//        }

//        let model = rowModel;

//        for (let i = 0; i < indexes.length - 1; i++){
//            let index = indexes[i]
//            let count = model.GetItemsCount();
//            if (count <= index){
//                console.error("BasicTreeView::insertRow() - invalid index ", index, "from the indexes", indexes)
//                return;
//            }

//            let childModel = model.GetData("ChildModel", index);
//            if (!childModel){
//                childModel = model.AddTreeModel("ChildModel", index)
//            }

//            model = childModel;
//        }

//        row["Level"] = indexes.length - 1;

//        let lastIndex = indexes[indexes.length - 1];

////        let newIndex = model.InsertNewItem();

//        model.InsertNewItemWithParameters(lastIndex, row);

////        let keys = Object.keys(row)
////        for (let key of keys){
////            let value = row[key]
////            model.SetData(key, value, newIndex);
////        }

//        tableListView.model = 0
//        tableListView.model = rowModel;

//        console.log("rowModel", rowModel.toJSON())

//        rowAdded();
//    }

//    function removeRow(indexes){
//        console.log("removeRow", indexes)

//        let model = rowModel;

//        for (let i = 0; i < indexes.length - 1; i++){
//            let index = indexes[i]
//            if (model.count <= index){
//                console.error("BasicTreeView::insertRow() - invalid index ", index, "from the indexes", indexes)
//                return;
//            }

//            model = model.GetData("ChildModel", index);
//        }

//        let lastIndex = indexes[indexes.length - 1];

//        model.RemoveItem(lastIndex);

//        rowRemoved();
//    }

    function removeRow(indexes){
        console.log("removeRow", indexes)

        let model = rowModel;

        for (let i = 0; i < indexes.length - 1; i++){
            let index = indexes[i]
            if (model.count <= index){
                console.error("BasicTreeView::insertRow() - invalid index ", index, "from the indexes", indexes)
                return;
            }

            model = model.get(index).ChildModel
        }

        let lastIndex = indexes[indexes.length - 1];

        model.remove(lastIndex)

        console.log("model", model)

        rowRemoved();
    }

    function printModel(rowModel){
        for (let i = 0; i < rowModel.count; i++){
            let obj = rowModel.get(i);

            console.log("obj", JSON.stringify(obj));

            if (obj.ChildModel.length > 0){
                printModel(obj.ChildModel);
            }
        }

    }

    function __checkState(delegates, state){
        let result = true;

        for (let i = 0; i < delegates.length; i++){
            let childrenDelegate = delegates[i];
            if (childrenDelegate.itemData.CheckState != state){
                result = false;
                break;
            }
        }

        return result;
    }
}
