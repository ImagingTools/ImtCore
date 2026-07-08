import QtQuick
import imtcontrols 1.0

BaseClass {
	id: rowData
	property BaseModel m_cells: null

	function getJSONKeyForProperty(propertyId){
		switch (propertyId){
			case 'm_cells': return 'cells'
		}
	}

	function createElement(propertyId, typename){
		switch (propertyId){
			case 'm_cells': return Qt.createComponent('CellData.qml')
		}
	}

	function createComponent(propertyId, typename){
		let retVal;
		switch (propertyId){
			case 'm_cells':{
				retVal = Qt.createComponent('qrc:/qml/imtcontrols/Base/BaseModel.qml')
				retVal.owner = rowData
				return retVal
			}
		}
	}

	function getPropertyType(propertyId){
		switch (propertyId){
			case 'm_cells': return 'BaseModel'
		}
	}
}

