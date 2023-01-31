import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

Dialog {
    id: messageDialog;

    title: qsTr("Error");

    topPanelComp: Style.topPanelDialogDecorator !==undefined ? Style.topPanelDialogDecorator: topPanelDefault;

    property string message;

    Component.onCompleted: {
        messageDialog.buttons.addButton({"Id":"Ok", "Name":"OK", "Enabled": true});
    }

    contentComp: MessageDialogBody {
        message: messageDialog.message;
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
