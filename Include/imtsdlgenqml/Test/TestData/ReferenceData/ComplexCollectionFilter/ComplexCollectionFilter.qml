import QtQuick
import imtcontrols 1.0

BaseClass {
	id: complexCollectionFilter
	readonly property string __typename: 'ComplexCollectionFilter'
	property BaseModel m_sortingInfo: null
	property GroupFilter m_fieldsFilter: null
	property TimeFilter m_timeFilter: null
	property TextFilter m_textFilter: null
	property var m_distinctFields: []

	Component.onCompleted: {
		this._internal.removed = ["m_sortingInfo","m_fieldsFilter","m_timeFilter","m_textFilter","m_distinctFields"]
	}

	function hasSortingInfo(){
		return (m_sortingInfo !== undefined && m_sortingInfo !== null)
	}

	function hasFieldsFilter(){
		return (m_fieldsFilter !== undefined && m_fieldsFilter !== null)
	}

	function hasTimeFilter(){
		return (m_timeFilter !== undefined && m_timeFilter !== null)
	}

	function hasTextFilter(){
		return (m_textFilter !== undefined && m_textFilter !== null)
	}

	function hasDistinctFields(){
		return (m_distinctFields !== undefined && m_distinctFields !== null)
	}

	function createSortingInfo(typename){
		m_sortingInfo = createComponent('m_sortingInfo', typename).createObject(this)
		m_sortingInfo.owner = this
		this._internal.removeAt('m_sortingInfo')
	}

	function createFieldsFilter(typename){
		m_fieldsFilter = createComponent('m_fieldsFilter', typename).createObject(this)
		m_fieldsFilter.owner = this
		this._internal.removeAt('m_fieldsFilter')
	}

	function createTimeFilter(typename){
		m_timeFilter = createComponent('m_timeFilter', typename).createObject(this)
		m_timeFilter.owner = this
		this._internal.removeAt('m_timeFilter')
	}

	function createTextFilter(typename){
		m_textFilter = createComponent('m_textFilter', typename).createObject(this)
		m_textFilter.owner = this
		this._internal.removeAt('m_textFilter')
	}

	function createDistinctFields(typename){
		m_distinctFields = []
		this._internal.removeAt('m_distinctFields')
	}

	function removeSortingInfo(){
		removeKey('m_sortingInfo')
	}

	function removeFieldsFilter(){
		removeKey('m_fieldsFilter')
	}

	function removeTimeFilter(){
		removeKey('m_timeFilter')
	}

	function removeTextFilter(){
		removeKey('m_textFilter')
	}

	function removeDistinctFields(){
		removeKey('m_distinctFields')
	}

	function getJSONKeyForProperty(propertyId){
		switch (propertyId){
			case 'm_sortingInfo': return 'sortingInfo'
			case 'm_fieldsFilter': return 'fieldsFilter'
			case 'm_timeFilter': return 'timeFilter'
			case 'm_textFilter': return 'textFilter'
			case 'm_distinctFields': return 'distinctFields'
			case '__typename': return '__typename'
		}
	}

	function createElement(propertyId, typename){
		switch (propertyId){
			case 'm_sortingInfo': return Qt.createComponent('qrc:/qml/imtbaseComplexCollectionFilterSdl/FieldSortingInfo.qml')
		}
	}

	function createComponent(propertyId, typename){
		let retVal;
		switch (propertyId){
			case 'm_sortingInfo':{
				retVal = Qt.createComponent('qrc:/qml/imtcontrols/Base/BaseModel.qml')
				retVal.owner = complexCollectionFilter
				return retVal
			}
			case 'm_fieldsFilter': return Qt.createComponent('qrc:/qml/imtbaseComplexCollectionFilterSdl/GroupFilter.qml')
			case 'm_timeFilter': return Qt.createComponent('qrc:/qml/imtbaseComplexCollectionFilterSdl/TimeFilter.qml')
			case 'm_textFilter': return Qt.createComponent('qrc:/qml/imtbaseComplexCollectionFilterSdl/TextFilter.qml')
		}
	}

	function createMe(){
		return Qt.createComponent('ComplexCollectionFilter.qml').createObject()
	}

	function getPropertyType(propertyId){
		switch (propertyId){
			case 'm_sortingInfo': return 'BaseModel'
			case 'm_fieldsFilter': return 'GroupFilter'
			case 'm_timeFilter': return 'TimeFilter'
			case 'm_textFilter': return 'TextFilter'
			case 'm_distinctFields': return 'array'
		}
	}
}

