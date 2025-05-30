import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

TableCellDelegateBase {
    id: bodyItem;
    property var model;

    z: parent.z + 1;

    width: parent.width;
    height: parent.height;

    signal currentIndexChanged(int index);

    onReused: {
        if (rowIndex >= 0 && model){
            cb.model = model;

            if (cb.model){
                for (let i = 0; i < cb.model.getItemsCount(); i++){
                    let value = cb.model.getData(bodyItem.cellHeaderId, i);
                    if (value == bodyItem.getValue()){
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
        font.pixelSize: Style.fontSizeM;
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
            console.log("onFinished", index);
            cb.visible = false;

            if (index >= 0){
                let value = cb.model.getData(bodyItem.cellHeaderId, index);

                console.log("value", value);

                bodyItem.setValue(value);
            }
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

