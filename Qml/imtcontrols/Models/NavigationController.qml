pragma Singleton

import QtQuick 2.12
import imtcontrols 1.0

QtObject {
    id: root;

    property var stack: []
    property int currentIndex: -1
    property int maxSize: 10

    signal navigatePath(var path, var params, var activeSegments)

    function navigate(path, params) {
        let segments = path.split("/")
        navigatePath(segments, params, [])
    }

    function next(){
        if (hasNext()){
            currentIndex++;

            let elem = stack[currentIndex]
            navigate(elem)
        }
    }

    function hasNext(){
        return currentIndex < stack.length - 1;
    }

    function prev(){
        if (hasPrev()){
            currentIndex--;
            let elem = stack[currentIndex]

            navigate(elem)
        }
    }

    function hasPrev(){
        return currentIndex > 0;
    }

    function push(path){
        if (stack.length >= maxSize){
            stack.shift();

            if (currentIndex > 0){
                currentIndex--;
            }
        }

        stack.push(path);
        currentIndex = stack.length - 1;
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
