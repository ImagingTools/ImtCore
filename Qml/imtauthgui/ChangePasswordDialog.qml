import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

Dialog {
    id: rootDialog;

    width: 700;

    Component.onCompleted: {
        buttonsModel.append({Id: Enums.save, Name:qsTr("Save"), Enabled: false})
        buttonsModel.append({Id: Enums.cancel, Name:qsTr("Cancel"), Enabled: true})
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
            height: column.height + 2 *  Style.size_largeMargin;

            property alias oldPassword: column.oldPassword;
            property alias newPassword: column.newPassword;

            PasswordInput {
                id: column;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: parent.left;
                anchors.leftMargin: Style.size_largeMargin;
                anchors.right: parent.right;
                anchors.rightMargin: Style.size_largeMargin;

                onAcceptedChanged: {
                    if (rootDialog.buttons){
                        rootDialog.buttons.setButtonState(Enums.save, accepted)
                    }
                }
            }
        }
    }
}


