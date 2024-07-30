pragma Singleton

import QtQuick 2.12

QtObject {
    id: root;

    property var stack: [];
    property int currentIndex: -1;
    property int maxSize: 99;

    function goTo(path){}

    function next(){
        if (hasNext()){
            currentIndex++;

            let elem = stack[currentIndex]["Element"]
            stack[currentIndex]["Callback"](elem);
        }
    }

    function hasNext(){
        return currentIndex < stack.length - 1;
    }

    function prev(){
        if (hasPrev()){
            currentIndex--;
            let elem = stack[currentIndex]["Element"]

            stack[currentIndex]["Callback"](elem);
        }
    }

    function hasPrev(){
        return currentIndex > 0;
    }

    function push(elementRef, callback){
        if (stack.length >= maxSize){
            stack.shift();

            if (currentIndex > 0){
                currentIndex--;
            }
        }

        let obj = {}
        obj["Element"] = elementRef;
        obj["Callback"] = callback;

        stack.push(obj);
        currentIndex = stack.length - 1;
    }

    function currentIndexCorrection(){

    }

    function pop(){
        if (stack.length == 0){
            return;
        }

        if (currentIndex == stack.length - 1){
            currentIndex--;
        }

        stack.pop();
    }

    function clear(){
        root.currentIndex = -1;
        root.stack = []
    }
}
