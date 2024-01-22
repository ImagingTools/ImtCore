import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

ComboBox {
    id: comboBoxContainer;

    property string filter;

    delegate: Component{
        Item {
            width: comboBoxContainer.width;
            height: acceptable ? comboBoxContainer.itemHeight : 0;

            property string displayText: model[comboBoxContainer.nameId];
            property string filter: comboBoxContainer.filter;

            visible: height > 0;

            property bool acceptable: displayText.toLowerCase().indexOf(filter.toLowerCase()) >= 0;

            Rectangle{
                id: background;
                anchors.fill: parent;

                color: comboBoxContainer.currentIndex == model.index ? Style.selectedColor : "transparent";
            }

            Text {
                id: mainText;

                anchors.left: parent.left;
                anchors.leftMargin: 10;
                anchors.right: parent.right;
                anchors.rightMargin: 10;
                anchors.verticalCenter: parent.verticalCenter;

                color: Style.textColor;
                font.pixelSize: Style.fontSize_common;
                font.family: Style.fontFamily;

                text: parent.displayText;

                elide: Text.ElideRight;
            }

            MouseArea {
                id: mouseArea;

                anchors.fill: parent;

                hoverEnabled: true;
                cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor;

                visible: !comboBoxContainer ? true : !comboBoxContainer.hoverBlocked;
                onEntered: {
                    if(comboBoxContainer && comboBoxContainer.selectedIndex !== undefined){
                        comboBoxContainer.selectedIndex = model.index;
                    }
                }

                onClicked: {
                    comboBoxContainer.finished(model.Id, model.index)
                }
            }
        }
    }

    popupMenuComp: Component {
        id: popupMenu;

        PopupMenuDialog {
            id: popup;

            delegate: comboBoxContainer.delegate;
            width: comboBoxContainer.width;
            itemHeight: comboBoxContainer.itemHeight;
            hiddenBackground: comboBoxContainer.hiddenBackground;
            textSize: comboBoxContainer.textSize;
            fontColor: comboBoxContainer.fontColor;
            shownItemsCount: comboBoxContainer.shownItemsCount;
            moveToEnd: comboBoxContainer.moveToEnd;
            moveToIndex: comboBoxContainer.moveToIndex;
            rootItem: comboBoxContainer;
            visibleScrollBar: comboBoxContainer.visibleScrollBar;
            onFinished: {
                comboBoxContainer.currentIndex = index;
                comboBoxContainer.isOpen = false;
                popup.root.closeDialog();
            }
            Component.onCompleted: {
                comboBoxContainer.finished.connect(popup.finished);

                if(comboBoxContainer.decorator_){
                    comboBoxContainer.decorator_.textVisible = false;
                }
            }

            Component.onDestruction: {
                if(comboBoxContainer.decorator_){
                    comboBoxContainer.decorator_.textVisible = true;
                }
            }

            topContentLoaderSourceComp: Component {
                Item {
                    id: contentItem;

                    width: popup.width;
                    height: 25;

                    property bool filterEnabled: false;

                    Component.onCompleted: {
                        popup.finished.connect(function(){ textField.text = "";});
                    }

                    CustomTextField {
                        id: textField;

                        anchors.centerIn: parent;

                        width: parent.width - 10;
                        height: parent.height;

                        color: "transparent";
                        borderWidth: 0;

                        text: comboBoxContainer.currentText;

                        Component.onCompleted: {
                            forceActiveFocus();
                        }

                        onTextChanged: {
                            if (!contentItem.filterEnabled){
                                contentItem.filterEnabled = true;

                                return;
                            }

                            comboBoxContainer.filter = text;
                        }
                    }
                }
            }
        }
    }

    function openPopupMenu(){
        comboBoxContainer.isOpen = true;
        comboBoxContainer.dialogsCountPrev = modalDialogManager.count;
        var point = comboBoxContainer.mapToItem(null, 0, comboBoxContainer.height);
        modalDialogManager.openDialog(popupMenuComp, { "x":     point.x,
                                          "y":     point.y - (comboBoxContainer.height - 2),
                                          "model": comboBoxContainer.model,
                                          "width": comboBoxContainer.width});
    }
}
