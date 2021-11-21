import QtQuick 2.12
import Acf 1.0
//import ACF 1.0
//import Acf.qjsvalueApi 1.0


Rectangle {
    id: menuPanel
    width: 59
    color: "#e6e6e8"
    property string textColor: "#335777"
    property string fontName: "Helvetica"
    property alias model: lvButtons.model
    signal activePageChanged(string activePageId, string activeIcon)


    ListView {
        id: lvButtons
        anchors.fill: parent

        delegate:  MenuPanelButton{
            width: menuPanel.width
            height: width
            text:  model[PageEnum.NAME]
            textColor: menuPanel.textColor
            fontName: menuPanel.fontName
            imageSource: model[PageEnum.ICON]
            imageSourceDisabled: model[PageEnum.ICON_OFF_SELECTED]
            imageSourceSelected: model[PageEnum.ICON_ON_SELECTED]
            selected: lvButtons.currentIndex === index ? true : false

            onClicked: {
                lvButtons.currentIndex = index
                menuPanel.activePageChanged(model[PageEnum.ID], model[PageEnum.ICON])
                console.log("window.location.href: ", window.location.href)
                console.log("window.location.hash: ", window.location.hash)
                console.log("window.location.search: ", window.location.search)
//                console.log("Acf", PageEnum.ICON)
//                console.log("AcfModel", model[PageEnum.ICON])

//                lvButtons.model.SetData(AcfS.Name,"Name2",index)

//                var column_number = 0;
//                var q_model_index = lvButtons.model.index(index, column_number);

//                console.log("Acf", AcfApi)

//                console.log("Acf", lvButtons.model.getData("Name", index))
            }
        }

    }

}
