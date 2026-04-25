import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

FocusScope {
    id: tableViewRoot;

    clip: true;

    property bool headerVisible: true;
    property bool contentVisible: true;
    property bool readOnly: false;
    property bool withoutSelection: false;

    property bool checkable: false;

    property alias rowDelegate: flick.delegate;
    property alias background: backgroundLoader.sourceComponent;

    // Optional header/footer rendered inside the virtualized list (Qt-style).
    property alias headerComponent: flick.header;
    property alias footerComponent: flick.footer;

    // Optional section grouping (delegates to ListView.section.*).
    property alias sectionProperty: flick.section.property;
    property alias sectionCriteria: flick.section.criteria;
    property alias sectionDelegate: flick.section.delegate;

    // Animations for add/remove/displaced — opt-in by assigning a Transition.
    property alias addTransition: flick.add;
    property alias removeTransition: flick.remove;
    property alias displacedTransition: flick.displaced;
    property alias addDisplacedTransition: flick.addDisplaced;
    property alias removeDisplacedTransition: flick.removeDisplaced;

    // Public ListView-style API (previously not exposed).
    property alias currentIndex: flick.currentIndex;
    property alias currentItem: flick.currentItem;
    property alias cacheBuffer: flick.cacheBuffer;
    property alias keyNavigationWraps: flick.keyNavigationWraps;
    property alias reuseItems: flick.reuseItems;

    function positionViewAtIndex(index, mode){
        flick.positionViewAtIndex(index, mode === undefined ? ListView.Visible : mode);
    }

    function positionViewAtBeginning(){
        flick.positionViewAtBeginning();
    }

    function positionViewAtEnd(){
        flick.positionViewAtEnd();
    }

    property int columnCount:repeater.count;
    property int rowCount: flick.count;

	property int rowItemHeight: Style.controlHeightM;
	property int headerHeight: Style.tableHeaderHeight;

    property alias contentHeight: flick.contentHeight;

    property var rowModel: ListModel {};

    onRowModelChanged: {
        tableViewRoot.itemsList = []
    }

    property var columnModel: ListModel {};

    property ModelIndex selectedIndex: null;

    property TreeViewSelection tableSelection: TreeViewSelection {
        onSelectionChanged: {
            if (tableViewRoot.tableSelection.items.length > 0){
                tableViewRoot.selectedIndex = tableViewRoot.tableSelection.items[0];
            }
            else{
                tableViewRoot.selectedIndex = null;
            }

            tableViewRoot.selectionChanged();
        }
    };

    property alias tableListView: flick;

    signal selectionChanged();
    signal rowModelDataChanged(var delegate, var prop);
    signal rowAdded();
    signal rowRemoved();
    signal checkedItemsChanged();

    property var itemsList: [];

    function getCheckedItems(){
        let result = []
        let delegates = tableViewRoot.getItemsDataAsList();
        for (let delegate of delegates){
            if (delegate.checkState === Qt.Checked){
                result.push(delegate);
            }
        }
        return result;
    }

    function checkItem(delegate){
        if (delegate){
            if (delegate.checkState !== Qt.Checked){
                delegate.checkState = Qt.Checked;

                if (delegate.parentDelegate){
                    delegate.parentDelegate.childrenCheckStateChanged(delegate);
                }

                for (let childDelegate of delegate.childrenDelegates){
                    childDelegate.parentCheckStateChanged(delegate);
                }

                tableViewRoot.checkedItemsChanged();
            }
        }
    }

    function uncheckItem(delegate){
//        let delegate = tableViewRoot.getItemByIndex(modelIndex);
        if (delegate){
            if (delegate.checkState !== Qt.Unchecked){
                delegate.checkState = Qt.Unchecked;
                if (delegate.parentDelegate){
                    delegate.parentDelegate.childrenCheckStateChanged(delegate);
                }

                for (let childDelegate of delegate.childrenDelegates){
                    childDelegate.parentCheckStateChanged(delegate);
                }

                tableViewRoot.checkedItemsChanged();
            }
        }
    }

    function checkAll(){
        let ok = false;
        let delegates = tableViewRoot.getItemsDataAsList();
        for (let delegate of delegates){
            if (delegate.checkState !== Qt.Checked){
                delegate.checkState = Qt.Checked;
                ok = true;
            }
        }

        if (ok){
            tableViewRoot.checkedItemsChanged();
        }
    }

    function uncheckAll(){
        let ok = false;
        let delegates = tableViewRoot.getItemsDataAsList();
        for (let delegate of delegates){
            if (delegate.checkState !== Qt.Unchecked){
                delegate.checkState = Qt.Unchecked;
                ok = true;
            }
        }

        if (ok){
            tableViewRoot.checkedItemsChanged();
        }
    }

    function _addItem(item){
        tableViewRoot.itemsList.push(item);
    }

    function _removeItem(item){
        var index = tableViewRoot.itemsList.indexOf(item);
        if (index !== -1){
            tableViewRoot.itemsList.splice(index, 1);
        }
    }

    function getItemsDataAsList(){
        return itemsList;
    }

    function resetSelection(){
        tableViewRoot.tableSelection.resetSelection();
    }

    function singleSelect(item){
        tableViewRoot.tableSelection.singleSelect(item);
    }

    function addChildItem(modelIndex, rowObj){
        if (!rowObj){
            return false;
        }

        let treeModel;
        let currentIndex = 0;

        if (modelIndex){
            treeModel = modelIndex.getModel();
            currentIndex = modelIndex.index;
        }
        else{
            treeModel = rowModel;
        }

        let childModel = modelIndex.getData("ChildModel");

        if (!childModel){
            childModel = treeModel.addTreeModel("ChildModel", currentIndex);
        }

        if (!childModel){
            return false;
        }

        childModel.insertNewItemWithParameters(0, rowObj)

        return true;
    }

    function removeChildItem(modelIndex){
        if (modelIndex){
           let treeModel = modelIndex.getModel();
            if (!treeModel){
                return false;
            }

            let currentIndex = modelIndex.index;

            treeModel.removeItem(currentIndex);

            return true;
        }

        return false;
    }

    function removeByIndex(modelIndex){
        for (let i = 0; i < tableViewRoot.itemsList.length; i++){
            let delegateItem = tableViewRoot.itemsList[i];
            if (delegateItem.modelIndex === modelIndex){
                let parentModel = modelIndex.getParentModel();
                if (!parentModel){
                    parentModel = tableViewRoot.rowModel;
                }
                parentModel.removeItem(modelIndex.getIndex());
                return true;
            }
        }

        return false;
    }

    function getItemByIndex(index){
        let delegates = tableViewRoot.getItemsDataAsList();
        for (let delegate of delegates){
            if (delegate.modelIndex.equal(index)){
                return delegate;
            }
        }

        return null;
    }

    Loader {
        id: backgroundLoader;

        anchors.fill: parent;

        sourceComponent: Rectangle {
            color: Style.baseColor;
        }
    }

    function getRootItemByIndex(index){
        if (index < 0 || index >= flick.count){
            return null;
        }

        return flick.itemAtIndex(index);
    }

    FocusScope {
        id: headerItem;

        z: 5;

        height: visible ? tableViewRoot.headerHeight : 0;
        width: flick.width;

        visible: tableViewRoot.headerVisible;

        property alias headerBackground: headerBackgroundLoader.sourceComponent;

        MouseArea {
            anchors.fill: parent;

            onClicked: {
                headerItem.focus = true;
            }
        }

        Loader {
            id: headerBackgroundLoader;

            anchors.fill: parent;

            sourceComponent: Rectangle {
                id: headerBackground;

                color: Style.alternateBaseColor;

                border.width: 1;
                border.color: Style.imagingToolsGradient2;
            }
        }

        Row {
            id: headerRow;

            focus: true;
            spacing: Style.marginM;

            Repeater {
                id: repeater;

                model: tableViewRoot.columnModel;

                delegate: Item {
                    id: repeaterItem;

                    width: tableViewRoot.columnCount == 0 ? tableViewRoot.width : tableViewRoot.width / tableViewRoot.columnCount;
                    height: tableViewRoot.headerHeight;

                    Text {
                        id: headerText;

                        anchors.left: repeaterItem.left;
                        anchors.leftMargin: Style.marginM;
                        anchors.verticalCenter: repeaterItem.verticalCenter;

                        Component.onCompleted: {
                            headerText.text =  model.name;
                        }

                        width: parent.width;

                        font.pixelSize: Style.fontSizeM;
                        font.family: Style.fontFamilyBold;
                        font.bold: true;
                        color: Style.textColor;
                        elide: Text.ElideRight;
                    }
                }
            }
        }
    }

    function flickableToEnd(){
        flick.contentY = flick.contentHeight;
    }

    property alias flickable: flick;

    // Virtualized list of top-level rows. Replaces the previous
    // Flickable + Column + Repeater combination so that delegates are
    // created lazily as they enter the viewport. ListView itself is a
    // Flickable, so the public `flickable`/`tableListView` aliases (and
    // the existing CustomScrollbar binding to `targetItem: flick`) keep
    // working unchanged.
    ListView {
        id: flick;

        anchors.top: headerItem.bottom;
        anchors.bottom: parent.bottom;

        width: parent.width;

        model: tableViewRoot.rowModel;
        delegate: TableViewItemDelegateBase {
            root: tableViewRoot;
        }

        boundsBehavior: Flickable.StopAtBounds;
        clip: true;
        keyNavigationEnabled: true;
        highlightFollowsCurrentItem: true;
        // High default cacheBuffer keeps every delegate alive (legacy behaviour
        // of the previous Repeater implementation, on which getItemsDataAsList()
        // and check-state iteration rely). Consumers handling large datasets
        // can opt in to virtualization by lowering `cacheBuffer` — e.g.
        // `cacheBuffer: 0` to only keep visible delegates.
        cacheBuffer: 1000000;

        visible: tableViewRoot.contentVisible;
    }

    CustomScrollbar {
        id: scrollbar;

        z: 100;

        anchors.right: flick.right;
        anchors.top: flick.top;

        secondSize: 10;
        targetItem: flick;
        visible: flick.visible;
    }

    function addRow(row){
        return insertRow(rowModel.count, row);
    }

    function addColumn(column){
        return tableViewRoot.insertColumn(columnModel.count, column);
    }

    function insertColumn(index, column){
        columnModel.insert(index, column);
    }

    function insertRow(index, row){
        row["Level"] = 0;

        let hasKey = "Visible" in row;
        if (!hasKey){
            row["Visible"] = true;
        }

        hasKey = "Active" in row;
        if (!hasKey){
            row["Active"] = true;
        }

        hasKey = "Selected" in row;
        if (!hasKey){
            row["Selected"] = false;
        }

        rowModel.insert(index, row);

        rowAdded();
    }

    function removeColumn(index){
        if (index < 0 || index >= columnCount){
            console.warn("TableView::removeColumn(): invalid argument")
            return
        }

        columnModel.remove(index);
    }

    function removeRow(index){
        if (index < 0 || index >= rowCount){
            console.warn("TableView::removeRow(): invalid argument")
            return
        }

        rowModel.remove(index);

        tableViewRoot.rowRemoved();
    }

    function __checkColumn(columnId){
        for (let i = 0; i < columnModel.count; i++){
            let id = columnModel.get(i).id;
            if (columnId == id){
                return true;
            }
        }

        return false;
    }
}
