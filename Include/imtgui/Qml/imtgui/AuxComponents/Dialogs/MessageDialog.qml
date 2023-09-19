import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

Dialog {
    id: messageDialog;

    title: qsTr("Message");

    topPanelComp: Style.topPanelDialogDecorator !==undefined ? Style.topPanelDialogDecorator: topPanelDefault;

    property string message;

    canMove: true;

    Keys.onPressed: {
        console.log("MessageDialog onPressed", event.key);

        if (event.key == Qt.Key_Return){
            messageDialog.finished("Yes");
        }
    }

    onLocalizationChanged: {
        messageDialog.buttonsModel.setProperty(0, "Name", qsTr("Yes"));
        messageDialog.buttonsModel.setProperty(1, "Name", qsTr("No"));
    }

    contentComp: Component {
        MessageDialogBody {
            message: messageDialog.message;
        }
    }

    Component.onCompleted: {
        messageDialog.buttons.addButton({"Id":"Yes", "Name":qsTr("Yes"), "Enabled": true});
        messageDialog.buttons.addButton({"Id":"No", "Name":qsTr("No"), "Enabled": true});
    }

    Component{
        id: emptyDecorator;
        Item{
            property Item rootItem: null;
        }
    }

    Loader{
        id: messageDecoratorLoader;

        sourceComponent: Style.messageDecorator !==undefined ? Style.messageDecorator: emptyDecorator;
        onLoaded: {
            if(messageDecoratorLoader.item){
                messageDecoratorLoader.item.rootItem = messageDialog;
            }
        }
    }
}
