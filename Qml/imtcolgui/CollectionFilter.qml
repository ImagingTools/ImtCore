import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtbaseComplexCollectionFilterSdl 1.0


/*!
	\qmltype ComplexCollectionFilter
	\inqmlmodule imtbaseComplexCollectionFilterSdl
	\brief Provides advanced filtering and sorting logic for collections.
	
	This component allows the configuration of complex filters and sorting rules using field filters,
	group filters, and time ranges. It supports logical operations (AND, OR, NOT),
	value types (integer, number, string, bool), and filter operations (equal, contains, etc.).
*/

ComplexCollectionFilter{
	id: root

	/*!
		\qmlproperty Component fieldSortingInfoFact
		Factory for creating sorting field info objects.
	*/
	property Component fieldSortingInfoFact: Component{ FieldSortingInfo{} }

	/*!
		\qmlproperty Component groupFilterFact
		Factory for creating group filter objects.
	*/
	property Component groupFilterFact: Component{ GroupFilter{} }

	/*!
		\qmlproperty Component fieldFilterFact
		Factory for creating field filter objects.
	*/
	property Component fieldFilterFact: Component{ FieldFilter{} }

	/*!
		\qmlproperty Component baseModelFact
		Factory for creating base model objects.
	*/
	property Component baseModelFact: Component{ BaseModel{} }

	/*!
		\qmlproperty var textFilteringInfoIds
		Array of field IDs used for text-based filtering.
	*/
	property var textFilteringInfoIds: []

	/*!
		\qmlproperty var logicalOperation
		Enumeration of logical operations: AND and OR.
	*/
	property var logicalOperation: ({
										AND: "And",
										OR: "Or"
									})

	/*!
		\qmlproperty var filterOperation
		Enumeration of supported filter operations.
	*/
	property var filterOperation: ({
									   NOT: "Not",
									   EQUAL: "Equal",
									   LESS: "Less",
									   GREATER: "Greater",
									   CONTAINS: "Contains"
								   })

	/*!
		\qmlproperty var valueType
		Enumeration of value types used in filters.
	*/
	property var valueType: ({
								 INTEGER: "Integer",
								 NUMBER: "Number",
								 STRING: "String",
								 BOOL: "Bool"
							 })

	/*!
		\qmlsignal filterChanged
		Emitted when the filter is changed.
	*/
	signal filterChanged()

	/*!
		\qmlsignal cleared
		Emitted when the filter is cleared.
	*/
	signal cleared()

	/*!
		\qmlmethod object createBaseModel()
		Creates and returns a new base model object.
	*/
	function createBaseModel(){
		return baseModelFact.createObject(root)
	}

	/*!
		\qmlmethod string getTextFilter()
		Returns the current text filter. (Placeholder, always returns empty string.)
	*/
	function getTextFilter(){
		if (!hasTextFilter()){
			return ""
		}

		return m_textFilter.text
	}

	/*!
		\qmlmethod void setTextFilter(string filter)
		Sets a new text filter using OR operation on the fields defined in \c textFilteringInfoIds.
	*/
	function setTextFilter(filter){
		if (filter === ""){
			if (hasTextFilter()){
				m_textFilter.destroy()
				m_textFilter = null
			}
		}
		else{
			if (!hasTextFilter()){
				createTextFilter()
			}

			m_textFilter.m_text = filter
			m_textFilter.m_fieldIds = textFilteringInfoIds
		}
	}

	/*!
		\qmlmethod void setSortingInfo(string infoId, string sortOrder)
		Sets sorting info by field ID and sort order.
	*/
	function setSortingInfo(infoId, sortOrder){
		clearSortFilter()

		let obj = fieldSortingInfoFact.createObject(m_sortingInfo)
		obj.m_fieldId = infoId
		obj.m_sortingOrder = sortOrder
		if (!hasSortingInfo()){
			createSortingInfo()
		}

		m_sortingInfo.addElement(obj)
	}

	/*!
		\qmlmethod object getSortingInfo()
		Returns the first sorting info item or null if not set.
	*/
	function getSortingInfo(){
		if (m_sortingInfo.length > 0){
			return m_sortingInfo.get(0).item
		}
		return null
	}

	/*!
		\qmlmethod void setFilteringInfoIds(var filteringInfoIds)
		Sets the array of text filtering field IDs.
	*/
	function setFilteringInfoIds(filteringInfoIds){
		textFilteringInfoIds = filteringInfoIds
	}

	/*!
		\qmlmethod object createGroupFilter(string operation, object fields, object groups)
		Creates a group filter using given logical operation, fields, and subgroups.
	*/
	function createGroupFilter(operation, fields, groups){
		let groupObj = groupFilterFact.createObject(m_fieldsFilter)

		if (fields){
			groupObj.m_fieldFilters = fields
		}
		if (groups){
			groupObj.m_groupFilters = groups
		}
		groupObj.m_logicalOperation = operation || logicalOperation.AND

		return groupObj
	}

	/*!
		\qmlmethod void addGroupFilter(object group)
		Adds a group filter to the fields filter.
	*/
	function addGroupFilter(group){
		if (!hasFieldsFilter()){
			createFieldsFilter()
			m_fieldsFilter.m_logicalOperation = logicalOperation.AND
		}
		if (!m_fieldsFilter.hasGroupFilters()){
			m_fieldsFilter.createGroupFilters()
		}
		if (!m_fieldsFilter.hasLogicalOperation()){
			m_fieldsFilter.m_logicalOperation = logicalOperation.AND
		}

		m_fieldsFilter.m_groupFilters.addElement(group)
	}

	/*!
		\qmlmethod object createFieldFilter(string fieldId, variant value, string valueType, var operations)
		Creates and returns a new field filter.
	*/
	function createFieldFilter(fieldId, value, valueType, operations){
		let fieldObj = fieldFilterFact.createObject(m_fieldsFilter)
		fieldObj.m_fieldId = fieldId
		fieldObj.m_filterValue = value
		fieldObj.m_filterValueType = valueType
		fieldObj.m_filterOperations = operations
		return fieldObj
	}

	/*!
		\qmlmethod void addFieldFilter(object field)
		Adds a field filter to the fields filter.
	*/
	function addFieldFilter(field){
		if (!hasFieldsFilter()){
			createFieldsFilter()
			m_fieldsFilter.m_logicalOperation = logicalOperation.AND
		}

		if (!m_fieldsFilter.hasFieldFilters()){
			m_fieldsFilter.createFieldFilters()
		}

		m_fieldsFilter.m_fieldFilters.addElement(field)
	}

	/*!
		\qmlmethod bool removeFieldFilter(object field)
		Removes the given field filter. Returns true if found and removed.
	*/
	function removeFieldFilter(field){
		if (!hasFieldsFilter()){
			return false
		}
		if (!m_fieldsFilter.hasFieldFilters()){
			return false
		}

		for (let i = 0; i < m_fieldsFilter.m_fieldFilters.count; i++){
			let item = m_fieldsFilter.m_fieldFilters.get(i).item
			if (item === field){
				m_fieldsFilter.m_fieldFilters.remove(i)
				return true
			}
		}
		return false
	}

	/*!
		\qmlmethod bool removeFilterByFieldId(string field)
		Removes the given field filter by fieldId. Returns true if found and removed.
		Does not delete items located inside groups!!!
	*/
	function removeFilterByFieldId(fieldId){
		if (!hasFieldsFilter()){
			return false
		}

		if (!m_fieldsFilter.hasFieldFilters()){
			return false
		}

		let retVal = false
		for (let i = m_fieldsFilter.m_fieldFilters.count - 1; i >= 0; i--){
			let item = m_fieldsFilter.m_fieldFilters.get(i).item
			if (item.m_fieldId === fieldId){
				m_fieldsFilter.m_fieldFilters.remove(i)
				retVal = true
			}
		}

		return retVal
	}

	/*!
		\qmlmethod bool removeGroupFilter(object group)
		Removes the given group filter. Returns true if found and removed.
	*/
	function removeGroupFilter(group){
		if (!hasFieldsFilter()){
			return false
		}
		if (!m_fieldsFilter.hasGroupFilters()){
			return false
		}
		for (let i = 0; i < m_fieldsFilter.m_groupFilters.count; i++){
			let item = m_fieldsFilter.m_groupFilters.get(i).item
			if (item === group){
				m_fieldsFilter.m_groupFilters.remove(i)
				return true
			}
		}
		return false
	}

	/*!
		\qmlmethod void setTimeFilter(object timeFilter)
		Applies a given time filter by copying it into the internal time filter.
	*/
	function setTimeFilter(timeFilter){
		clearTimeFilter()
		if (timeFilter){
			if (!hasTimeFilter()){
				createTimeFilter()
			}

			m_timeFilter.copyFrom(timeFilter)
		}
	}

	/*!
		\qmlmethod void clearAllFilters()
		Clears all filters: sorting, fields, time, and distinct filters.
	*/
	function clearAllFilters(){
		clearSortFilter()
		clearFieldsFilter()
		clearTimeFilter()
		clearDistinctFilter()

		cleared()
	}

	/*!
		\qmlmethod void clearSortFilter()
		Clears all sorting filters.
	*/
	function clearSortFilter(){
		if (hasSortingInfo()){
			m_sortingInfo.clear()
		}
	}

	/*!
		\qmlmethod void clearFieldsFilter()
		Clears all field and group filters.
	*/
	function clearFieldsFilter(){
		if (hasFieldsFilter()){
			if (m_fieldsFilter.hasFieldFilters()){
				m_fieldsFilter.m_fieldFilters.clear()
			}
			if (m_fieldsFilter.hasGroupFilters()){
				m_fieldsFilter.m_groupFilters.clear()
			}
			removeFieldsFilter()
		}
	}

	/*!
		\qmlmethod void clearTimeFilter()
		Clears the time filter parameters.
	*/
	function clearTimeFilter(){
		if (hasTimeFilter()){
			removeTimeFilter()
		}
	}

	/*!
		\qmlmethod void clearDistinctFilter()
		Clears the distinct fields array.
	*/
	function clearDistinctFilter(){
		m_distinctFields = []
	}

	/*!
		\qmlmethod bool timeFilterIsEmpty()
		Returns true if the time filter has no meaningful values.
	*/
	function timeFilterIsEmpty(){
		if (hasTimeFilter()){
			let isEmpty = true
			if (m_timeFilter.hasTimeRange()){
				isEmpty = isEmpty && m_timeFilter.m_timeRange.m_begin === "" && m_timeFilter.m_timeRange.m_end === ""
			}

			return isEmpty && m_timeFilter.m_timeUnit === "" &&
					m_timeFilter.m_interpretationMode === "" &&
					m_timeFilter.m_unitMultiplier === 0
		}

		return true;
	}

	/*!
		\qmlmethod bool textFilterIsEmpty()
		Returns true if the text filter has no meaningful values.
	*/
	function textFilterIsEmpty(){
		if (hasTextFilter()){
			return m_textFilter.m_text === ""
		}

		return true
	}

	/*!
		\qmlmethod bool fieldsFilterIsEmpty()
		Returns true if there are no field or group filters set.
	*/
	function fieldsFilterIsEmpty(){
		if (hasFieldsFilter()){
			let isEmpty = true
			if (m_fieldsFilter.hasFieldFilters()){
				isEmpty = m_fieldsFilter.m_fieldFilters.count === 0
			}

			if (m_fieldsFilter.hasGroupFilters()){
				isEmpty = isEmpty && m_fieldsFilter.m_groupFilters.count === 0
			}

			return isEmpty
		}

		return true
	}

	/*!
		\qmlmethod bool isEmpty()
		Returns true if all filters (time and field/group) are empty.
	*/
	function isEmpty(){
		return timeFilterIsEmpty() && fieldsFilterIsEmpty() && textFilterIsEmpty()
	}
}
