pragma Singleton

import QtQuick 2.12
import Acf 1.0

QtObject {
    id: root;

    property var stack: [];
    property int currentIndex: -1;
    property int maxSize: 100;

    function goTo(path){}

    function next(){
        if (currentIndex < stack.length - 1){
            currentIndex++;

            let elem = stack[currentIndex]["Element"]
            stack[currentIndex]["Callback"](elem);
        }
    }

    function prev(){
        if (currentIndex > 0){
            currentIndex--;
            let elem = stack[currentIndex]["Element"]

            stack[currentIndex]["Callback"](elem);
        }
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
        currentIndex++;
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
