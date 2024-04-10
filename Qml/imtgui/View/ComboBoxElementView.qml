import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

ElementView {
    id: comboBoxElementView;

    property var model: null;
    property bool changeable: true;
    property int shownItemsCount: 5;
    property int currentIndex: -1;

    // ID for display in combo box delegates
    property string nameId: "Name";

    QtObject {
        id: internal;

        property ComboBox cbRef: null;
    }

    onModelChanged: {
        if (internal.cbRef && model){
            internal.cbRef.model = model;
        }
    }

    onControlItemChanged: {
        if (internal.cbRef && model){
            internal.cbRef.model = model;
        }
    }

    onCurrentIndexChanged: {
        if (internal.cbRef){
            internal.cbRef.currentIndex = currentIndex;
        }
    }

    onChangeableChanged: {
        if (internal.cbRef){
            internal.cbRef.changeable = changeable;
        }
    }

    function setupComboBox(cbRef){
        if (!cbRef){
            return;
        }

        cbRef.changeable = comboBoxElementView.changeable;
        cbRef.nameId = comboBoxElementView.nameId;
        cbRef.model = comboBoxElementView.model;
        cbRef.currentIndex = comboBoxElementView.currentIndex;

        internal.cbRef = cbRef;

        cbConn.target = cbRef;
    }

    Connections {
        id: cbConn;

        function onCurrentIndexChanged(){
            if (!internal.cbRef){
                return;
            }

            if (comboBoxElementView.currentIndex != internal.cbRef.currentIndex){
                comboBoxElementView.currentIndex = internal.cbRef.currentIndex;
            }
        }
    }

    controlComp: cbComp;

    Component {
        id: cbComp;

        ComboBox {
            id: cb;

            width: 300;
            height: 30;

            Component.onCompleted: {
                comboBoxElementView.setupComboBox(cb);
            }
        }
    }
}


