import QtQuick 2.12
import QtQuick.Controls 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

Popup {
    id: root

    modal: false
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
    padding: 0
    width: Style.sizeHintXS
    height: Math.min(contentColumn.implicitHeight, maxPopupHeight)

    property var dataProvider: null
    property int pageLimit: dataProvider && dataProvider.defaultLimit !== undefined ? dataProvider.defaultLimit : 50
    property int debounceInterval: 300
    property int itemHeight: Style.controlHeightM
    property int maxVisibleItems: 8
    property int maxPopupHeight: searchField.height + Math.max(1, maxVisibleItems) * itemHeight + footer.height + 2 * Style.marginM
    property bool multiSelect: false
    property string idKey: "id"
    property string textKey: "name"
    property string placeholderText: qsTr("Filter")
    property string emptyText: qsTr("No items found")
    property var preselectedItems: []
    property Component delegate: genericDelegate

    property string filter: searchField.text
    property bool loading: false
    property string errorMessage: ""
    property bool hasMore: false
    property int highlightedIndex: -1
    property int count: itemsModel.count

    property var _nextCursor: null
    property int _requestSerial: 0
    property var _selectedById: ({})
    property var _selectedItemsById: ({})

    signal itemSelected(var item)
    signal selectionChanged(var items)
    signal fetchFailed(string errorMessage)

    onOpened: {
        searchField.forceActiveFocus()
        resetAndFetch()
    }

    onClosed: {
        debounceTimer.stop()
    }

    onPreselectedItemsChanged: {
        applyPreselectedItems()
    }

    Component.onCompleted: {
        applyPreselectedItems()
    }

    background: Rectangle {
        color: Style.baseColor
        radius: Style.buttonRadius
        border.color: Style.borderColor
        border.width: 1
    }

    contentItem: Column {
        id: contentColumn

        width: root.width
        spacing: Style.marginXS
        padding: Style.marginM

        CustomTextField {
            id: searchField

            width: parent.width - 2 * parent.padding
            height: Style.controlHeightM
            placeHolderText: root.placeholderText

            onTextChanged: {
                debounceTimer.restart()
            }

            Keys.onPressed: {
                if (event.key === Qt.Key_Down){
                    root.moveHighlight(1)
                    event.accepted = true
                }
                else if (event.key === Qt.Key_Up){
                    root.moveHighlight(-1)
                    event.accepted = true
                }
                else if (event.key === Qt.Key_Return || event.key === Qt.Key_Enter){
                    root.selectHighlighted()
                    event.accepted = true
                }
            }
        }

        Item {
            width: parent.width - 2 * parent.padding
            height: Math.min(Math.max(itemsModel.count, 1), root.maxVisibleItems) * root.itemHeight

            ListView {
                id: listView

                anchors.fill: parent
                clip: true
                boundsBehavior: Flickable.StopAtBounds
                model: itemsModel
                currentIndex: root.highlightedIndex

                delegate: Item {
                    id: row

                    width: listView.width
                    height: root.itemHeight

                    property var item: model.itemData
                    property string itemId: root.itemId(item)
                    property bool selected: root.isSelected(item)
                    property bool highlighted: root.highlightedIndex === index
                    property bool checkboxMode: root.multiSelect
                    property string filterText: root.filter

                    Rectangle {
                        anchors.fill: parent
                        color: row.highlighted ? Style.selectedColor : "transparent"
                    }

                    Loader {
                        id: delegateLoader

                        anchors.fill: parent
                        sourceComponent: root.delegate

                        property var item: row.item
                        property bool selected: row.selected
                        property bool highlighted: row.highlighted
                        property bool checkboxMode: row.checkboxMode
                        property string filterText: row.filterText
                    }

                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        acceptedButtons: Qt.LeftButton
                        onEntered: root.highlightedIndex = index
                        onClicked: root.toggleItem(row.item)
                    }
                }

                onContentYChanged: {
                    if (contentY + height >= contentHeight - root.itemHeight){
                        root.fetchNextPage()
                    }
                }
            }

            Text {
                anchors.centerIn: parent
                visible: !root.loading && itemsModel.count === 0 && root.errorMessage === ""
                text: root.emptyText
                color: Style.placeHolderTextColor
                font.pixelSize: Style.fontSizeM
            }
        }

        Item {
            id: footer

            width: parent.width - 2 * parent.padding
            height: visible ? Style.controlHeightM : 0
            visible: root.loading || root.errorMessage !== ""

            Text {
                anchors.centerIn: parent
                visible: root.loading
                text: qsTr("Loading...")
                color: Style.placeHolderTextColor
                font.pixelSize: Style.fontSizeM
            }

            Text {
                anchors.centerIn: parent
                visible: !root.loading && root.errorMessage !== ""
                text: root.errorMessage
                color: Style.errorTextColor
                font.pixelSize: Style.fontSizeM
                elide: Text.ElideRight
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
            }
        }
    }

    ListModel {
        id: itemsModel
    }

    Timer {
        id: debounceTimer
        interval: root.debounceInterval
        repeat: false
        onTriggered: root.resetAndFetch()
    }

    Component {
        id: genericDelegate

        Item {
            Text {
                anchors.left: parent.left
                anchors.leftMargin: Style.marginM
                anchors.right: selectionMark.left
                anchors.verticalCenter: parent.verticalCenter
                text: item ? (item[root.textKey] || item.title || item.name || item.id || "") : ""
                color: Style.textColor
                font.pixelSize: Style.fontSizeM
                elide: Text.ElideRight
            }

            Text {
                id: selectionMark
                anchors.right: parent.right
                anchors.rightMargin: Style.marginM
                anchors.verticalCenter: parent.verticalCenter
                visible: selected
                text: checkboxMode ? "☑" : "✓"
                color: Style.iconColorOnSelected
                font.pixelSize: Style.fontSizeM
            }
        }
    }

    function openAt(item, xOffset, yOffset){
        let point = item.mapToItem(null, xOffset || 0, yOffset === undefined ? item.height : yOffset)
        root.x = point.x
        root.y = point.y
        root.open()
    }

    function itemId(item){
        if (!item){
            return ""
        }
        let value = item[root.idKey]
        if (value === undefined && root.idKey !== "m_id"){
            value = item.m_id
        }
        if (value === undefined && root.idKey !== "Id"){
            value = item.Id
        }
        return value === undefined || value === null ? "" : String(value)
    }

    function applyPreselectedItems(){
        let selected = ({})
        let selectedItems = ({})
        let values = root.preselectedItems || []
        for (let i = 0; i < values.length; ++i){
            let value = values[i]
            let id = typeof value === "object" ? itemId(value) : String(value)
            if (id === ""){
                continue
            }
            selected[id] = true
            selectedItems[id] = typeof value === "object" ? value : ({"id": id})
        }
        root._selectedById = selected
        root._selectedItemsById = selectedItems
        root.selectionChanged(selectedItems())
    }

    function selectedItems(){
        let result = []
        for (let id in root._selectedItemsById){
            result.push(root._selectedItemsById[id])
        }
        return result
    }

    function isSelected(item){
        let id = itemId(item)
        return id !== "" && root._selectedById[id] === true
    }

    function toggleItem(item){
        let id = itemId(item)
        if (id === ""){
            return
        }

        let selected = root._selectedById
        let selectedItemsMap = root._selectedItemsById

        if (root.multiSelect){
            if (selected[id]){
                delete selected[id]
                delete selectedItemsMap[id]
            }
            else {
                selected[id] = true
                selectedItemsMap[id] = item
                root.itemSelected(item)
            }
        }
        else {
            selected = ({})
            selectedItemsMap = ({})
            selected[id] = true
            selectedItemsMap[id] = item
            root.itemSelected(item)
            root.close()
        }

        root._selectedById = selected
        root._selectedItemsById = selectedItemsMap
        root.selectionChanged(selectedItems())
    }

    function resetAndFetch(){
        root._requestSerial++
        root._nextCursor = null
        root.hasMore = false
        root.errorMessage = ""
        itemsModel.clear()
        root.highlightedIndex = -1
        fetchPage(null, root._requestSerial)
    }

    function fetchNextPage(){
        if (root.loading || !root.hasMore){
            return
        }
        fetchPage(root._nextCursor, root._requestSerial)
    }

    function fetchPage(cursor, serial){
        if (!root.dataProvider || !root.dataProvider.fetch){
            root.errorMessage = qsTr("Data provider is not configured")
            return
        }

        root.loading = true
        let requestSerial = serial
        let params = {"filter": root.filter, "cursor": cursor, "limit": root.pageLimit}
        let result = root.dataProvider.fetch(params)

        if (!result || !result.then){
            root.loading = false
            root.errorMessage = qsTr("Data provider must return a Promise")
            root.fetchFailed(root.errorMessage)
            return
        }

        result.then(function(response){
            if (requestSerial !== root._requestSerial){
                return
            }

            let items = response && response.items ? response.items : []
            for (let i = 0; i < items.length; ++i){
                itemsModel.append({"itemData": items[i]})
            }

            root._nextCursor = response && response.nextCursor !== undefined ? response.nextCursor : null
            root.hasMore = root._nextCursor !== null && root._nextCursor !== ""
            root.loading = false
            root.errorMessage = ""
            if (root.highlightedIndex < 0 && itemsModel.count > 0){
                root.highlightedIndex = 0
            }
        }).catch(function(error){
            if (requestSerial !== root._requestSerial){
                return
            }

            root.loading = false
            root.errorMessage = String(error)
            root.fetchFailed(root.errorMessage)
        })
    }

    function moveHighlight(delta){
        if (itemsModel.count === 0){
            return
        }
        let index = root.highlightedIndex
        if (index < 0){
            index = 0
        }
        else {
            index = Math.max(0, Math.min(itemsModel.count - 1, index + delta))
        }
        root.highlightedIndex = index
        listView.positionViewAtIndex(index, ListView.Contain)
        if (index >= itemsModel.count - 2){
            fetchNextPage()
        }
    }

    function selectHighlighted(){
        if (root.highlightedIndex < 0 || root.highlightedIndex >= itemsModel.count){
            return
        }
        toggleItem(itemsModel.get(root.highlightedIndex).itemData)
    }
}
