import QtQuick 2.12
import Acf 1.0

Item {
    id: root;

    property var itemData;

    property var delegateItem: null; //Reference on the delegate from model.

    property ModelIndex parentIndex: null; //Reference on the parent index model.

    property ModelIndex nextIndex: null; //Reference to the next index.
    property ModelIndex prevIndex: null; //Reference to the prev index.

    property var childModel: []; //Reference on the all children index models.

    property int index: -1;

//    function equal(otherIndex){

//        console.log("equal")

//        return root._equalRecursive(root, otherIndex);
//    }

    function equal(otherIndex){
        console.log("equal", otherIndex);
        let selfIndexes = root.getIndexes();
        let otherIndexes = otherIndex.getIndexes();

        console.log("selfIndexes", selfIndexes.toString());
        console.log("otherIndexes", otherIndexes.toString());

        return selfIndexes.toString() === otherIndexes.toString();
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
        console.log("getIndexes")
        let result = []
        root._recursiveIndexes(root, result);

        return result.reverse();
    }

    /**
        private function
    */
    function _recursiveIndexes(modelIndex, retval){
        console.log("_recursiveIndexes")
        if (modelIndex){
            retval.push(modelIndex.index)
            root._recursiveIndexes(modelIndex.parentIndex, retval);
        }
    }
}
