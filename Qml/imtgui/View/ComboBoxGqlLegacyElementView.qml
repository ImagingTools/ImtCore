import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtguigql 1.0
import imtcontrols 1.0

ElementView {
	id: comboBoxGqlLegacyElementView;
	bottomComp: isSelectionRequired && currentIndex == -1 ? errorComp : undefined;

	property bool isSelectionRequired: false;
	property string errorText: qsTr("Please select an item");

	property int itemHeight: Style.controlHeightL;
	property int elementHeight: Style.controlHeightL;
	property string placeHolderText: "";
	property string commandId;
	property string valueName: "name";
	property int currentIndex: -1;
	property string currentText: "";
	property bool canClose: false;
	property bool isAddressSearch: false;
	property bool hasSearchProperty: false;
	property bool canReturnModel: false;
	property string gettedParam: "name";
	property string filterId: "name";
	property string propertyId: "";
	property SearchCompLegacy cbRef: null;

	signal finished(string itemId);

	function setupComboBox(cbRef){
		if (!cbRef){
			return;
		}

		cbRef.itemHeight = comboBoxGqlLegacyElementView.itemHeight;
		cbRef.elementHeight = comboBoxGqlLegacyElementView.elementHeight;
		cbRef.placeHolderText = comboBoxGqlLegacyElementView.placeHolderText;
		cbRef.commandId = comboBoxGqlLegacyElementView.commandId;
		cbRef.valueName = comboBoxGqlLegacyElementView.valueName;
		cbRef.currentIndex = comboBoxGqlLegacyElementView.currentIndex;
		cbRef.currentText = comboBoxGqlLegacyElementView.currentText;
		cbRef.canClose = comboBoxGqlLegacyElementView.canClose;
		cbRef.isAddressSearch = comboBoxGqlLegacyElementView.isAddressSearch;
		cbRef.hasSearchProperty = comboBoxGqlLegacyElementView.hasSearchProperty;
		cbRef.canReturnModel = comboBoxGqlLegacyElementView.canReturnModel;
		cbRef.gettedParam = comboBoxGqlLegacyElementView.gettedParam;
		cbRef.filterId = comboBoxGqlLegacyElementView.filterId;
		cbRef.propertyId = comboBoxGqlLegacyElementView.propertyId;
		cbRef.setGettedParamsModel(cbRef.gettedParam);
		cbRef.setFilterIdsModel(cbRef.filterId);
		if(cbRef.hasSearchProperty){
			cbRef.setPropertiesModel(cbRef.propertyId, "");
		}

		comboBoxGqlLegacyElementView.cbRef = cbRef;
	}

	Connections {
		id: cbConn;
		target: comboBoxGqlLegacyElementView.cbRef;

		function onCurrentIndexChanged(){
			if (!comboBoxGqlLegacyElementView.cbRef){
				return;
			}

			if (comboBoxGqlLegacyElementView.currentIndex != comboBoxGqlLegacyElementView.cbRef.currentIndex){
				comboBoxGqlLegacyElementView.currentIndex = comboBoxGqlLegacyElementView.cbRef.currentIndex;
			}
		}

		function onCurrentTextChanged(){
			if (!comboBoxGqlLegacyElementView.cbRef){
				return;
			}

			if (comboBoxGqlLegacyElementView.currentText != comboBoxGqlLegacyElementView.cbRef.currentText){
				comboBoxGqlLegacyElementView.currentText = comboBoxGqlLegacyElementView.cbRef.currentText;
			}
		}
	}

	controlComp: cbComp;

	Component {
		id: cbComp;

		SearchCompLegacy{
			id: cb;
			color: Style.backgroundColor;

			width: comboBoxGqlLegacyElementView.controlWidth;
			height: comboBoxGqlLegacyElementView.controlHeight;

			Component.onCompleted: {
				comboBoxGqlLegacyElementView.setupComboBox(cb);
			}

			onAccepted: {
				if(cb.returnModel.getItemsCount()){
					let id = cb.returnModel.getData("id");
					comboBoxGqlLegacyElementView.finished(id)
				}
			}

			Connections {
				target: comboBoxGqlLegacyElementView;

				function onCurrentIndexChanged(){
					if (cb.currentIndex != comboBoxGqlLegacyElementView.currentIndex){
						cb.currentIndex = comboBoxGqlLegacyElementView.currentIndex;
					}
				}

				function onCurrentTextChanged(){
					if (cb.currentText != comboBoxGqlLegacyElementView.currentText){
						cb.currentText = comboBoxGqlLegacyElementView.currentText;
					}
				}
			}
		}
	}

	Component {
		id: errorComp;

		Text {
			text: comboBoxGqlLegacyElementView.errorText;
			color: Style.errorTextColor;
			font.family: Style.fontFamily;
			font.pixelSize: Style.fontSizeM;
		}
	}
}
