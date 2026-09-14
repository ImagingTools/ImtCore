import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

ItemDelegate {
	id: popupMenuDelegate;
	// Test instrumentation: named for the item it stands for, so a menu row can be addressed by
	// what it does instead of by position. Separators keep the generic name. Inert.
	objectName: popupMenuDelegate.itemId === "" ? "PopupMenuDelegate" : "PopupItem_" + popupMenuDelegate.itemId
	readonly property string itemId: String(model.item ? model.item.m_id : model.Id ? model.Id : model.id ? model.id : "")
	
	decorator: Style.popupItemDelegateDecorator;
	
	property bool selected: false;

	property bool isSeparator: popupMenuDelegate.itemId === "";
	
	// Reference to the control in which the delegate is declared
	property Item containerItem: null;
	property Item rootItem: null;
	property int contentLeftMargin: Style.marginM;

	enabled: (model.item && model.item.m_enabled !==undefined && model.item.m_enabled !==null && model.item.m_enabled == false) ? false : true;
	
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
