import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtauthgui 1.0


Item {
    id: buttonsDialogContainer;

    signal buttonClicked(string buttonId);

    function addButton(buttonObj){
        let buttonId = buttonObj["Id"];
        let buttonText = buttonObj["Text"];

        let index = buttonsModel.InsertNewItem();
        buttonsModel.SetData("Id", buttonId, index);
        buttonsModel.SetData("Name", buttonText, index);
    }

    TreeItemModel {
        id: buttonsModel;
    }

    Row {
        id: rowButtons;

        spacing: 10;

        Repeater {
            id: buttonsRepeater;

            model: buttonsModel;

            delegate: AuxButton {

                width: 70;
                height: 25;

                hasText: true;
                hasIcon: false;

                textButton: model.Name;

                onClicked: {
                    buttonsDialogContainer.buttonClicked(model.Id);
                }
            }
        }
    }
}
