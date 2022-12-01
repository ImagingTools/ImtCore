import QtQuick 2.12
import Acf 1.0

FocusScope {
    id: delegate;

    property var root: tableViewRoot;

    property alias mouseArea: mouseAreaBase;

    width: root.width;
    height: root.rowItemHeight;

    visible: model.Visible;

    property ModelIndex modelIndex: ModelIndex {
        itemData: model;
        index: model.index;
//        delegateItem: delegate;
    };

    onActiveFocusChanged: {
        console.log("TableViewItemDelegate onActiveFocusChanged", activeFocus);
    }

    onFocusChanged: {
        console.log("TableViewItemDelegate onFocusChanged", focus);
    }

    Component.onCompleted: {
        console.log("TableViewItemDelegate onCompleted", model.Id, model.Name);
    }

    property bool selected: false;

    property var itemData: model;

    property alias headerDelegate: headerDelegateLoader.sourceComponent;
    readonly property alias headerItem: headerDelegateLoader.item;

    property alias footerDelegate: footerDelegateLoader.sourceComponent;
    readonly property alias footerItem: footerDelegateLoader.item;

    property alias prefixRowDelegate: prefixRowLoader.sourceComponent;
    readonly property alias prefixRowItem: prefixRowLoader.item;

    property alias suffixRowDelegate: suffixRowLoader.sourceComponent;
    readonly property alias suffixRowItem: suffixRowLoader.item;

    property alias rowBodyDelegate: rowLoader.sourceComponent;
    readonly property alias rowBodyItem: rowLoader.item;

    property alias highlightDelegate: highlightLoader.sourceComponent;
    readonly property alias highlightItem: highlightLoader.item;

    readonly property alias prefixRowLoader: prefixRowLoader;

    signal clicked();

    Loader {
        id: headerDelegateLoader;

        anchors.top: parent.top;

        width: parent.width;
    }

    Loader {
        id: highlightLoader;

        width: parent.width;
        height: root.rowItemHeight;

        sourceComponent: Rectangle {
            id: highlight;

            visible: model.Selected;

            color: Style.selectedColor;
        }
    }

    Loader {
        id: prefixRowLoader;

        anchors.left: delegate.left;
        anchors.leftMargin: model.Level * 20;
        anchors.top: headerDelegateLoader.bottom;
    }

    Loader {
        id: rowLoader;

        anchors.left: prefixRowLoader.right;
        anchors.leftMargin: 10;
        anchors.top: headerDelegateLoader.bottom;

        focus: true;

        sourceComponent: Row {
            id: row;

            height: root.rowItemHeight;

            property var values: [];

            Repeater {
                id: repeater;

                delegate: Item {
                    id: repeaterItem;

                    width: root.width / root.columnCount;
                    height: root.rowItemHeight;

                    Text {
                        anchors.verticalCenter: repeaterItem.verticalCenter;

                        text: row.values[model.index]

                        width: parent.width;

                        font.family: Style.fontFamily;
                        font.pixelSize: Style.fontSize_common;
                        color: itemData.Active ? Style.textColor : Style.disabledInActiveTextColor;

                        wrapMode: Text.WordWrap;
                        elide: Text.ElideRight;
                    }

                    Component.onCompleted: {
                        if (model.index == 0){
                            width = root.width / root.columnCount - root.shiftLevel * model.Level;
                        }
                    }
                }
            }

            Component.onCompleted: {
                for (let i = 0; i < root.columnModel.count; i++){
                    let id = root.columnModel.get(i).Id;
                    values.push(model[id])
                }

                repeater.model = root.columnModel;
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
            console.log("onClicked");

            if (root.selection.contains(model)){
                root.selection.deselect(model);
                root.selectedIndex = null;
            }
            else{
                root.selection.select(model);
                root.selectedIndex = modelIndex;
            }

            delegate.clicked();
            console.log("delegate.focus = true");

            rowBodyItem.forceActiveFocus();

            root.focus = true;
//            delegate.focus = true;

//            delegate.forceActiveFocus();
        }
    }

    Loader {
        id: footerDelegateLoader;

        anchors.top: rowLoader.bottom;

        width: parent.width;
    }
}
