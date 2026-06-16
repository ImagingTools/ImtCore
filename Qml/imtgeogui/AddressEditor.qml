import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0


Column {
	id: container;
	spacing: Style.marginXL;
	property var model;

	property var addressTypeNames: [qsTr("Country"), qsTr("Region"), qsTr("Area"),
		qsTr("City"), qsTr("Settlement"), qsTr("District"),
		qsTr("Street"), qsTr("Building"), qsTr("Apartment"),
		qsTr("Section"), qsTr("SNT"), qsTr("Alley"), qsTr("Other")]
	property TreeItemModel typesModel: TreeItemModel{}

	Component.onCompleted: {
		for (var i = 0; i < container.addressTypeNames.length; i++){
			container.typesModel.insertNewItem();
			container.typesModel.setData("id", String(i), i);
			container.typesModel.setData("name", container.addressTypeNames[i], i);//
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
				container.model.setData("typeValue", container.typesModel.getData("name", typeComboBox.currentIndex), 0)
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

		textInputValidator: DoubleValidator{}

		onTextChanged: {
			let newText = nameField.text.trim().replace(',','.')
			let oldText = container.model.getData("latitude", 0)
			if (oldText !== newText){
				container.model.setData("latitude", latitudeField.text, 0)
			}
		}
	}

	TextInputElementView{
		id: longitudeField;
		name: qsTr("Longitude");
		width: parent.width

		textInputValidator: DoubleValidator{}

		onTextChanged: {
			let newText = nameField.text.trim().replace(',','.')
			let oldText = container.model.getData("longitude", 0)
			if (oldText !== newText){
				container.model.setData("longitude", longitudeField.text, 0)
			}
		}
	}
}
