import QtQuick 2.12

Item{
    id: container

    property int currentIndex: -1
    property alias count: container.pagesModel.count
    property ListModel pagesModel: ListModel{}
    property bool adaptSizeToCurrentPage: false

    signal pageAdded(int index, var item)
    signal pageRemoved(int index, var item)
    signal currentPageChanged(var item)

    onCurrentPageChanged: {
        itemConnections.target = null

        if (item && adaptSizeToCurrentPage){
            console.log("onCurrentPageChanged", item.height)
            width = item.width
            height = item.height
            itemConnections.target = item
        }
    }

    Connections {
        id: itemConnections
        function onHeightChanged(){
            console.log("itemConnections onHeightChanged", target.height)
            if (container.adaptSizeToCurrentPage){
                container.height = target.height
            }
        }

        function onWidthChanged(){
            if (container.adaptSizeToCurrentPage){
                container.width = target.width
            }
        }
    }

    function addPage(comp){
        if (!comp)
            return

        pagesModel.append({ "component": comp })
        if (currentIndex === -1)
            setCurrentIndex(0)
    }

    function removePage(index){
        if (index < 0 || index >= pagesModel.count)
            return

        pagesModel.remove(index)
        if (index === currentIndex){
            if (pagesModel.count > 0)
                setCurrentIndex(Math.max(0, index - 1))
            else
                setCurrentIndex(-1)
        }
    }

    function clear(){
        pagesModel.clear()
        setCurrentIndex(-1)
    }

    function setCurrentIndex(index){
        if (index < 0 || index >= pagesModel.count)
            return

        if (currentIndex !== index){
            currentIndex = index
            let loader = repeater.itemAt(index)
            if (loader)
                currentPageChanged(loader.item)
        }
    }

    function currentPage(){
        let loader = repeater.itemAt(currentIndex)
        return loader ? loader.item : null
    }

    function next(){
        if (currentIndex < pagesModel.count - 1)
            setCurrentIndex(currentIndex + 1)
    }

    function previous(){
        if (currentIndex > 0)
            setCurrentIndex(currentIndex - 1)
    }

    function getComponent(index){
        if (index < 0 || index >= pagesModel.count)
            return null
    
        return pagesModel.get(index).component
    }

    Repeater{
        id: repeater
        anchors.fill: parent
        model: container.pagesModel

        delegate: Loader {
            id: loader
            anchors.fill: container.adaptSizeToCurrentPage ? undefined : parent

            sourceComponent: model.component
            visible: model.index === container.currentIndex

            onLoaded:{
                container.pageAdded(model.index, item)
            }

            onStatusChanged:{
                if (status === Loader.Error)
                    console.error("Failed to load stacked page")
            }

            Component.onDestruction:{
                container.pageRemoved(model.index, item)
            }
        }
    }
}
