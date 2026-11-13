import QtQuick
import imtcontrols 1.0

BaseClass {
	id: tableViewRow
	readonly property string __typename: 'TableViewRow'
	property string m_id: ""
	property bool m_inactive: false
	property var m_index: []
	property int m_measurementIndex: 0

	Component.onCompleted: {
		this._internal.removed = ["m_id","m_inactive","m_index","m_measurementIndex"]
	}

	function isArrayRequired(propertyId){
		switch (propertyId){
		}
		return false
	}

	function getJSONKeyForProperty(propertyId){
		switch (propertyId){
			case 'm_id': return 'id'
			case 'm_inactive': return 'inactive'
			case 'm_index': return 'index'
			case 'm_measurementIndex': return 'measurementIndex'
			case '__typename': return '__typename'
		}
	}

	function createElement(propertyId, typename){
		switch (propertyId){
		}
	}

	function createComponent(propertyId, typename){
		let retVal;
		switch (propertyId){
		}
	}

	function createMe(){
		return Qt.createComponent('TableViewRow.qml').createObject()
	}

	function getPropertyType(propertyId){
		switch (propertyId){
			case 'm_id': return 'string'
			case 'm_inactive': return 'bool'
			case 'm_index': return 'var'
			case 'm_measurementIndex': return 'int'
		}
	}
}
