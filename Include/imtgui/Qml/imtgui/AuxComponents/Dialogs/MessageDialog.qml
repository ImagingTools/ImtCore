import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

Dialog {
    id: messageDialog;

    title: qsTr("Message");


    topPanelComp: Style.topPanelDialogDecorator !==undefined ? Style.topPanelDialogDecorator: topPanelDefault;

    property string message;

    Keys.onPressed: {
        console.log("MessageDialog onPressed", event.key);

        if (event.key == Qt.Key_Return){
             messageDialog.finished("Yes");
        }
    }

    contentComp: Component { MessageDialogBody {
        message: messageDialog.message;
    } }

    Component.onCompleted: {
        messageDialog.buttons.addButton({"Id":"Yes", "Name":"Yes", "Enabled": true});
        messageDialog.buttons.addButton({"Id":"No", "Name":"No", "Enabled": true});
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
