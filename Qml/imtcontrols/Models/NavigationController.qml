pragma Singleton
import QtQuick 2.12

QtObject {
    id: root

    property int maxSize: 10

    signal navigatePath(var path, var params, var activeSegments)
    signal currentIndexChanged(int index)

    function navigate(path, params) {
        __internal.push(path, params)
        __internal.navigateInternal()
    }

    function push(path, params) {
        __internal.push(path, params)
    }

    function next() {
        if (hasNext()) {
            __internal.currentIndex++
            __internal.navigateInternal()
        }
    }

    function hasNext() {
        return __internal.currentIndex < __internal.stack.length - 1
    }

    function prev() {
        if (hasPrev()) {
            __internal.currentIndex--
            __internal.navigateInternal()
        }
    }

    function hasPrev() {
        return __internal.currentIndex > 0
    }

    function clear() {
        __internal.clear()
    }

    // Внутренний объект
    property QtObject __internal: QtObject {
        property var stack: []          // [{ path, params }, ...]
        property int currentIndex: -1

        onCurrentIndexChanged: {
            root.currentIndexChanged(currentIndex)
        }

        function clear() {
            stack = []
            currentIndex = -1
        }

        function push(path, params) {
            if (currentIndex < stack.length - 1) {
                stack.splice(currentIndex + 1, stack.length - (currentIndex + 1))
            }

            stack.push({ path: path, params: params || {} })

            while (stack.length > root.maxSize){
                stack.shift()
                if (currentIndex > 0) currentIndex--
            }

            currentIndex = stack.length - 1
        }

        function navigateInternal() {
            if (currentIndex < 0 || currentIndex >= stack.length) {
                console.log("Unable to navigate internal. Current index is invalid")
                return
            }

            let entry = stack[currentIndex]
            let segments = entry.path.split("/")
            root.navigatePath(segments, entry.params || {}, [])
        }
    }
}
