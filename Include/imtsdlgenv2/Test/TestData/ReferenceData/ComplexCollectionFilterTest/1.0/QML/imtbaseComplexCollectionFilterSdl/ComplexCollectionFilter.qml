import QtQuick
import imtcontrols 1.0

BaseClass {
	id: complexCollectionFilter
	readonly property string __typename: 'ComplexCollectionFilter'
	property BaseModel m_sortingInfo: BaseModel {owner: complexCollectionFilter}
	property var m_fieldsFilter
	property var m_timeFilter
	property BaseModel m_distinctFields: []

	function getJSONKeyForProperty(propertyId){
		switch (propertyId){
			case 'm_sortingInfo': return 'sortingInfo'
			case 'm_fieldsFilter': return 'fieldsFilter'
			case 'm_timeFilter': return 'timeFilter'
			case 'm_distinctFields': return 'distinctFields'
		}
	}

	function hasSortingInfo() {
		return (m_sortingInfo !== undefined && m_sortingInfo != null)
	}

	function hasFieldsFilter() {
		return (m_fieldsFilter !== undefined && m_fieldsFilter != null)
	}

	function hasTimeFilter() {
		return (m_timeFilter !== undefined && m_timeFilter != null)
	}


	function createSortingInfo(typename) {
		m_sortingInfo = createComponent('m_sortingInfo', typename)
	}

	function createFieldsFilter(typename) {
		m_fieldsFilter = createComponent('m_fieldsFilter', typename)
	}

	function createTimeFilter(typename) {
		m_timeFilter = createComponent('m_timeFilter', typename)
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

