import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

TableCellDelegateBase {
    id: bodyItem;
    property var model;

    z: parent.z + 1;

    width: parent.width;
    height: parent.height;

    signal currentIndexChanged(int index);

    onRowIndexChanged: {
        if (rowIndex >= 0 && model){
            cb.model = model;

            if (cb.model){
                for (let i = 0; i < cb.model.getItemsCount(); i++){
                    let id = cb.model.getData("Id", i);
                    if (String(id) == bodyItem.cellHeaderId){
                        cb.currentIndex = i;
                        break;
                    }
                }
            }
        }
    }

    Text {
        id: textLabel;
        anchors.verticalCenter: parent.verticalCenter;
        width: parent.width;
        color: Style.textColor;
        font.family: Style.fontFamily;
        font.pixelSize: Style.fontSize_common;
        text: bodyItem.getValue();
    }

    ComboBox {
        id: cb;
        width: parent.width;
        height: bodyItem.height;
        visible: false;
        nameId: bodyItem.cellHeaderId;

        onCurrentIndexChanged: {
            cb.visible = false;
            bodyItem.currentIndexChanged(cb.currentIndex);
        }

        onFinished: {
            cb.visible = false;
        }

        onFocusChanged: {
            if (!focus){
                cb.visible = false;
            }
        }
    }

    MouseArea {
        id: ma;
        anchors.fill: parent;
        onDoubleClicked: {
            if (cb.model && cb.model.getItemsCount() > 0){
                cb.visible = true;
                cb.z = ma.z + 1;
                cb.forceActiveFocus();
                cb.openPopupMenu();
            }
        }
    }
}

