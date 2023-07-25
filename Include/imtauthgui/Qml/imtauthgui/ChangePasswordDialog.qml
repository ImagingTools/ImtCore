import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

Dialog {
    id: root;

    width: 600;

    property string password: contentItem.password;

    Component.onCompleted: {
        console.log("InputDialog onCompleted", root);

        root.buttons.addButton({"Id":"Ok", "Name":qsTr("OK"), "Enabled": false, "Active": true});
        root.buttons.addButton({"Id":"Cancel", "Name":qsTr("Cancel"), "Enabled": true, "Active": true});
    }

    contentComp: Component { Item {
        id: inputDialogBodyContainer;

        height: columnBody.height + 30;

        property alias password: columnBody.password;

        PasswordInput {
            id: columnBody;

            anchors.verticalCenter: parent.verticalCenter;
            anchors.right: parent.right;
            anchors.left: parent.left;
            anchors.rightMargin: 10;
            anchors.leftMargin: 10;

            spacing: 10;

            onAcceptedChanged: {
                root.buttons.setButtonState("Ok", columnBody.accepted)
            }
        }
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
                messageDecoratorLoader.item.rootItem = root;
            }

            root.width = 300;
        }
    }
}
