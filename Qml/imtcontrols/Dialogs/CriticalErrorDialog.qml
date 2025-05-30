import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import com.imtcore.imtqml 1.0

Dialog {
    id: messageDialog;

	width: Style.sizeHintM;

    title: qsTr("Error");

    topPanelComp: Component {
        Item {}
    }

    property string message;

    contentComp: Component { MessageDialogBody {
            width: messageDialog.width;
            message: messageDialog.message;
        } }
}
