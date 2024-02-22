import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

Component {
    id: textInputComp;

    Item {
        id: bodyItem;

        property Item tableCellDelegate: null;

        z: parent.z + 1;

        width: parent.width;
        height: 25;

        onTableCellDelegateChanged: {
            if (tableCellDelegate){
                let paramModel = tableCellDelegate.getValue();

                let elementsModel = paramModel.GetData("Elements");
                if (!elementsModel){
                    return;
                }

                cb.model = elementsModel;

                if (paramModel.ContainsKey("CurrentIndex")){
                    let index = paramModel.GetData("CurrentIndex")

                    if (index >= 0){
                        textLabel.text = elementsModel.GetData("Name", index);
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
        }

        ComboBox {
            id: cb;

            width: parent.width;
            height: 25;

            visible: false;

            onCurrentIndexChanged: {
                cb.visible = false;

                if (bodyItem.tableCellDelegate){
                    if (cb.model){
                        let name = cb.model.GetData("Name", cb.currentIndex)

                        textLabel.text = name;

                        let valueModel = bodyItem.tableCellDelegate.getValue();
                        valueModel.SetData("CurrentIndex", cb.currentIndex);

                        bodyItem.tableCellDelegate.setValue(valueModel);
                    }
                }
            }
        }

        MouseArea {
            anchors.fill: parent;

            onDoubleClicked: {
                cb.visible = true;

                cb.forceActiveFocus();
            }
        }
    }
}

