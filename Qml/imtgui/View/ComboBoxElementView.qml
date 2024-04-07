import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

ElementView {
    id: root;

    property TreeItemModel model: null;
    property bool changeable: true;
    property int shownItemsCount: 5;
    property int currentIndex: -1;

    // ID for display in combo box delegates
    property string nameId: "Name";

    onModelChanged: {
        if (controlItem && model){
            controlItem.model = model;
        }
    }

    onControlItemChanged: {
        if (controlItem && model){
            controlItem.model = model;
        }
    }

    onCurrentIndexChanged: {
        if (controlItem){
            controlItem.currentIndex = currentIndex;
        }
    }

    controlComp: Component {
        ComboBox {
            width: 300;
            height: 30;

            changeable: root.changeable;
            nameId: root.nameId;
            currentIndex: root.currentIndex;

            onCurrentIndexChanged: {
                if (root.currentIndex != currentIndex){
                    root.currentIndex = currentIndex;
                }
            }
        }
    }
}


