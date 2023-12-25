import QtQuick 2.0
import Acf 1.0
import imtgui  1.0
import imtqml  1.0

TreeItemDelegate {
    id: treeItemDelegate;

    itemDelegate: Component {
        Rectangle {
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
                anchors.leftMargin: 5;
                anchors.verticalCenter: parent.verticalCenter;

                checkState: model.State;

                visible: model.Level >= 1;

                onClicked: {
                    if (!model.Active){
                        return;
                    }
                    console.log("onClicked", treeItemDelegate.itemData.Id);

                    if (model.State == Qt.PartiallyChecked){
                        model.State = Qt.Checked;
                    }
                    else{
                        model.State = Qt.Checked - model.State;
                    }

                    treeView.itemsStateChanged(treeItemDelegate.itemData);

                    //                treeView.childrenStateChanged(itemData, model.State);
                    //                treeView.parentStateChanged(itemData, model.State);
                }
            }

            Text {
                id: title;

                anchors.left: checkBox.visible ? checkBox.right : iconArrow.right;
                anchors.leftMargin: 10;
                anchors.verticalCenter: parent.verticalCenter;

                color: model.Active ? Style.textColor : Style.disabledInActiveTextColor;

                font.pixelSize: Style.fontSize_common;
                font.family: Style.fontFamily;

                text: model.Name;
            }
        }
    }
}

