import QtQuick 2.12
import Acf 1.0
//import ACF 1.0
//import Acf.qjsvalueApi 1.0


Rectangle {
    id: container
    width: 59
    color: "#e6e6e8"
    property string textColor: "#335777"
    property string fontName: "Helvetica"
    property alias model: lvButtons.model
    signal activePageChanged(string activePageId)


    ListView {
        id: lvButtons
        anchors.fill: parent

        delegate:  MenuPanelButton{
            width: container.width
            height: width
            text:  model[PageEnum.NAME]
            textColor: container.textColor
            fontName: container.fontName
            imageSource: model[PageEnum.ICON]
            imageSourceDisabled: model[PageEnum.ICON_OFF_SELECTED]
            imageSourceSelected: model[PageEnum.ICON_ON_SELECTED]
            selected: lvButtons.currentIndex === index ? true : false

            onClicked: {
                lvButtons.currentIndex = index
                container.activePageChanged(model[PageEnum.ID])
//                console.log("Acf", PageEnum.NAME)
//                console.log("Acf", model.name)

//                lvButtons.model.SetData(AcfS.Name,"Name2",index)

//                var column_number = 0;
//                var q_model_index = lvButtons.model.index(index, column_number);

//                console.log("Acf", AcfApi)

//                console.log("Acf", lvButtons.model.getData("Name", index))
            }
        }

    }

}
