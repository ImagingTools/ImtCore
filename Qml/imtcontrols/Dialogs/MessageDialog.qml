import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

Dialog {
	id: messageDialog;
	
	width: Style.sizeHintL;
	
	title: qsTr("Message");
	
	property string message;
	
	canMove: true;
	forceFocus: true;
	
	Component.onCompleted: {
		addButton(Enums.yes, qsTr("Yes"), true)
		addButton(Enums.no, qsTr("No"), true)
	}
	
	Keys.onPressed: {
		if (event.key === Qt.Key_Return){
			messageDialog.finished(Enums.yes);
		}
	}
	
	onLocalizationChanged: {
		setButtonName(Enums.yes, qsTr("Yes"))
		setButtonName(Enums.no, qsTr("No"))
	}
	
	contentComp: Component {
		MessageDialogBody {
			width: messageDialog.width;
			message: messageDialog.message;
		}
	}
}
