import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

Dialog {
    id: messageDialog;

    width: 400;

    buttonsModel: ListModel{
        ListElement{Id: Enums.ButtonType.Ok; Name:qsTr("OK"); Enabled: true}
    }

    title: qsTr("Error");

    topPanelComp: Style.topPanelDialogDecorator !==undefined ? Style.topPanelDialogDecorator: topPanelDefault;

    property string message;

    Keys.onPressed: {
        if (event.key == Qt.Key_Enter){
            messageDialog.finished(Enums.ButtonType.Ok);
        }
    }

    onLocalizationChanged: {
        messageDialog.buttonsModel.setProperty(0, "Name", qsTr("OK"));
    }

    contentComp: Component { MessageDialogBody {
            width: messageDialog.width;
            message: messageDialog.message;
        } }

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
