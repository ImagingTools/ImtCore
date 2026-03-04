import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtguigql 1.0
import imtcontrols 1.0

ElementView {
	id: comboBoxAddressElementView;
	bottomComp: isSelectionRequired && currentIndex == -1 ? errorComp : undefined;

	property bool isSelectionRequired: false;
	property string errorText: qsTr("Please select an item");

	property int itemHeight: Style.controlHeightL;
	property int shownItemsCount: 5;
	property string commandId;
	property string nameId: "fullAddress";
	property var fields: [];
	property var textFilteringInfoIds: [];
	property int currentIndex: -1;
	property string currentText: "";
	property string filterText: "";
	property SearchAddressComp cbRef: null;

	signal finished(string addressId, string address);

	function setupComboBox(cbRef){
		if (!cbRef){
			return;
		}

		cbRef.itemHeight = comboBoxAddressElementView.itemHeight;
		cbRef.shownItemsCount = comboBoxAddressElementView.shownItemsCount;
		cbRef.commandId = comboBoxAddressElementView.commandId;
		cbRef.nameId = comboBoxAddressElementView.nameId;
		cbRef.dataProvider.fields = comboBoxAddressElementView.fields;
		cbRef.dataProvider.textFilteringInfoIds = comboBoxAddressElementView.textFilteringInfoIds;
		cbRef.currentIndex = comboBoxAddressElementView.currentIndex;
		cbRef.currentText = comboBoxAddressElementView.currentText;
		cbRef.filterText = comboBoxAddressElementView.filterText;

		comboBoxAddressElementView.cbRef = cbRef;
	}

	Connections {
		id: cbConn;
		target: comboBoxAddressElementView.cbRef;

		function onCurrentIndexChanged(){
			if (!comboBoxAddressElementView.cbRef){
				return;
			}

			if (comboBoxAddressElementView.currentIndex != comboBoxAddressElementView.cbRef.currentIndex){
				comboBoxAddressElementView.currentIndex = comboBoxAddressElementView.cbRef.currentIndex;
			}
		}

		function onCurrentTextChanged(){
			if (!comboBoxAddressElementView.cbRef){
				return;
			}

			if (comboBoxAddressElementView.currentText != comboBoxAddressElementView.cbRef.currentText){
				comboBoxAddressElementView.currentText = comboBoxAddressElementView.cbRef.currentText;
			}
		}
	}

	controlComp: cbComp;

	Component {
		id: cbComp;

		SearchAddressComp{
			id: cb;

			width: comboBoxAddressElementView.controlWidth;
			height: comboBoxAddressElementView.controlHeight;
			placeHolderText: qsTr("Address");

			Component.onCompleted: {
				comboBoxAddressElementView.setupComboBox(cb);
			}

			onFinished: {
				if(index < 0){
					return
				}

				let id	 = model.getData("id", index);
				let address = model.getData("fullAddress", index);

				comboBoxAddressElementView.finished(id, address)
			}

			function setCustomFilters(inputParams){
				inputParams.InsertField("parentIds", cb.parentIds);
				inputParams.InsertField("listEverything", true);
			}

			Connections {
				target: comboBoxAddressElementView;

				function onCurrentIndexChanged(){
					if (cb.currentIndex != comboBoxAddressElementView.currentIndex){
						cb.currentIndex = comboBoxAddressElementView.currentIndex;
					}
				}

				function onCurrentTextChanged(){
					if (cb.currentText != comboBoxAddressElementView.currentText){
						cb.currentText = comboBoxAddressElementView.currentText;
					}
				}
			}
		}
	}

	Component {
		id: errorComp;

		Text {
			text: comboBoxAddressElementView.errorText;
			color: Style.errorTextColor;
			font.family: Style.fontFamily;
			font.pixelSize: Style.fontSizeM;
		}
	}
}
