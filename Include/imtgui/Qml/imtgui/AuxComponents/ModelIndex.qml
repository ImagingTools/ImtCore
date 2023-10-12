import QtQuick 2.12

QtObject {
    id: root;

    property var itemData;

    property var delegateItem: null; //Reference on the delegate from model.

    property ModelIndex parentIndex: null; //Reference on the parent index model.

    property var childModel: []; //Reference on the all children index models.

    property var treeModel: null; //Reference on the tree item model

    property int index: -1;

    property int depth: 0;

    function getData(value){
        return root.itemData[value];
    }

    function getDepth(value){
        return root.depth;
    }

    function setData(key, value){
        root.itemData[key] = value;
    }

    function getModel(){
        return treeModel;
    }

    function getParentModel(){
        if (root.parentIndex != null){
            return root.parentIndex.getData("ChildModel");
        }

         return undefined;
    }

    function getIndex(){
        return root.index;
    }

    function addChildIndex(childIndex){
        childIndex.parentIndex = root;
        root.childModel.push(childIndex);
    }

    function equal(otherIndex){
        let selfIndexes = root.getIndexes();
        let otherIndexes = otherIndex.getIndexes();

        return selfIndexes.toString() === otherIndexes.toString();
    }

    function equalByArrayIndexes(indexes){
        console.log("equalByArrayIndexes");

        let selfIndexes = root.getIndexes();

        console.log("selfIndexes", selfIndexes.toString());
        console.log("otherIndexes", indexes.toString());

        console.log("result", selfIndexes.toString() === indexes.toString());

        return selfIndexes.toString() === indexes.toString();
    }

    function _equalRecursive(modelIndex1, modelIndex2){
        let result = true;
        if (modelIndex1.index == modelIndex2.index){

            if (!modelIndex1.parentIndex && !modelIndex2.parentIndex){
                return true;
            }
            else if (modelIndex1.parentIndex && modelIndex2.parentIndex){
                result &= _equalRecursive(modelIndex1.parentIndex, modelIndex2.parentIndex);
            }
            else{
                return false;
            }
        }
        else{
            return false;
        }

        return result;
    }

    /**
        Get full path to the item from model index.
    */
    function getIndexes(){
        let result = []
        root._recursiveIndexes(root, result);

        return result.reverse();
    }

    /**
        private function
    */
    function _recursiveIndexes(modelIndex, retval){
        if (modelIndex){
            retval.push(modelIndex.index)
            root._recursiveIndexes(modelIndex.parentIndex, retval);
        }
    }
}
