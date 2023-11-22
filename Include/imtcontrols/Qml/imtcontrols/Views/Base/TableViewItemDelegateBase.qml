import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

FocusScope {
    id: delegate;

    width: root ? root.width : 0;
    //width: parent.width;
    height: root ? root.rowItemHeight : 0;

    visible: delegate.isVisible;

    property Item root: null;
    property var dataModel: model;

    property alias mouseArea: mouseAreaBase;

    property ModelIndex modelIndex: ModelIndex {
        itemData: model;
        index: model.index;
        depth: delegate.level;
    };

    property int level: 0;

    property var itemData: model;

    property alias headerDelegate: headerDelegateLoader.sourceComponent;
    property alias headerItem: headerDelegateLoader.item;

    property alias footerDelegate: footerDelegateLoader.sourceComponent;
    property alias footerItem: footerDelegateLoader.item;

    property alias prefixRowDelegate: prefixRowLoaderObj.sourceComponent;
    property alias prefixRowItem: prefixRowLoaderObj.item;

    property alias suffixRowDelegate: suffixRowLoader.sourceComponent;
    property alias suffixRowItem: suffixRowLoader.item;

    property alias rowBodyDelegate: rowLoader.sourceComponent;
    property alias rowBodyItem: rowLoader.item;

    property alias highlightDelegate: highlightLoader.sourceComponent;
    property alias highlightItem: highlightLoader.item;

    property alias prefixRowLoader: prefixRowLoaderObj;

    property bool selected: false;
    property bool isActive: true;
    property bool isVisible: true;
    property bool isCheckable: true;
    property int checkState: Qt.Unchecked;

    signal clicked();

    Component.onDestruction: {
        delegate.root._removeItem(delegate);

        delegate.root.tableSelection.selectionChanged.disconnect(delegate.selectionChanged);
    }

    Component.onCompleted: {
        if (delegate.root){
            delegate.root._addItem(delegate);
            delegate.root.tableSelection.selectionChanged.connect(delegate.selectionChanged);
        }
    }

//    onRootChanged:  {
//        if (delegate.root){
//            delegate.root._addItem(delegate);
//            delegate.root.tableSelection.selectionChanged.connect(delegate.selectionChanged);
//        }
//    }

    function selectionChanged(){
        if (delegate.root){
            delegate.selected = delegate.root.tableSelection.isSelected(delegate.modelIndex);
        }
    }

    function getItemData(){
        return delegate.itemData;
    }

    function select(){
        if (delegate.root.tableSelection.isSelected(delegate.modelIndex)){
            delegate.root.tableSelection.resetSelection();
        }
        else{
            delegate.root.tableSelection.singleSelect(delegate.modelIndex);
        }
    }

    Loader {
        id: headerDelegateLoader;

        anchors.top: parent.top;

        width: parent.width;
    }

    Loader {
        id: highlightLoader;

        width: parent.width;
        height: delegate.root ? delegate.root.rowItemHeight : 0;

        visible: delegate.selected;

        sourceComponent: Rectangle {
            id: highlight;

            color: Style.selectedColor;
        }
    }

    Loader {
        id: prefixRowLoaderObj;

        anchors.left: delegate.left;
        anchors.leftMargin: delegate.level * 20;
        anchors.top: headerDelegateLoader.bottom;
    }

    Loader {
        id: rowLoader;

        anchors.left: prefixRowLoaderObj.right;
        anchors.leftMargin: 10;
        anchors.top: headerDelegateLoader.bottom;

        focus: true;

        sourceComponent: Row {
            id: row;

            height: delegate.root ? delegate.root.rowItemHeight : 0;

            property var columnModel: delegate.root ? delegate.root.columnModel : 0;
            onColumnModelChanged: {
                repeater.model = row.columnModel;
            }

            Repeater {
                id: repeater;

                delegate: Item {
                    id: repeaterItem;

                    width: delegate.root ? delegate.root.width / delegate.root.columnCount : 0;
                    height: delegate.root ? delegate.root.rowItemHeight :0;

                    Text {
                        id: text_;

                        anchors.verticalCenter: repeaterItem.verticalCenter;

                        width: parent.width;

                        font.family: Style.fontFamily;
                        font.pixelSize: Style.fontSize_common;

                        color: delegate.isActive ? Style.textColor : Style.disabledInActiveTextColor;

                        wrapMode: Text.WordWrap;
                        elide: Text.ElideRight;

                        text: delegate.dataModel[model.Id] ? delegate.dataModel[model.Id] : "";
                    }
                }
            }
        }
    }

    Loader {
        id: suffixRowLoader;

        anchors.left: rowLoader.right;
        anchors.top: headerDelegateLoader.bottom;
    }

    MouseArea {
        id: mouseAreaBase;

        anchors.fill: parent;

        z: -1;

        onClicked: {
            delegate.select();

            delegate.clicked();

            delegate.rowBodyItem.forceActiveFocus();
        }
    }

    Loader {
        id: footerDelegateLoader;

        anchors.top: rowLoader.bottom;

        width: parent.width;
    }
}
