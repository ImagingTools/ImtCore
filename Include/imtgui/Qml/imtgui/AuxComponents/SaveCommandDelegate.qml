import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

Item {
    id: container;

    property string originalModelJson;
    property TreeItemModel objectModel;

    Component.onDestruction: {
        objectModel.modelChanged.disconnect(compare);
    }

    onObjectModelChanged: {
        let json = JSON.stringify(objectModel);

        originalModelJson = json;

        objectModel.modelChanged.connect(compare);
    }

    function sortObj(unordered){
       return Object.keys(unordered).sort().reduce(
          (obj, key) => {
            obj[key] = unordered[key];
            return obj;
          },
          {}
        );
    }

    function compare(){
        console.log("SaveCommand compare");
        let modifiedModelJson = JSON.stringify(objectModel);

        let unorderedObj1 = JSON.parse(originalModelJson);
        let unorderedObj2 = JSON.parse(modifiedModelJson);

        console.log("unorderedObj1", JSON.stringify(unorderedObj1));
        console.log("unorderedObj2", JSON.stringify(unorderedObj2));

        let orderedObj1 = sortByKey(unorderedObj1);
        let orderedObj2 = sortByKey(unorderedObj2);

        console.log("unorderedObj1", JSON.stringify(orderedObj1));
        console.log("unorderedObj2", JSON.stringify(orderedObj2));

        if (JSON.stringify(orderedObj1) == JSON.stringify(orderedObj2)){
            commandsProvider.changeCommandMode("Save", "Disabled");
        }
        else{
            commandsProvider.changeCommandMode("Save", "Normal");
        }
    }

    function sortByKey(obj){
        return Object.keys(obj)
        .sort()
        .reduce((accumulator, key) => {
          if (typeof obj[key] === "object") {
            // recurse nested properties that are also objects
            if (obj[key] == null) {
              accumulator[key] = null;
            } else if (isArray(obj[key])) {
              accumulator[key] = obj[key].map((item) => {
                if (typeof item === "object") {
                  return sortByKey(item);
                } else {
                  return item;
                }
              });
            } else {
              accumulator[key] = sortByKey(obj[key]);
            }
          } else {
            accumulator[key] = obj[key];
          }
          return accumulator;
        }, {});
    }

//    function jsonCompare(json1, json2){

//        let obj1 = JSON.parse(json1)
//        let obj2 = JSON.parse(json2)

//        let keys1 = Object.keys(obj1);
//        let keys2 = Object.keys(obj2);

//        for (let key in keys1){

//            if (!obj2.hasOwnProperty(key)){
//                return false;
//            }

//            let value1 = obj1[key]
//            let value2 = obj2[key]
//        }

//        return true;
//    }
}

