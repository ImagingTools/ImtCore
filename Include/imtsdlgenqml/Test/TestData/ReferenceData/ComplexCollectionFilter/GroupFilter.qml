import QtQuick
import imtcontrols 1.0

BaseClass {
	id: groupFilter
	readonly property string __typename: 'GroupFilter'
	property BaseModel m_fieldFilters: null
	property BaseModel m_groupFilters: null
	property string m_logicalOperation: ''

	Component.onCompleted: {
		this._internal.removed = ["m_fieldFilters","m_groupFilters","m_logicalOperation"]
	}

	function hasFieldFilters(){
		return (m_fieldFilters !== undefined && m_fieldFilters !== null)
	}

	function hasGroupFilters(){
		return (m_groupFilters !== undefined && m_groupFilters !== null)
	}

	function hasLogicalOperation(){
		return (m_logicalOperation !== undefined && m_logicalOperation !== null)
	}

	function createFieldFilters(typename){
		m_fieldFilters = createComponent('m_fieldFilters', typename).createObject(this)
		m_fieldFilters.owner = this
		this._internal.removeAt('m_fieldFilters')
	}

	function createGroupFilters(typename){
		m_groupFilters = createComponent('m_groupFilters', typename).createObject(this)
		m_groupFilters.owner = this
		this._internal.removeAt('m_groupFilters')
	}

	function removeFieldFilters(){
		removeKey('m_fieldFilters')
	}

	function removeGroupFilters(){
		removeKey('m_groupFilters')
	}

	function removeLogicalOperation(){
		removeKey('m_logicalOperation')
	}

	function getJSONKeyForProperty(propertyId){
		switch (propertyId){
			case 'm_fieldFilters': return 'fieldFilters'
			case 'm_groupFilters': return 'groupFilters'
			case 'm_logicalOperation': return 'logicalOperation'
			case '__typename': return '__typename'
		}
	}

	function createElement(propertyId, typename){
		switch (propertyId){
			case 'm_fieldFilters': {
				switch (typename){
					case 'FieldFilter': return Qt.createComponent('qrc:/qml/imtbaseComplexCollectionFilterSdl/FieldFilter.qml')
					case 'ArrayFieldFilter': return Qt.createComponent('qrc:/qml/imtbaseComplexCollectionFilterSdl/ArrayFieldFilter.qml')
				}
			}
			case 'm_groupFilters': return Qt.createComponent('qrc:/qml/imtbaseComplexCollectionFilterSdl/GroupFilter.qml')
		}
	}

	function createComponent(propertyId, typename){
		let retVal;
		switch (propertyId){
			case 'm_fieldFilters':{
				retVal = Qt.createComponent('qrc:/qml/imtcontrols/Base/BaseModel.qml')
				retVal.owner = groupFilter
				return retVal
			}
			case 'm_groupFilters':{
				retVal = Qt.createComponent('qrc:/qml/imtcontrols/Base/BaseModel.qml')
				retVal.owner = groupFilter
				return retVal
			}
		}
	}

	function createMe(){
		return Qt.createComponent('GroupFilter.qml').createObject()
	}

	function getPropertyType(propertyId){
		switch (propertyId){
			case 'm_fieldFilters': return 'BaseModel'
			case 'm_groupFilters': return 'BaseModel'
			case 'm_logicalOperation': return 'string'
		}
	}
}

