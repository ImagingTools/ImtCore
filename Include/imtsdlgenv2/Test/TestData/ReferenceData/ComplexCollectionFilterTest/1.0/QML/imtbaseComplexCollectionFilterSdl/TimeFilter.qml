import QtQuick
import imtcontrols 1.0
import imtbaseImtBaseTypesSdl 1.0

BaseClass {
	id: timeFilter
	readonly property string __typename: 'TimeFilter'
	property var m_timeRange
	property var m_timeUnit
	property var m_interpretationMode
	property var m_unitMultiplier

	function hasTimeRange(){
		return (m_timeRange !== undefined && m_timeRange !== null)
	}

	function hasTimeUnit(){
		return (m_timeUnit !== undefined && m_timeUnit !== null)
	}

	function hasInterpretationMode(){
		return (m_interpretationMode !== undefined && m_interpretationMode !== null)
	}

	function hasUnitMultiplier(){
		return (m_unitMultiplier !== undefined && m_unitMultiplier !== null)
	}

	function createTimeRange(typename){
		m_timeRange = createComponent('m_timeRange', typename)
	}

	function getJSONKeyForProperty(propertyId){
		switch (propertyId){
			case 'm_timeRange': return 'timeRange'
			case 'm_timeUnit': return 'timeUnit'
			case 'm_interpretationMode': return 'interpretationMode'
			case 'm_unitMultiplier': return 'unitMultiplier'
		}
	}

	function createElement(propertyId, typename){
		switch (propertyId){
		}
	}

	function createComponent(propertyId, typename){
		switch (propertyId){
			case 'm_timeRange': return Qt.createComponent('qrc:/qml/imtbaseImtBaseTypesSdl/TimeRange.qml')
		}
	}

	function createMe(){
		return Qt.createComponent('TimeFilter.qml').createObject()
	}

	function getPropertyType(propertyId){
		switch (propertyId){
			case 'm_timeRange': return 'TimeRange'
			case 'm_timeUnit': return 'string'
			case 'm_interpretationMode': return 'string'
			case 'm_unitMultiplier': return 'int'
		}
	}
}

