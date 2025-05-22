import QtQuick 2.0
import Acf 1.0
import com.imtcore.imtqml 1.0

import imtcontrols 1.0

Item {
    id: treeItemDelegate;

    height: childrenColumn.visible ? childrenColumn.height + viewDelegateLoader.height : viewDelegateLoader.height;

    property int itemHeight: 30;

    property TreeItemModel childModel: model.ChildModel ? model.ChildModel : null;

    property Item rootItem: null;

    property int state: model.State;
    property string itemId: model.Id || model.id || "";

    property bool itemVisible: model.Visible;
    property bool itemActive: model.Active;

    property var itemData: model;

    property alias itemDelegate: viewDelegateLoader.sourceComponent;

    property int childrenCount: childModelRepeater.count;

    property alias childRepeater: childModelRepeater;

    signal doubleClicked;
    signal clicked(var itemData, int index);

    onChildModelChanged: {
        childModelRepeater.model = model.ChildModel;
    }

    Loader {
        id: viewDelegateLoader;

        anchors.top: treeItemDelegate.top;

        width: parent.width;
        height: model.Visible ? treeItemDelegate.itemHeight : 0;

        sourceComponent: Rectangle {
            id: body;

            anchors.top: treeItemDelegate.top;
            anchors.right: treeItemDelegate.right;

            width: parent.width;
            height: parent.height;

            visible: treeItemDelegate.itemData ? treeItemDelegate.itemData.Visible : false;

            color: model.Selected ? Style.selectedColor : "transparent";

            MouseArea {
                anchors.fill: parent;

                onClicked: {
                    treeItemDelegate.clicked(model, model.index);
                }

                onDoubleClicked: {
                    treeItemDelegate.doubleClicked();
                }
            }

            Image {
                id: iconArrow;

                anchors.left: parent.left;
                anchors.leftMargin: 10;
                anchors.verticalCenter: parent.verticalCenter;

                width: 10;
                height: 10;

                visible: treeItemDelegate.childrenCount > 0;

                source: model.Opened ? "../../../" + Style.getIconPath("Icons/Down", Icon.State.On, Icon.Mode.Normal) :
                                         "../../../" + Style.getIconPath("Icons/Right", Icon.State.On, Icon.Mode.Normal);

                sourceSize.height: height;
                sourceSize.width: width;

                MouseArea {
                    id: mouseArea;

                    anchors.fill: parent;

                    onClicked: {
                        console.log("iconArrow onClicked");
                        model.Opened = !model.Opened;
                    }
                }
            }

            CheckBox {
                id: checkBox;

                anchors.left: iconArrow.right;
				anchors.leftMargin: Style.sizeSmallMargin;
                anchors.verticalCenter: parent.verticalCenter;

                checkState: model.State;

                visible: model.Level >= 1 && model.Optional;

				function nextCheckState() {
                    if (!model.Active){
                        return;
                    }

                    if (model.State == Qt.PartiallyChecked){
                        model.State = Qt.Checked;
                    }
                    else{
                        model.State = Qt.Checked - model.State;
                    }

                    treeItemDelegate.rootItem.itemsStateChanged(treeItemDelegate.itemData);
                }
            }

            Text {
                id: title;

                anchors.left: checkBox.visible ? checkBox.right : iconArrow.right;
				anchors.leftMargin: Style.sizeMainMargin;
                anchors.verticalCenter: parent.verticalCenter;

                color: model.Active ? Style.textColor : Style.inactiveTextColor;

                font.pixelSize: Style.fontSizeNormal;
                font.family: Style.fontFamily;

                text: model.name;
            }
        }
    }


    Column {
        id: childrenColumn;

        anchors.top: viewDelegateLoader.bottom;
        anchors.right: parent.right;

		width: treeItemDelegate.width - 2*Style.sizeMainMargin;

        visible: model.Opened && model.Visible;

        Repeater {
            id: childModelRepeater;

            delegate: treeItemDelegate.rootItem.itemDelegate;

            onItemAdded: {
                if (treeItemDelegate.itemData.Level >= 1){
                    item.itemData.Parent = treeItemDelegate.itemData;
                    if (!treeItemDelegate.itemData.Children){
                        treeItemDelegate.itemData.Children = [item.itemData];
                    }
                    else{
                        treeItemDelegate.itemData.Children.push(item.itemData);
                    }
                }
            }
        }
    }
}
