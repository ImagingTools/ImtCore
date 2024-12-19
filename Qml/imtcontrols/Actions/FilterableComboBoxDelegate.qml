import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

DetailedPopupMenuDelegate {
	height: comboBoxRef ? comboBoxRef.itemHeight: 0;

    visible: height > 0;

    highlighted: comboBoxRef && comboBoxRef.currentIndex === model.index
    selected: comboBoxRef && comboBoxRef.popup ? comboBoxRef.popup.selectedIndex === model.index : false;

	property ComboBox comboBoxRef;

    onClicked: {
        if (comboBoxRef.popup){
            comboBoxRef.popup.finished(model.Id, model.index)
        }
    }

    onEntered: {
        if (comboBoxRef.popup){
            comboBoxRef.popup.selectedIndex = model.index;
        }
    }
}
