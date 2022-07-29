import QtQuick 2.12
import Acf 1.0
import imtgui 1.0


Item {
    id: buttonsDialogContainer;

    height: 30;
    width: rowButtons.width;

    signal buttonClicked(string buttonId);

    function addButton(buttonObj){
        console.log("buttonsDialogContainer addButton");
        buttonsModel.append(buttonObj)
    }

    ListModel {
        id: buttonsModel;
    }

    Row {
        id: rowButtons;

        height: buttonsDialogContainer.height;

        spacing: 10;

        Repeater {
            id: buttonsRepeater;

            model: buttonsModel;

            delegate: AuxButton {
                id: delegateButton;

                Component.onCompleted: {
//                    console.log("AuxButton onCompleted", model.Id, model.Name. model.index);
                }

                width: 70;
                height: 25;

                hasText: true;
                hasIcon: false;

                textButton: model.Name;
                borderColor: delegateButton.highlighted ? Style.iconColorOnSelected : Style.buttonColor;
                backgroundColor: Style.imagingToolsGradient1;

                onClicked: {
                    console.log("AuxButton onClicked", model.Id);
                    buttonsDialogContainer.buttonClicked(model.Id);
                }
            }
        }
    }
}
