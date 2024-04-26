import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

DetailedPopupMenuDelegate {
    height: acceptable && comboBoxRef ? comboBoxRef.itemHeight: 0;

    visible: height > 0;

    text: displayText;

    highlighted: comboBoxRef && comboBoxRef.currentIndex === model.index
    selected: comboBoxRef && comboBoxRef.popup ? comboBoxRef.popup.selectedIndex === model.index : false;

    property string displayText: comboBoxRef ? model[comboBoxRef.nameId] : "";
    property string filter: comboBoxRef ? comboBoxRef.filter : "";

    property ComboBox comboBoxRef;

    onFilterChanged: {
        if (!comboBoxRef){
            return;
        }

        if (filter === ""){
            acceptable = true;
            return;
        }

        acceptable = false;

        let keys = comboBoxRef.model.GetKeys(model.index);
        for (let i = 0; i < comboBoxRef.filteringFields.length; i++){
            let id = comboBoxRef.filteringFields[i];
            if (keys.includes(id)){
                let value = model[id];
                if (value.toLowerCase().indexOf(filter.toLowerCase()) >= 0){
                    acceptable = true;
                    break;
                }
            }
        }
    }

    property bool acceptable: true;

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
