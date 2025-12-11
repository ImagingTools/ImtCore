import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtgui 1.0
import imtbaseComplexCollectionFilterSdl 1.0

FilterDelegateBase {
	id: timeDelegateFilter
	name: qsTr("Date")

	property bool canTimeRangeEdit: true
	property bool showFilterDetails: false

	property TimeFilter timeFilter: TimeFilter{}

	signal accepted()
	signal cleared()

	onIsActiveChanged: {
		if (!filterMenu){
			return
		}

		if (isActive){
			filterMenu.setFilterIsEnabled(filterId, true)
		}
		else{
			filterMenu.setFilterIsEnabled(filterId, false)
		}
	}

	onClearFilter:{
		timeDelegateFilter.mainButtonText = name
		timeDelegateFilter.isActive = false
		timeFilter.removeTimeRange()
		timeFilter.removeTimeUnit()
		timeFilter.removeInterpretationMode()
		timeFilter.removeUnitMultiplier()

		timeDelegateFilter.cleared()

		if (!beQuiet && filterMenu){
			filterMenu.filterChanged()
		}
	}

	onOpenFilter:{
		var point = mapToItem(null, x, y + height);
		ModalDialogManager.openDialog(timeFilterParamComp,{"x": point.x, "y": point.y, "timeFilter": timeFilter});
	}

	function setTimeUnit(mode, unit, beQuiet){
		if (timeFilter.hasTimeRange()){
			timeFilter.removeTimeRange()
		}
	
		timeFilter.m_interpretationMode = mode
		timeFilter.m_timeUnit = unit
		_internal.activateFilter()
		accepted()

		if (!beQuiet && filterMenu){
			filterMenu.filterChanged()
		}
	}

	function setTimeRange(begin, end, beQuiet){
		timeFilter.m_interpretationMode = ""
		timeFilter.m_timeUnit = ""

		if (timeFilter.hasInterpretationMode()){
			timeFilter.removeInterpretationMode()
		}

		if (timeFilter.hasTimeUnit()){
			timeFilter.removeTimeUnit()
		}

		if (!timeFilter.hasTimeRange()){
			timeFilter.createTimeRange()
		}

		timeFilter.m_timeRange.m_begin = begin
		timeFilter.m_timeRange.m_end = end

		_internal.activateFilter()

		accepted()

		if (!beQuiet && filterMenu){
			filterMenu.filterChanged()
		}
	}

	QtObject{
		id: _internal

		function activateFilter(){
			timeDelegateFilter.isActive = true
		
			if (timeDelegateFilter.timeFilter.hasTimeRange()){
				let begin = new Date(timeDelegateFilter.timeFilter.m_timeRange.m_begin)
				let end = new Date(timeDelegateFilter.timeFilter.m_timeRange.m_end)
				timeDelegateFilter.mainButtonText = _internal.formatShortMonth(begin) + " - " + _internal.formatShortMonth(end)
				return
			}
		
			if (timeDelegateFilter.timeFilter.hasTimeUnit() && timeDelegateFilter.timeFilter.hasInterpretationMode()){
				let unit = timeDelegateFilter.timeFilter.m_timeUnit
				let mode = timeDelegateFilter.timeFilter.m_interpretationMode
		
				let baseText = ""
				if (unit === "Week" && mode === "For")
					baseText = qsTr("Last 7 Days")
				else if (unit === "Month" && mode === "Current")
					baseText = qsTr("This Month")
				else if (unit === "Month" && mode === "Last")
					baseText = qsTr("Last Month")
				else if (unit === "Year" && mode === "Current")
					baseText = qsTr("This Year")
				else if (unit === "Year" && mode === "Last")
					baseText = qsTr("Last Year")
				else
					baseText = qsTr("Custom")
		
				let range = _internal.getDateRangeForPreset(unit, mode)
				if (!timeDelegateFilter.showFilterDetails || !range){
					timeDelegateFilter.mainButtonText = baseText
					return
				}
		
				if (unit === "Year"){
					timeDelegateFilter.mainButtonText = baseText + " (" + range.begin.getFullYear() + ")"
				}
				else{
					let startStr = _internal.formatShortMonth(range.begin)
					let endStr = _internal.formatShortMonth(range.end)
		
					timeDelegateFilter.mainButtonText = baseText + " (" + startStr + " - " + endStr + ")"
				}
			}
		}

		function formatShortMonth(date){
			let months = ["Jan", "Feb", "Mar", "Apr", "May", "Jun",
							"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"]
			let day = String(date.getDate()).padStart(2, '0')
			let month = months[date.getMonth()]
			return day + " " + month
		}

		function startOfMonth(date){
			return new Date(date.getFullYear(), date.getMonth(), 1)
		}
	
		function endOfMonth(date){
			return new Date(date.getFullYear(), date.getMonth() + 1, 0)
		}
	
		function startOfYear(date){
			return new Date(date.getFullYear(), 0, 1)
		}
	
		function endOfYear(date){
			return new Date(date.getFullYear(), 11, 31)
		}
	
		function getDateRangeForPreset(unit, mode){
			let now = new Date()
	
			if (unit === "Week" && mode === "For"){
				let end = new Date(now)
				let begin = new Date(now)
				begin.setDate(begin.getDate() - 7)
				return{ begin: begin, end: end }
			}
	
			if (unit === "Month" && mode === "Current"){
				return{
					begin: startOfMonth(now),
					end: endOfMonth(now)
				}
			}
	
			if (unit === "Month" && mode === "Last"){
				let lastMonth = new Date(now.getFullYear(), now.getMonth() - 1, 1)
				return{
					begin: startOfMonth(lastMonth),
					end: endOfMonth(lastMonth)
				}
			}
	
			if (unit === "Year" && mode === "Current"){
				return{
					begin: startOfYear(now),
					end: endOfYear(now)
				}
			}
	
			if (unit === "Year" && mode === "Last"){
				let lastYear = now.getFullYear() - 1
				return{
					begin: new Date(lastYear, 0, 1),
					end: new Date(lastYear, 11, 31)
				}
			}
	
			return null
		}
	}

	Component{
		id: timeFilterParamComp;
		
		TimeFilterParamView{
			canTimeRangeEdit: timeDelegateFilter.canTimeRangeEdit

			onTimeRangeChanged:{
				timeDelegateFilter.setTimeRange(begin, end, false)
				ModalDialogManager.closeByComp(timeFilterParamComp)
			}

			onTimeUnitChanged:{
				timeDelegateFilter.setTimeUnit(mode, unit, false)
				ModalDialogManager.closeByComp(timeFilterParamComp)
			}

			onCancelled:{
				timeDelegateFilter.clearFilter()
			}
		}
	}
}


