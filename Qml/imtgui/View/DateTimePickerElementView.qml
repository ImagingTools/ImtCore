import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

ElementView {
	id: expirationEditor;
	
	controlComp: datePickerComp;
	
	property bool readOnly: false;
	property bool timeEdit: false;
	property DateInput datePicker: null;
	
	signal editingFinished();
	
	onReadOnlyChanged: {
		if (datePicker){
			datePicker.readOnly = readOnly;
		}
	}
	
	function setDateAsString(dateStr){
		if (datePicker){
			datePicker.setDateAsString(dateStr);
		}
	}
	
	function getDateAsString(){
		if (datePicker){
			return datePicker.getDateAsString();
		}
		
		return "";
	}
	
	function setDate(date){
		if (datePicker){
			datePicker.setDate(date);
		}
	}
	
	Component {
		id: datePickerComp;
		
		DateInput {
			width: 200;
			hasTitle: false

			Component.onCompleted: {
				expirationEditor.datePicker = this;
			}
			
			onDateChanged: {
				expirationEditor.editingFinished();
			}
		}
	}
}


