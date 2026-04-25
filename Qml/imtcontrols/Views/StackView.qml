import QtQuick 2.12

Item{
    id: container

    property int currentIndex: -1
    property alias count: container.pagesModel.count
    property ListModel pagesModel: ListModel{}
    property bool adaptSizeToCurrentPage: false
	property int defaultCurrentIndex: 0;

	// Stack navigation – mirrors QtQuick.Controls.StackView
	readonly property int depth: pagesModel.count
	property var currentItem: null
	property bool busy: false

    signal pageAdded(int index, var item)
    signal pageRemoved(int index, var item)
    signal currentPageChanged(var item)

    onCurrentPageChanged: {
        itemConnections.target = null
        currentItem = item

        if (item && adaptSizeToCurrentPage){
            width = item.width
            height = item.height
            itemConnections.target = item
        }
    }

    Connections {
        id: itemConnections
        function onHeightChanged(){
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
			setCurrentIndex(defaultCurrentIndex)
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

    // Stack-navigation API – mirrors QtQuick.Controls.StackView

    // Push a component onto the stack: appends it and navigates to it.
    // Returns the newly created item once loaded, or null if comp is invalid.
    function push(comp) {
        if (!comp)
            return null
        let insertIndex = pagesModel.count
        pagesModel.append({ "component": comp })
        setCurrentIndex(insertIndex)
        let loader = repeater.itemAt(currentIndex)
        return loader ? loader.item : null
    }

    // Pop the current top page and navigate to the previous one.
    // If depth ≤ 1 this is a no-op. Returns the item that was on top.
    function pop() {
        if (pagesModel.count <= 1)
            return null
        let topIndex  = pagesModel.count - 1
        let loader    = repeater.itemAt(topIndex)
        let topItem   = loader ? loader.item : null
        // Navigate back first so the user sees the previous page.
        setCurrentIndex(topIndex - 1)
        pagesModel.remove(topIndex)
        return topItem
    }

    // Replace the current top page with a new component.
    function replace(comp) {
        if (!comp)
            return null
        let replaceIndex = currentIndex >= 0 ? currentIndex : 0
        pagesModel.set(replaceIndex, { "component": comp })
        // Re-trigger navigation so the new loader is picked up.
        let prevIndex = currentIndex
        currentIndex = -1
        setCurrentIndex(prevIndex)
        let loader = repeater.itemAt(currentIndex)
        return loader ? loader.item : null
    }

    // Get the item at the given stack index (0 = bottom, depth-1 = top).
    function get(index) {
        if (index < 0 || index >= pagesModel.count)
            return null
        let loader = repeater.itemAt(index)
        return loader ? loader.item : null
    }

    // Find a page item using a predicate function(item) → bool.
    // Returns the first matching item, or null.
    function find(predicate) {
        for (let i = 0; i < pagesModel.count; i++) {
            let loader = repeater.itemAt(i)
            if (loader && loader.item && predicate(loader.item))
                return loader.item
        }
        return null
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
