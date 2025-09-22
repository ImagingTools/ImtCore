import QtQuick

Item {
    id: root

    property var paths: []           // массив статических путей
    property string parentSegment: "" // сегмент родителя, если есть
    property bool active: false

    // теперь сигнал передаёт и остаток пути
    signal activated(var params, var restPath, string matchedPath)

    Component.onCompleted: {
        NavigationController.navigatePath.connect(processPath)
    }

    Component.onDestruction: {
        NavigationController.navigatePath.disconnect(processPath)
    }

    function processPath(path, params, activeSegments) {
        if (path.length === 0)
            return
    
        if (parentSegment !== "" && activeSegments.indexOf(parentSegment) === -1)
            return
    
        for (let i = 0; i < paths.length; i++) {
            let pattern = paths[i]
            if (pattern === path[0]) {
                root.active = true
                let rest = path.slice(1)
                root.activated(params, rest, pattern)
    
                let newActiveSegments = activeSegments.slice()
                newActiveSegments.push(path[0])
    
                if (rest.length > 0) {
                    NavigationController.navigatePath(rest, params, newActiveSegments)
                }
                break
            }
        }
    }
}

