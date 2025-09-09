import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtguigql 1.0
import imtcontrols 1.0

ElementView {
	id: comboBoxGqlElementView;
	bottomComp: isSelectionRequired && currentIndex == -1 ? errorComp : undefined;

	property bool isSelectionRequired: false;
	property string errorText: qsTr("Please select an item");

	property int itemHeight: Style.controlHeightL;
	property int shownItemsCount: 5;
	property string placeHolderText: "";
	property string commandId;
	property string nameId: "name";
	property var fields: [];
	property var textFilteringInfoIds: [];
	property int currentIndex: -1;
	property string currentText: "";
	property string filterText: "";
	property SearchComp cbRef: null;

	signal finished(string itemId, int index);

	function setupComboBox(cbRef){
		if (!cbRef){
			return;
		}

		cbRef.itemHeight = comboBoxGqlElementView.itemHeight;
		cbRef.shownItemsCount = comboBoxGqlElementView.shownItemsCount;
		cbRef.placeHolderText = comboBoxGqlElementView.placeHolderText;
		cbRef.commandId = comboBoxGqlElementView.commandId;
		cbRef.nameId = comboBoxGqlElementView.nameId;
		cbRef.dataProvider.fields = comboBoxGqlElementView.fields;
		cbRef.dataProvider.textFilteringInfoIds = comboBoxGqlElementView.textFilteringInfoIds;
		cbRef.currentIndex = comboBoxGqlElementView.currentIndex;
		cbRef.currentText = comboBoxGqlElementView.currentText;
		cbRef.filterText = comboBoxGqlElementView.filterText;

		comboBoxGqlElementView.cbRef = cbRef;
	}

	Connections {
		id: cbConn;
		target: comboBoxGqlElementView.cbRef;

		function onCurrentIndexChanged(){
			if (!comboBoxGqlElementView.cbRef){
				return;
			}

			if (comboBoxGqlElementView.currentIndex != comboBoxGqlElementView.cbRef.currentIndex){
				comboBoxGqlElementView.currentIndex = comboBoxGqlElementView.cbRef.currentIndex;
			}
		}

		function onCurrentTextChanged(){
			if (!comboBoxGqlElementView.cbRef){
				return;
			}

			if (comboBoxGqlElementView.currentText != comboBoxGqlElementView.cbRef.currentText){
				comboBoxGqlElementView.currentText = comboBoxGqlElementView.cbRef.currentText;
			}
		}
	}

	controlComp: cbComp;

	Component {
		id: cbComp;

		SearchComp{
			id: cb;

			width: comboBoxGqlElementView.controlWidth;
			height: comboBoxGqlElementView.controlHeight;

			Component.onCompleted: {
				comboBoxGqlElementView.setupComboBox(cb);
			}

			onFinished: {
				comboBoxGqlElementView.finished(itemId, index)
				cb.popup.closeCombo();
			}

			Connections {
				target: comboBoxGqlElementView;

				function onCurrentIndexChanged(){
					if (cb.currentIndex != comboBoxGqlElementView.currentIndex){
						cb.currentIndex = comboBoxGqlElementView.currentIndex;
					}
				}

				function onCurrentTextChanged(){
					if (cb.currentText != comboBoxGqlElementView.currentText){
						cb.currentText = comboBoxGqlElementView.currentText;
					}
				}
			}
		}
	}

	Component {
		id: errorComp;

		Text {
			text: comboBoxGqlElementView.errorText;
			color: Style.errorTextColor;
			font.family: Style.fontFamily;
			font.pixelSize: Style.fontSizeM;
		}
	}
}
