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

	function updateModel(){
		container.model.setData("TypeId", typeComboBox.currentIndex, 0)
		container.model.setData("TypeValue", container.typesModel.getData("name", typeComboBox.currentIndex), 0)
		container.model.setData("Name", nameField.text, 0)
		container.model.setData("Latitude", latitudeField.text, 0)
		container.model.setData("Longitude", longitudeField.text, 0)
	}

	function updateGui(){
		typeComboBox.currentIndex = container.model.getData("TypeId", 0)
		nameField.text = container.model.getData("Name", 0)
		latitudeField.text = container.model.getData("Latitude", 0)
		longitudeField.text = container.model.getData("Longitude", 0)
	}

	ComboBoxElementView {
		id: typeComboBox;
		name: qsTr("Select type");
		width: parent.width

		onCurrentIndexChanged: {
			let oldIndex = container.model.getData("TypeId", 0)
			if (oldIndex !== typeComboBox.currentIndex){
				container.updateModel();
			}
		}
	}

	TextInputElementView{
		id: nameField;
		name: qsTr("Name");
		width: parent.width

		onEditingFinished: {
			let oldText = container.model.getData("Name", 0)
			if (oldText !== nameField.text){
				container.updateModel();
			}
		}
	}

	TextInputElementView{
		id: latitudeField;
		name: qsTr("Latitude");
		width: parent.width

		onEditingFinished: {
			let oldText = container.model.getData("Latitude", 0)
			if (oldText !== latitudeField.text){
				container.updateModel();
			}
		}
	}

	TextInputElementView{
		id: longitudeField;
		name: qsTr("Longitude");
		width: parent.width

		onEditingFinished: {
			let oldText = container.model.getData("Longitude", 0)
			if (oldText !== longitudeField.text){
				container.updateModel();
			}
		}
	}
}
