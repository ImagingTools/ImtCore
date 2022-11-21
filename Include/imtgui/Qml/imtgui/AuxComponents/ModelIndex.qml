import QtQuick 2.12
import Acf 1.0

Item {
    id: root;

    property var itemData;

    property ModelIndex parentIndex; //Reference on the parent index model.

    //property var children: {[]}; //Reference on the all children index models.

    property int index: -1;
    property int level: 1;

    onParentIndexChanged: {
        if (parentIndex){
            level = parentIndex.level + 1;
            parentIndex.parentIndexChanged.connect(parentIndexChanged);
        }
    }

    Component.onDestruction: {
        if (parentIndex){
            parentIndex.parentIndexChanged.disconnect(parentIndexChanged);
        }
    }

    function equal(otherIndex){
        if (!otherIndex || level != otherIndex.level){
            return false;
        }

        return _equalRecursive(root, otherIndex);
    }

    function _equalRecursive(modelIndex1, modelIndex2){
        console.log("_equalRecursive", modelIndex1, modelIndex2);
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
        _recursiveIndexes(root, result);

        return result.reverse();
    }

    /**
        private function
    */
    function _recursiveIndexes(modelIndex, retval){
        if (modelIndex){
            retval.push(modelIndex.index)
            _recursiveIndexes(modelIndex.parentIndex, retval);
        }
    }
}
