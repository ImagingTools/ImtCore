import QtQuick
import imtcontrols 1.0

BaseClass {
	id: textFilter
	readonly property string __typename: 'TextFilter'
	property string m_text: ''
	property var m_fieldIds: []

	Component.onCompleted: {
		this._internal.removed = ["m_text","m_fieldIds"]
	}

	function hasText(){
		return (m_text !== undefined && m_text !== null)
	}

	function hasFieldIds(){
		return (m_fieldIds !== undefined && m_fieldIds !== null)
	}

	function emplaceFieldIds(typename){
		m_fieldIds = []
		this._internal.removeAt('m_fieldIds')
	}

	function removeText(){
		removeKey('m_text')
	}

	function removeFieldIds(){
		removeKey('m_fieldIds')
	}

	function getJSONKeyForProperty(propertyId){
		switch (propertyId){
			case 'm_text': return 'text'
			case 'm_fieldIds': return 'fieldIds'
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
		return Qt.createComponent('TextFilter.qml').createObject()
	}

	function getPropertyType(propertyId){
		switch (propertyId){
			case 'm_text': return 'string'
			case 'm_fieldIds': return 'array'
		}
	}
}

