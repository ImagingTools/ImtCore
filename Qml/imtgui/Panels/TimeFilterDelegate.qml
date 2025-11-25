import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtgui 1.0
import imtbaseComplexCollectionFilterSdl 1.0

FilterDelegateBase {
	id: timeDelegateFilter
	name: qsTr("Date")

	property bool canTimeRangeEdit: true

	signal accepted()

	property TimeFilter timeFilter: TimeFilter {
	}

	property TimeFilterParamView timeFilterParamView: null

	onClearFilter: {
		timeDelegateFilter.mainButtonText = name
		timeDelegateFilter.isActive = false
		timeFilter.removeTimeRange()
		timeFilter.removeTimeUnit()
		timeFilter.removeInterpretationMode()
		timeFilter.removeUnitMultiplier()
		
		if (collectionFilter){
			if (collectionFilter.hasTimeFilter()){
				collectionFilter.clearTimeFilter()
				collectionFilter.filterChanged()
			}
		}
	}
	
	onOpenFilter: {
		var point = mapToItem(null, x, y + height);
		ModalDialogManager.openDialog(timeFilterParamComp, {"x": point.x, "y": point.y, "timeFilter": timeFilter});
	}

	function setTimeFilter(timeFilter, name, beQuiet){
		if (timeDelegateFilter.collectionFilter){
			timeDelegateFilter.collectionFilter.setTimeFilter(timeFilter);

			timeDelegateFilter.isActive = true
			timeDelegateFilter.mainButtonText = name

			if (!beQuiet){
				timeDelegateFilter.collectionFilter.filterChanged()
			}
		}
	}
	
	Component {
		id: timeFilterParamComp;
		
		TimeFilterParamView {
			canTimeRangeEdit: timeDelegateFilter.canTimeRangeEdit
			Component.onCompleted: {
				timeDelegateFilter.timeFilterParamView = this
			}

			onAccepted: {
				timeDelegateFilter.setTimeFilter(timeFilter, name, false)
				ModalDialogManager.closeByComp(timeFilterParamComp);
				timeDelegateFilter.accepted()
			}
			
			onCancelled: {
				timeDelegateFilter.clearFilter()
			}
		}
	}
}


