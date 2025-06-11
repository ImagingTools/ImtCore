import QtQuick
import imtcontrols 1.0

BaseClass {
	id: groupFilter
	readonly property string __typename: 'GroupFilter'
	property BaseModel m_fieldFilters: BaseModel {owner: groupFilter}
	property BaseModel m_groupFilters: BaseModel {owner: groupFilter}
	property var m_logicalOperation: null

	function getJSONKeyForProperty(propertyId){
		switch (propertyId){
			case 'm_fieldFilters': return 'fieldFilters'
			case 'm_groupFilters': return 'groupFilters'
			case 'm_logicalOperation': return 'logicalOperation'
		}
	}

	function createComponent(propertyId, typename){
		switch (propertyId){
			case 'm_fieldFilters': return Qt.createComponent('qrc:/qml/imtbaseComplexCollectionFilterSdl/FieldFilter.qml')
			case 'm_groupFilters': return Qt.createComponent('qrc:/qml/imtbaseComplexCollectionFilterSdl/GroupFilter.qml')
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

