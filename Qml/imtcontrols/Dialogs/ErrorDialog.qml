import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

Dialog {
    id: messageDialog;

    width: 400;

    title: qsTr("Error");
    forceFocus: true;

    topPanelComp: Style.topPanelDialogDecorator !==undefined ? Style.topPanelDialogDecorator: topPanelDefault;

    property string message;

    Component.onCompleted: {
        buttonsModel.append({Id: Enums.ok, Name:qsTr("OK"), Enabled: true})
    }

    Keys.onPressed: {
        if (event.key === Qt.Key_Return){
            messageDialog.finished(Enums.ok);
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
