import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

Dialog {
	id: messageDialog;
	
	width: Style.sizeHintL;
	backgroundColor: Style.baseColor
	title: qsTr("Message");
	
	property string message;
	// Yes in the error color, No focused and taken by Enter
	property bool destructive: false;

	canMove: true;
	forceFocus: true;

	Component.onCompleted: {
		addButton(Enums.yes, qsTr("Yes"), true, false, messageDialog.destructive ? "danger" : "")
		addButton(Enums.no, qsTr("No"), true, messageDialog.destructive)
	}

	Keys.onPressed: {
		if (event.key === Qt.Key_Return){
			messageDialog.finished(messageDialog.destructive ? Enums.no : Enums.yes);
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
