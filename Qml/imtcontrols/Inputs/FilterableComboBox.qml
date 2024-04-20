import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

ComboBox {
    id: comboBoxContainer;

    property string filter;

    property TreeItemModel proxyModel: TreeItemModel {}

    onModelChanged: {
        if (model){
            proxyModel.Copy(model);
        }
    }

    //    onFilterChanged: {
    //        proxyModel.Clear();

    //        for (let i = 0; i < model.GetItemsCount(); i++){
    //            if (model.ContainsKey(comboBoxContainer.nameId, i)){
    //                let data = model.GetData(comboBoxContainer.nameId, i);
    //                if (data.toLowerCase().indexOf(filter.toLowerCase()) >= 0){
    //                    let index = proxyModel.InsertNewItem();

    //                    proxyModel.SetData("Id", model.GetData("Id", i), index);
    //                    proxyModel.SetData(comboBoxContainer.nameId, model.GetData(comboBoxContainer.nameId, i), index);
    //                }
    //            }
    //        }
    //    }

    Repeater {
        id: helperRepeater;

        model: comboBoxContainer.model;

        visible: false;

        delegate: Component {
            Item {
                property string displayText: model[comboBoxContainer.nameId];
                property bool acceptable: displayText.toLowerCase().indexOf(comboBoxContainer.filter.toLowerCase()) >= 0;

                onAcceptableChanged: {
                    console.log("onAcceptableChanged", acceptable, model.index);
                    if (!acceptable){
                        for (let i = 0; i < comboBoxContainer.proxyModel.GetItemsCount(); i++){
                            if (comboBoxContainer.proxyModel.GetData("Id", i) == model.Id){
                                comboBoxContainer.proxyModel.RemoveItem(i)

                                break;
                            }
                        }
                    }
                    else{
                        let index = comboBoxContainer.proxyModel.InsertNewItem();
                        comboBoxContainer.proxyModel.CopyItemDataFromModel(index, comboBoxContainer.model, model.index)
                    }
                }
            }
        }
    }

//    delegate: Component {PopupMenuDelegate {
//            width: comboBoxContainer.width;
//            height: acceptable ? comboBoxContainer.itemHeight : 0;

//            visible: height > 0;

//            text: displayText;

//            highlighted: comboBoxContainer.currentIndex === model.index
//            selected: comboBoxContainer.popup ? comboBoxContainer.popup.selectedIndex === model.index : false;

//            property string displayText: model[comboBoxContainer.nameId];
//            property string filter: comboBoxContainer.filter;

//            property bool acceptable: displayText.toLowerCase().indexOf(filter.toLowerCase()) >= 0;

//            onClicked: {
//                if (comboBoxContainer.popup){
//                    comboBoxContainer.popup.finished(model.Id, model.index)
//                }
//            }

//            onEntered: {
//                if (comboBoxContainer.popup){
//                    comboBoxContainer.popup.selectedIndex = model.index;
//                }
//            }

//            Component.onCompleted: {
//                console.log("PopupMenuDelegate onCompleted");
//            }
//        }
//    }

    property CustomTextField textInput;

    popupMenuComp: Component {
        id: popupMenu;

        PopupMenuDialog {
            id: popup;

            width: comboBoxContainer.width;
            itemHeight: comboBoxContainer.itemHeight;

            delegate: comboBoxContainer.delegate;
            hiddenBackground: comboBoxContainer.hiddenBackground;
            shownItemsCount: comboBoxContainer.shownItemsCount;
            moveToEnd: comboBoxContainer.moveToEnd;
            moveToIndex: comboBoxContainer.moveToIndex;
            visibleScrollBar: comboBoxContainer.visibleScrollBar;
            selectedIndex: comboBoxContainer.currentIndex;

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

            decorator: Component {PopupDecorator {
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

                                anchors.verticalCenter: parent.verticalCenter;
                                anchors.left: parent.left;
                                anchors.right: parent.right;
                                anchors.rightMargin: Style.size_mainMargin;

                                height: parent.height;

                                color: "transparent";
                                borderWidth: 0;

                                text: comboBoxContainer.currentText;

                                onTextChanged: {
                                    if (!contentItem.filterEnabled){
                                        contentItem.filterEnabled = true;

                                        return;
                                    }

                                    comboBoxContainer.filter = text;
                                }

                                Component.onCompleted: {
                                    comboBoxContainer.textInput = textField;
                                }
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
                                          "model": comboBoxContainer.proxyModel,
                                          "width": comboBoxContainer.width});

        if (textInput){
            textInput.focus = true;
            textInput.forceActiveFocus();
        }
    }
}
