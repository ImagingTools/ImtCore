import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

ComboBox {
    id: comboBoxContainerGql;

    width: 90;
    height: 30;

    popupMenuComp: Component {
        id: popupMenu;
        PopupMenuDialogGql {
            id: popup;

            rootItem: comboBoxContainerGql;
            offset: comboBoxContainerGql.offset;
            count: comboBoxContainerGql.count;
            commandId: comboBoxContainerGql.commandId;
            filterName: comboBoxContainerGql.filterName;
            delegate: comboBoxContainerGql.delegate;
            properties: comboBoxContainerGql.properties;
            gettedParams: comboBoxContainerGql.gettedParams;
            filterIdsModel: comboBoxContainerGql.filterIdsModel;
            delegateRadius: comboBoxContainerGql.delegateRadius;
            hiddenBackground: comboBoxContainerGql.hiddenBackground;
            itemHeight: comboBoxContainerGql.itemHeight;
            filterHeight: comboBoxContainerGql.filterHeiht;
            textSize: comboBoxContainerGql.textSize;
            fontColor: comboBoxContainerGql.fontColor;
            canClose: comboBoxContainerGql.canClose;
            preventFirstLoading: comboBoxContainerGql.preventFirstLoading;
            pauseDuration: comboBoxContainerGql.pauseDuration;
            excludeFilterPart: comboBoxContainerGql.excludeFilterPart;
            canUpdateModel: comboBoxContainerGql.canUpdateModel;

            Component.onCompleted: {
                comboBoxContainerGql.finished.connect(popup.finished);
                popup.clearSignal.connect(comboBoxContainerGql.clearSignal);
            }

            onFilterTextChanged: {
                comboBoxContainerGql.filterText = popup.filterText;
                comboBoxContainerGql.currentIndex = -1;
                comboBoxContainerGql.currentText = popup.filterText;
            }
            onFinished: {
                if (index > -1){
                    for (var item = 0; item < comboBoxContainerGql.gettedParams.GetItemsCount(); item++){
                        let param = comboBoxContainerGql.gettedParams.GetData("Name",  item);
                        let value = popup.model.GetData(param, index);
                        console.log(param, " = ", value);
                        comboBoxContainerGql.gettedParams.SetData("Value", value, item);
                    }
                }
                comboBoxContainerGql.setCurrentText(popup.model,index)
                if (comboBoxContainerGql.currentText == ""){
                    comboBoxContainerGql.currentText = popup.filterText;
                }

                comboBoxContainerGql.currentIndex = index;

                if(popup.canClose){
                    popup.root.closeDialog();
                }

            }
            onPropertiesChanged: {
                comboBoxContainerGql.currentIndex = -1;
                comboBoxContainerGql.currentText = "";
            }
        }
    }

    property TreeItemModel properties : TreeItemModel{};
    property TreeItemModel gettedParams : TreeItemModel{};
    property TreeItemModel filterIdsModel : TreeItemModel{};

    property bool isColor: false;
    property bool backVisible: true;
    property bool canClose: true;
    property bool closeEmpty: false;
    property bool complexModel: false;
    property bool preventFirstLoading: false;

    property int radius: 5;
    property int offset: 0;
    property int count: 15;
    property int delegateRadius: 0;
    property int filterHeiht: 30;
    property int textDelegateSize: Style.fontSize_common;

    property string commandId: "";
    property string filterName: "Name";
    property string filterText: "";
    property bool keepFilterText: false;
    property bool inFocus: false;
    property int pauseDuration: 500;

    property string excludeFilterPart: "";
    property bool canUpdateModel: true;

    property bool doNotCorrectPosition : false;

    signal setCurrentText(var modelll, int index);
    signal editSignal();
    signal closeSignal(var model_);
    signal clearSignal();
    signal closeEmptySignal();

    onSetCurrentText: {
        if(!comboBoxContainerGql.complexModel){
            comboBoxContainerGql.currentText = modelll.GetData(comboBoxContainerGql.nameId, index);
        }
    }

    function openPopupMenu(){
        comboBoxContainerGql.dialogsCountPrev = modalDialogManager.count;
        //var point = comboBoxContainerGql.mapToItem(thumbnailDecoratorContainer, 0, 0);
        var point = comboBoxContainerGql.mapToItem(null, 0, 0);
        var filterText_ = comboBoxContainerGql.keepFilterText ? comboBoxContainerGql.currentText : "";
        modalDialogManager.openDialog(popupMenu, { "x":     point.x,
                                                   "y":     point.y,
                                                   "filterText": filterText_,
                                                   "model": comboBoxContainerGql.model,
                                                   "width": comboBoxContainerGql.width,
                                                   "countVisibleItem": 5 });
    }

    function closeFunc(){
        if(comboBoxContainerGql.closeEmpty){
            modalDialogManager.closeDialog();
            closeEmptySignal();
        }
    }
}
