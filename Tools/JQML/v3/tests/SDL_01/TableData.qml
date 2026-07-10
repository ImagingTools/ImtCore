import QtQuick
import imtcontrols 1.0

BaseClass {
	id: tableData
	property BaseModel m_rows: null

	function getJSONKeyForProperty(propertyId){
		switch (propertyId){
			case 'm_rows': return 'rows'
		}
	}

	function createElement(propertyId, typename){
		switch (propertyId){
			case 'm_rows': return Qt.createComponent('RowData.qml')
		}
	}

	function createComponent(propertyId, typename){
		let retVal;
		switch (propertyId){
			case 'm_rows':{
				retVal = Qt.createComponent('qrc:/qml/imtcontrols/Base/BaseModel.qml')
				retVal.owner = tableData
				return retVal
			}
		}
	}

	function getPropertyType(propertyId){
		switch (propertyId){
			case 'm_rows': return 'BaseModel'
		}
	}
}

