import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

Dialog {
	id: rootDialog;
	
	width: 700;
	
	property bool currentPasswordInputVisible: true;
	
	Component.onCompleted: {
		addButton(Enums.save, qsTr("Save"), false)
		addButton(Enums.cancel, qsTr("Cancel"), true)
	}
	
	Keys.onPressed: {
		if (event.key === Qt.Key_Return){
			let enabled = rootDialog.buttons.getButtonState(Enums.ok);
			if (enabled){
				rootDialog.finished(Enums.ok)
			}
		}
	}
	
	contentComp: Component {
		Item {
			width: rootDialog.width;
			height: column.height + 2 *  Style.sizeLargeMargin;
			
			property alias oldPassword: column.oldPassword;
			property alias newPassword: column.newPassword;
			
			PasswordInput {
				id: column;
				
				anchors.verticalCenter: parent.verticalCenter;
				anchors.left: parent.left;
				anchors.leftMargin: Style.sizeLargeMargin;
				anchors.right: parent.right;
				anchors.rightMargin: Style.sizeLargeMargin;
				currentPasswordInputVisible: rootDialog.currentPasswordInputVisible;
				
				onAcceptedChanged: {
					if (rootDialog.buttons){
						rootDialog.buttons.setButtonState(Enums.save, accepted)
					}
				}
			}
		}
	}
}


