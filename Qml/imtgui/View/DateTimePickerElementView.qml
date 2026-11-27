import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

/**
	A date typed as dd.MM.yyyy or picked in the calendar.
	An optional date (canShowCurrentDate false) starts empty and reads as empty when its text is cleared.
*/
ElementView {
	id: expirationEditor;
	
	controlComp: datePickerComp;
	
	property bool readOnly: false;
	property bool timeEdit: false;
	// False for an optional date: no date is shown until one is set
	property bool canShowCurrentDate: true;
	property string placeHolderText: qsTr("dd.mm.yyyy");
	property DateInput datePicker: null;

	// A date set before the input exists
	property string __pendingDate: "";
	
	signal editingFinished();
	
	onReadOnlyChanged: {
		if (datePicker){
			datePicker.readOnly = readOnly;
		}
	}
	
	// "dd.MM.yyyy"; an empty text clears the date
	function setDateAsString(dateStr){
		if (!datePicker){
			__pendingDate = dateStr;
			return;
		}

		if (dateStr === ""){
			datePicker.inputItem.text = "";
			datePicker.dateStr = "";
			return;
		}

		datePicker.setDateAsString(dateStr);
	}
	
	// "dd.MM.yyyy", empty when no date is typed
	function getDateAsString(){
		if (!datePicker){
			return __pendingDate;
		}

		if (datePicker.inputItem.text.trim() === ""){
			return "";
		}

		return datePicker.getDateAsString();
	}
	
	function setDate(date){
		if (datePicker){
			datePicker.setDate(date);
		}
	}
	
	Component {
		id: datePickerComp;
		
		DateInput {
			width: expirationEditor.contentWidth > 0 ? Math.min(200, expirationEditor.contentWidth) : 200;
			hasTitle: false
			readOnly: expirationEditor.readOnly
			canShowCurrentDate: expirationEditor.canShowCurrentDate
			placeHolderText: expirationEditor.placeHolderText

			Component.onCompleted: {
				expirationEditor.datePicker = this;
				if (expirationEditor.__pendingDate !== ""){
					expirationEditor.setDateAsString(expirationEditor.__pendingDate);
				}
			}
			
			onDateChanged: {
				expirationEditor.editingFinished();
			}
		}
	}
}
