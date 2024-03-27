import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

ComboBox {
    id: comboBoxContainer;

    property string filter;

    delegate: PopupMenuDelegate {
        width: comboBoxContainer.width;
        height: acceptable ? comboBoxContainer.itemHeight : 0;

        visible: height > 0;

        text: displayText;

        highlighted: comboBoxContainer.currentIndex == model.index

        property string displayText: model[comboBoxContainer.nameId];
        property string filter: comboBoxContainer.filter;

        property bool acceptable: displayText.toLowerCase().indexOf(filter.toLowerCase()) >= 0;

        onClicked: {
            if (comboBoxContainer.popup){
                comboBoxContainer.popup.finished(model.Id, model.index)
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
            shownItemsCount: comboBoxContainer.shownItemsCount;
            moveToEnd: comboBoxContainer.moveToEnd;
            moveToIndex: comboBoxContainer.moveToIndex;
            visibleScrollBar: comboBoxContainer.visibleScrollBar;

            Component.onCompleted: {
                if(comboBoxContainer.decorator_){
                    comboBoxContainer.decorator_.textVisible = false;
                }
            }

            Component.onDestruction: {
                if(comboBoxContainer.decorator_){
                    comboBoxContainer.decorator_.textVisible = true;
                }
            }

            decorator: PopupDecorator {
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
                                focus = true;
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

            onFinished: {
                comboBoxContainer.finished(commandId, index)
            }

            onStarted: {
                comboBoxContainer.popup = popup;
            }
        }
    }

    function openPopupMenu(){
        comboBoxContainer.isOpen = true;
        var point = comboBoxContainer.mapToItem(null, 0, comboBoxContainer.height);
        modalDialogManager.openDialog(popupMenuComp, {
                                          "x":     point.x,
                                          "y":     point.y - (comboBoxContainer.height - 2),
                                          "model": comboBoxContainer.model,
                                          "width": comboBoxContainer.width});
    }
}
