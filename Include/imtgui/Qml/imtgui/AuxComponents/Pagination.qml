import QtQuick 2.12
import Acf 1.0;
import imtqml 1.0;

Row {
    id: paginationContainer;

    height: visible ? 20 : 0;
    spacing: 10;

    property int pagesSize: 1;
    property int currentValue: currentIndex + 1;
    property int currentIndex: 0;

    // The number of elements on the page
    property int countElements: 25;

    property int countAllElements: 0;

    property alias pageCount: listModel.count;

    Component.onCompleted: {
        paginationContainer.refreshBtn();
    }

    onPagesSizeChanged: {
        console.log("Pagination onPagesSizeChanged", pagesSize);
        if (paginationContainer.currentIndex > paginationContainer.pagesSize - 1){
            paginationContainer.currentIndex = 0;
        }

        paginationContainer.refreshBtn();
    }

    ListModel {
        id: listModel;
    }

    QtObject {
        id: props;

        property int maxElementCount: 6;
    }

    function refreshBtn(){
        listModel.clear();
        if (paginationContainer.pagesSize < props.maxElementCount){
            for (var i = 0; i < paginationContainer.pagesSize; i++){
                listModel.append({number: i+1, selected: paginationContainer.currentIndex + 1 === i + 1});
            }
        }
        else {
            [1, 2].map(function(v){listModel.append({number: v, selected: paginationContainer.currentIndex + 1 === v})}.bind(this));
            listModel.append({number: paginationContainer.currentIndex + 1 - 2 > 1 + 3 ? -1 : 1 + 2, selected: paginationContainer.currentIndex + 1 === 3});
            for (var k = Math.max(1 + 3, paginationContainer.currentIndex + 1 - 2); k <= Math.min(paginationContainer.pagesSize - 3, paginationContainer.currentIndex + 1 + 2); k++){
                listModel.append({number: k, selected: paginationContainer.currentIndex + 1 === k});
            }
            listModel.append({number: paginationContainer.currentIndex + 1 + 2 < paginationContainer.pagesSize - 3 ? -1 : paginationContainer.pagesSize - 2, selected: paginationContainer.currentIndex + 1 === paginationContainer.pagesSize - 2});
            [paginationContainer.pagesSize - 1, paginationContainer.pagesSize].map(function(v){listModel.append({number: v, selected: paginationContainer.currentIndex + 1 === v})}.bind(this));
        }
        repeaterPagination.model = listModel;
    }

    AuxButton {
        id: buttonDecr;

        height: 20;
        width: 24;

        hasIcon: true;
        hasText: false;
        highlighted: Style.highlightedButtons !==undefined ? Style.highlightedButtons : containsMouse;

        enabled: listModel.count > 1 && paginationContainer.currentIndex != 0;

        iconSource: buttonDecr.enabled ? "../../../" + Style.getIconPath("Icons/Left", Icon.State.On, Icon.Mode.Normal):
                                         "../../../" + Style.getIconPath("Icons/Left", Icon.State.Off, Icon.Mode.Disabled);

        onClicked: {
            if (paginationContainer.currentIndex >= 1){
                paginationContainer.currentIndex--;
                if (paginationContainer.pagesSize >= props.maxElementCount){
                    paginationContainer.refreshBtn();
                }
            }
        }
    }

    Row{
        Repeater {
            id: repeaterPagination;

            delegate: AuxButton {
                id: buttonDelegate;

                height: 20;
                width: Math.max(24, textWidth + 10);

                hasIcon: false;
                hasText: true;
                highlighted: Style.highlightedButtons !==undefined ? Style.highlightedButtons : containsMouse;


                enabled: model.number !== -1 && paginationContainer.currentIndex !== model.number - 1;
                textButton: model.number === -1 ? "..." : model.number;
                onClicked: {
                    paginationContainer.currentIndex = model.number - 1;
                    if (paginationContainer.pagesSize >= props.maxElementCount){
                        paginationContainer.refreshBtn();
                    }
                }

                Rectangle {
                    anchors.top: buttonDelegate.bottom;
                    width: parent.width;
                    height: 2;

                    color: Style.tabSelectedColor;
                    visible: (model.index == undefined ||  model.selected == undefined) ? false : paginationContainer.pagesSize < props.maxElementCount ? model.index === paginationContainer.currentIndex : model.selected;
                }
            }
        }
    }

    AuxButton {
        id: buttonIncr;

        height: 20;
        width: 24;

        hasIcon: true;
        hasText: false;
        highlighted: Style.highlightedButtons !==undefined ? Style.highlightedButtons : containsMouse;

        enabled: listModel.count > 1 && paginationContainer.currentIndex != paginationContainer.pagesSize - 1;

        iconSource: buttonIncr.enabled ? "../../../../" + Style.getIconPath("Icons/Right", Icon.State.On, Icon.Mode.Normal):
                                         "../../../../" + Style.getIconPath("Icons/Right", Icon.State.Off, Icon.Mode.Disabled);

        onClicked: {
            if (paginationContainer.currentIndex < paginationContainer.pagesSize - 1){
                paginationContainer.currentIndex++;
                if (paginationContainer.pagesSize >= props.maxElementCount){
                    paginationContainer.refreshBtn();
                }
            }
        }
    }

    BaseText {
        text: "(" + (paginationContainer.currentIndex * paginationContainer.countElements + 1) + "-" +
              ((paginationContainer.pagesSize - 1 != paginationContainer.currentIndex) ?
                   paginationContainer.currentValue * paginationContainer.countElements :
                   paginationContainer.countAllElements) + "/" + paginationContainer.countAllElements + ")"
    }

    Row {
        spacing: 4;

        BaseText {
            text: qsTr("Per page: ")
        }

        AuxButton {
            id: count25;

            width: 20;
            height: 20;

            hasIcon: false;
            hasText: true;

            highlighted: Style.highlightedButtons !==undefined ? Style.highlightedButtons : containsMouse;

            enabled: paginationContainer.countElements != 25;

            textButton: "25";

            onClicked: {
                paginationContainer.countElements = 25;
            }
        }

        AuxButton {
            id: count50;

            width: 20;
            height: 20;

            hasIcon: false;
            hasText: true;

            highlighted: Style.highlightedButtons !==undefined ? Style.highlightedButtons : containsMouse;

            enabled: paginationContainer.countElements != 50;

            textButton: "50";

            onClicked: {
                paginationContainer.countElements = 50;
            }
        }

        AuxButton {
            id: count100;

            width: 20;
            height: 20;

            hasIcon: false;
            hasText: true;

            highlighted: Style.highlightedButtons !==undefined ? Style.highlightedButtons : containsMouse;

            enabled: paginationContainer.countElements != 100;

            textButton: "100";

            onClicked: {
                paginationContainer.countElements = 100;
            }
        }
    }
}


