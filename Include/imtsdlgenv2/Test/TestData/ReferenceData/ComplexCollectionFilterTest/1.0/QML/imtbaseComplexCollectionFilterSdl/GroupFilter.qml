import QtQuick
import imtcontrols 1.0

BaseClass {
	id: groupFilter
	readonly property string __typename: 'GroupFilter'
	property var m_fieldFilters
	property var m_groupFilters
	property var m_logicalOperation

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
		m_fieldFilters = createComponent('m_fieldFilters', typename)
	}

	function createGroupFilters(typename){
		m_groupFilters = createComponent('m_groupFilters', typename)
	}

	function getJSONKeyForProperty(propertyId){
		switch (propertyId){
			case 'm_fieldFilters': return 'fieldFilters'
			case 'm_groupFilters': return 'groupFilters'
			case 'm_logicalOperation': return 'logicalOperation'
		}
	}

	function createElement(propertyId, typename){
		switch (propertyId){
			case 'm_fieldFilters': return Qt.createComponent('qrc:/qml/imtbaseComplexCollectionFilterSdl/FieldFilter.qml')
			case 'm_groupFilters': return Qt.createComponent('qrc:/qml/imtbaseComplexCollectionFilterSdl/GroupFilter.qml')
		}
	}

	function createComponent(propertyId, typename){
		switch (propertyId){
			case 'm_fieldFilters':{
				let retVal = Qt.createComponent('qrc:/qml/imtcontrols/Base/BaseModel.qml')
				retVal.owner = groupFilter

				return retVal
			}
			case 'm_groupFilters':{
				let retVal = Qt.createComponent('qrc:/qml/imtcontrols/Base/BaseModel.qml')
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
			case 'm_fieldFilters': return 'FieldFilter'
			case 'm_groupFilters': return 'GroupFilter'
			case 'm_logicalOperation': return 'string'
		}
	}
}

