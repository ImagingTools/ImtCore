import QtQuick
import imtcontrols 1.0

BaseClass {
	id: complexCollectionFilter
	readonly property string __typename: 'ComplexCollectionFilter'
	property BaseModel m_sortingInfo: BaseModel {owner: complexCollectionFilter}
	property var m_fieldsFilter: null
	property var m_timeFilter: null
	property BaseModel m_distinctFields: []

	function getJSONKeyForProperty(propertyId){
		switch (propertyId){
			case 'm_sortingInfo': return 'sortingInfo'
			case 'm_fieldsFilter': return 'fieldsFilter'
			case 'm_timeFilter': return 'timeFilter'
			case 'm_distinctFields': return 'distinctFields'
		}
	}

	function createComponent(propertyId, typename){
		switch (propertyId){
			case 'm_sortingInfo': return Qt.createComponent('qrc:/qml/imtbaseComplexCollectionFilterSdl/FieldSortingInfo.qml')
			case 'm_fieldsFilter': return Qt.createComponent('qrc:/qml/imtbaseComplexCollectionFilterSdl/GroupFilter.qml')
			case 'm_timeFilter': return Qt.createComponent('qrc:/qml/imtbaseComplexCollectionFilterSdl/TimeFilter.qml')
		}
	}

	function createMe(){
		return Qt.createComponent('ComplexCollectionFilter.qml').createObject()
	}

	function getPropertyType(propertyId){
		switch (propertyId){
			case 'm_sortingInfo': return 'FieldSortingInfo'
			case 'm_fieldsFilter': return 'GroupFilter'
			case 'm_timeFilter': return 'TimeFilter'
			case 'm_distinctFields': return 'string'
		}
	}
}

