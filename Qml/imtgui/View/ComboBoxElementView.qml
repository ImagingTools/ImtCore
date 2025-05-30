import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

ElementView {
    id: comboBoxElementView;
    bottomComp: isSelectionRequired && currentIndex == -1 ? errorComp : undefined;

    property bool isSelectionRequired: false;
    property string errorText: qsTr("Please select an item");

    property var model: null;
    property bool changeable: true;
    property int shownItemsCount: 5;
    property int currentIndex: -1;
    property var delegate;

    // ID for display in combo box delegates
    property string nameId: "name";

    property ComboBox cbRef: null;

	signal finished(string itemId, int index);

    function setupComboBox(cbRef){
        if (!cbRef){
            return;
        }

        cbRef.changeable = comboBoxElementView.changeable;
        cbRef.nameId = comboBoxElementView.nameId;
		cbRef.model = comboBoxElementView.model;
        cbRef.currentIndex = comboBoxElementView.currentIndex;

        comboBoxElementView.cbRef = cbRef;
    }

    Connections {
        id: cbConn;
		target: comboBoxElementView.cbRef;

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

			Connections {
				target: comboBoxElementView;

				function onModelChanged(){
					if (cb.model != comboBoxElementView.model){
						cb.model = comboBoxElementView.model;
					}
				}

				function onChangeableChanged(){
					if (cb.changeable != comboBoxElementView.changeable){
						cb.changeable = comboBoxElementView.changeable;
					}
				}

				function onCurrentIndexChanged(){
					if (cb.currentIndex != comboBoxElementView.currentIndex){
						cb.currentIndex = comboBoxElementView.currentIndex;
					}
				}
			}
        }
    }

    Component {
        id: errorComp;

        Text {
            text: comboBoxElementView.errorText;
            color: Style.errorTextColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSizeM;
        }
    }
}


