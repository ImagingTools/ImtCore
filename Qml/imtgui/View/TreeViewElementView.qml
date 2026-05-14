import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

ElementView {
    id: root;

    property var treeView;

    bottomComp: treeViewComp;

    Component {
        id: treeViewComp;

        FocusScope {
            id: treeViewContainer;

            width: root.contentWidth;
            height: filterInput.height + Style.marginM + treeViewImpl.height;

            SearchTextInput {
                id: filterInput;

                width: parent.width;
                placeHolderText: qsTr("Filter...");

                onSearchChanged: {
                    treeViewImpl.__filterText = filterInput.text;
                    treeViewImpl.__rebuild();
                }
            }

            FocusScope {
                id: treeViewImpl;

                anchors.top: filterInput.bottom;
                anchors.topMargin: Style.marginM;
                width: parent.width;
                height: internalTreeView.contentHeight;

                clip: true;

                Component.onCompleted: {
                    root.treeView = treeViewImpl;
                }

                // --- Public properties ---
                property bool tristate: false;
                property bool readOnly: false;
                property var rowModel: ListModel {};
                property var columnModel: ListModel {};
                property int rowItemHeight: Style.controlHeightM;

                signal checkedItemsChanged();

                // --- Internal state ---
                property string __filterText: "";
                property var __allNodes: [];
                property var __treeModelData: [];

                // --- Public functions ---

                function getItemsDataAsList() {
                    return __allNodes;
                }

                function getCheckedItems() {
                    var result = [];
                    for (var i = 0; i < __allNodes.length; i++) {
                        if (__allNodes[i].checkState === Qt.Checked) {
                            result.push(__allNodes[i]);
                        }
                    }
                    return result;
                }

                function checkItem(node) {
                    if (!node || node.checkState === Qt.Checked) return;
                    node.checkState = Qt.Checked;
                    __propagateDown(node, Qt.Checked);
                    __propagateUp(node);
                    checkedItemsChanged();
                }

                function uncheckItem(node) {
                    if (!node || node.checkState === Qt.Unchecked) return;
                    node.checkState = Qt.Unchecked;
                    __propagateDown(node, Qt.Unchecked);
                    __propagateUp(node);
                    checkedItemsChanged();
                }

                function checkAll() {
                    var changed = false;
                    for (var i = 0; i < __allNodes.length; i++) {
                        if (__allNodes[i].checkState !== Qt.Checked) {
                            __allNodes[i].checkState = Qt.Checked;
                            changed = true;
                        }
                    }
                    if (changed) checkedItemsChanged();
                }

                function uncheckAll() {
                    var changed = false;
                    for (var i = 0; i < __allNodes.length; i++) {
                        if (__allNodes[i].checkState !== Qt.Unchecked) {
                            __allNodes[i].checkState = Qt.Unchecked;
                            changed = true;
                        }
                    }
                    if (changed) checkedItemsChanged();
                }

                // --- Internal: check propagation ---

                function __propagateDown(node, state) {
                    for (var i = 0; i < node.childNodes.length; i++) {
                        var child = node.childNodes[i];
                        if (child.isCheckable) child.checkState = state;
                        __propagateDown(child, state);
                    }
                }

                function __propagateUp(node) {
                    var p = node.parentNode;
                    if (!p || !p.isCheckable) return;

                    var allChecked = true;
                    var allUnchecked = true;
                    for (var i = 0; i < p.childNodes.length; i++) {
                        var cs = p.childNodes[i].checkState;
                        if (cs !== Qt.Checked) allChecked = false;
                        if (cs !== Qt.Unchecked) allUnchecked = false;
                    }

                    p.checkState = allChecked ? Qt.Checked : (allUnchecked ? Qt.Unchecked : Qt.PartiallyChecked);
                    __propagateUp(p);
                }

                // --- Internal: filter ---

                function __matchesFilter(item, filterLower) {
                    if (filterLower === "") return true;
                    for (var i = 0; i < columnModel.count; i++) {
                        var value = item[columnModel.get(i).id];
                        if (value && value.toString().toLowerCase().indexOf(filterLower) >= 0) return true;
                    }
                    return false;
                }

                function __subtreeMatches(sourceModel, filterLower) {
                    for (var i = 0; i < sourceModel.count; i++) {
                        var item = sourceModel.get(i);
                        if (__matchesFilter(item, filterLower)) return true;
                        if (item.ChildModel && item.ChildModel.count > 0 && __subtreeMatches(item.ChildModel, filterLower)) return true;
                    }
                    return false;
                }

                // --- Internal: build tree model from rowModel ---

                function __buildTreeItems(sourceModel, level, parentNode, filterLower) {
                    var items = [];
                    for (var i = 0; i < sourceModel.count; i++) {
                        var item = sourceModel.get(i);

                        // Filter check
                        var selfMatch = __matchesFilter(item, filterLower);
                        var childMatch = (item.ChildModel && item.ChildModel.count > 0) ? __subtreeMatches(item.ChildModel, filterLower) : false;
                        if (filterLower !== "" && !selfMatch && !childMatch) continue;

                        // Create node
                        var node = {
                            sourceItem: item,
                            level: level,
                            checkState: item.CheckState !== undefined ? item.CheckState : Qt.Unchecked,
                            isCheckable: item.CheckBoxVisible !== undefined ? item.CheckBoxVisible : true,
                            isActive: item.Active !== undefined ? item.Active : true,
                            parentNode: parentNode,
                            childNodes: []
                        };
                        node.getItemData = function() { return this.sourceItem; }.bind(node);

                        if (parentNode) parentNode.childNodes.push(node);
                        var nodeIndex = __allNodes.length;
                        __allNodes.push(node);

                        // Recurse children
                        var childTreeItems = [];
                        if (item.ChildModel && item.ChildModel.count > 0) {
                            childTreeItems = __buildTreeItems(item.ChildModel, level + 1, node, filterLower);
                        }

                        // Build display text from columns
                        var displayText = "";
                        for (var c = 0; c < columnModel.count; c++) {
                            var val = item[columnModel.get(c).id];
                            if (val !== undefined) {
                                displayText += (displayText !== "" ? " | " : "") + val;
                            }
                        }

                        items.push({
                            displayText: displayText,
                            nodeIndex: nodeIndex,
                            children: childTreeItems
                        });
                    }
                    return items;
                }

                function __rebuild() {
                    __allNodes = [];
                    __treeModelData = rowModel ? __buildTreeItems(rowModel, 0, null, __filterText.toLowerCase()) : [];
                    internalTreeView.expandRecursively();
                }

                function __toggleCheck(nodeIndex) {
                    var node = __allNodes[nodeIndex];
                    if (node.checkState === Qt.Checked) {
                        uncheckItem(node);
                    } else {
                        checkItem(node);
                    }
                    __rebuild();
                }

                onRowModelChanged: __rebuild();

                // --- JQML2 TreeView ---
                TreeView {
                    id: internalTreeView;

                    width: parent.width;
                    height: contentHeight;

                    model: treeViewImpl.__treeModelData;
                    rowHeightProvider: function() { return treeViewImpl.rowItemHeight; };

                    delegate: Item {
                        id: treeDelegate;

                        required property bool isTreeNode;
                        required property bool expanded;
                        required property bool hasChildren;
                        required property int depth;
                        required property var treeView;
                        required property var model;
                        required property int row;

                        implicitWidth: internalTreeView.width;
                        implicitHeight: treeViewImpl.rowItemHeight;

                        Row {
                            id: nodeRow;

                            anchors.fill: parent;
                            leftPadding: treeDelegate.depth * 20;
                            spacing: Style.spacingM;

                            // Expand/collapse arrow
                            Item {
                                width: 15;
                                height: nodeRow.height;

                                Button {
                                    id: arrowButton;

                                    anchors.verticalCenter: parent.verticalCenter;
                                    anchors.horizontalCenter: parent.horizontalCenter;
                                    width: parent.width;
                                    height: width;

                                    visible: treeDelegate.hasChildren;

                                    iconSource: treeDelegate.expanded
                                        ? "../../../" + Style.getIconPath("Icons/Down", Icon.State.On, Icon.Mode.Normal)
                                        : "../../../" + Style.getIconPath("Icons/Right", Icon.State.On, Icon.Mode.Normal);

                                    onClicked: treeDelegate.treeView.toggleExpanded(treeDelegate.row);

                                    decorator: Component {
                                        ButtonDecorator {
                                            color: parent.hovered ? Style.buttonHoverColor : "transparent";
                                            border.width: 0;
                                        }
                                    }
                                }
                            }

                            // Checkbox
                            Item {
                                width: checkBox.visible ? 15 : 0;
                                height: nodeRow.height;

                                CheckBox {
                                    id: checkBox;

                                    anchors.verticalCenter: parent.verticalCenter;
                                    anchors.horizontalCenter: parent.horizontalCenter;

                                    property var __node: (treeDelegate.model.nodeIndex !== undefined && treeDelegate.model.nodeIndex >= 0 && treeDelegate.model.nodeIndex < treeViewImpl.__allNodes.length) ? treeViewImpl.__allNodes[treeDelegate.model.nodeIndex] : null;

                                    checkState: __node ? __node.checkState : Qt.Unchecked;
                                    isActive: __node ? (__node.isActive && !treeViewImpl.readOnly) : !treeViewImpl.readOnly;
                                    visible: treeViewImpl.tristate && (__node ? __node.isCheckable : true);

                                    function nextCheckState() {
                                        if (treeDelegate.model.nodeIndex !== undefined) {
                                            treeViewImpl.__toggleCheck(treeDelegate.model.nodeIndex);
                                        }
                                    }
                                }
                            }

                            // Display text
                            Item {
                                width: nodeRow.width - nodeRow.leftPadding - 15 - (checkBox.visible ? 15 : 0) - Style.spacingM * 2;
                                height: nodeRow.height;

                                Text {
                                    anchors.verticalCenter: parent.verticalCenter;
                                    width: parent.width;

                                    font.family: Style.fontFamily;
                                    font.pixelSize: Style.fontSizeM;
                                    color: checkBox.__node ? (checkBox.__node.isActive ? Style.textColor : Style.inactiveTextColor) : Style.textColor;

                                    wrapMode: Text.WordWrap;
                                    elide: Text.ElideRight;

                                    text: treeDelegate.model.displayText !== undefined ? treeDelegate.model.displayText : "";
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
