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
    property var delegate;

    // ID for display in combo box delegates
    property string nameId: "Name";

    property ComboBox cbRef: null;

    onModelChanged: {
        if (cbRef && model){
            cbRef.model = model;
        }
    }

    onControlItemChanged: {
        if (cbRef && model){
            cbRef.model = model;
        }
    }

    onCurrentIndexChanged: {
        if (cbRef){
            cbRef.currentIndex = currentIndex;
        }
    }

    onChangeableChanged: {
        if (cbRef){
            cbRef.changeable = changeable;
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

        comboBoxElementView.cbRef = cbRef;

        cbConn.target = cbRef;
    }

    Connections {
        id: cbConn;

        function onCurrentIndexChanged(){
            if (!comboBoxElementView.cbRef){
                return;
            }

            if (comboBoxElementView.currentIndex != comboBoxElementView.cbRef.currentIndex){
                comboBoxElementView.currentIndex = comboBoxElementView.cbRef.currentIndex;
            }
        }
    }

    controlComp: cbComp;

    Component {
        id: cbComp;

        ComboBox {
            id: cb;

            width: comboBoxElementView.controlWidth;
            height: comboBoxElementView.controlHeight;

            Component.onCompleted: {
                if (comboBoxElementView.delegate){
                    cb.delegate = comboBoxElementView.delegate;
                }

                comboBoxElementView.setupComboBox(cb);
            }
        }
    }
}


