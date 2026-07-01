import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0


Column {
	id: container;
	spacing: Style.marginXL;
	property var model;

	property bool isValid: nameField.text ? nameField.text.trim().length > 0 : false;

	property var addressTypeNames: [qsTr("Country"), qsTr("Region"), qsTr("Area"),
		qsTr("City"), qsTr("Settlement"), qsTr("District"),
		qsTr("Street"), qsTr("Building"), qsTr("Apartment"),
		qsTr("Section"), qsTr("SNT"), qsTr("Alley"), qsTr("Other")]
	property TreeItemModel typesModel: TreeItemModel{}

	Component.onCompleted: {
		for (let i = 0; i < container.addressTypeNames.length; i++){
			container.typesModel.insertNewItem();
			container.typesModel.setData("id", String(i), i);
			container.typesModel.setData("name", container.addressTypeNames[i], i);
		}

		typeComboBox.model = container.typesModel;
		updateGui()
	}

	function updateGui(){
		typeComboBox.currentIndex = container.model.getData("typeId", 0)
		nameField.text = container.model.getData("name", 0)
		latitudeField.text = container.model.getData("latitude", 0)
		longitudeField.text = container.model.getData("longitude", 0)
	}

	ComboBoxElementView {
		id: typeComboBox;
		name: qsTr("Select type");
		width: parent.width

		onCurrentIndexChanged: {
			let oldIndex = container.model.getData("typeId", 0)
			if (oldIndex !== typeComboBox.currentIndex){
				container.model.setData("typeId", typeComboBox.currentIndex, 0)
			}
		}
	}

	TextInputElementView{
		id: nameField;
		name: qsTr("Name");
		width: parent.width

		onTextChanged: {
			let oldText = container.model.getData("name", 0)
			if (oldText !== nameField.text){
				container.model.setData("name", nameField.text, 0)
			}
		}
	}

	TextInputElementView{
		id: latitudeField;
		name: qsTr("Latitude");
		width: parent.width

		textInputValidator: RegularExpressionValidator {
			regularExpression: /^-?\d*[.,]?\d*$/
		}

		onTextChanged: {
			let normalized = text.replace(',', '.')
			let value = parseFloat(normalized)
			if (!isNaN(value)) {
				container.model.setData("latitude", value, 0)
			}
		}
	}

	TextInputElementView{
		id: longitudeField;
		name: qsTr("Longitude");
		width: parent.width

		textInputValidator: RegularExpressionValidator {
			regularExpression: /^-?\d*[.,]?\d*$/
		}

		onTextChanged: {
			let normalized = text.replace(',', '.')
			let value = parseFloat(normalized)
			if (!isNaN(value)) {
				container.model.setData("longitude", value, 0)
			}
		}
	}
}
