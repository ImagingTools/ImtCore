import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

ItemDelegate {
	id: popupMenuDelegate;
	objectName: "PopupMenuDelegate"
	
	decorator: Style.popupItemDelegateDecorator;
	
	property bool selected: false;

	property bool isSeparator: String(model.item ? model.item.m_id : model.Id ? model.Id : model.id ? model.id : "") === "";
	
	// Reference to the control in which the delegate is declared
	property Item containerItem: null;
	property Item rootItem: null;
	property int contentLeftMargin: Style.marginM;

	enabled: (model.m_enabled !==undefined && model.m_enabled == false) ? false : true;
	
	onHeightChanged: {
		internal.checkSeparator();
	}
	
	onIsSeparatorChanged: {
		internal.checkSeparator();
	}
	
	QtObject {
		id: internal;
		
		function checkSeparator(){
			if (popupMenuDelegate.isSeparator){
				popupMenuDelegate.mouseArea.visible = false;
				popupMenuDelegate.height = 5;
			}
		}
	}
}
