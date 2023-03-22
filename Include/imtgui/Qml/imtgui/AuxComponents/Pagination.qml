import QtQuick 2.12
import Acf 1.0;

Row {
    id: paginationContainer;

    height: 20;
    spacing: 10;

    property int pagesSize: 1;
    property int currentValue: currentIndex + 1;
    property int currentIndex: 0;

    // The number of elements on the page
    property int countElements: 25;

    property alias pageCount: listModel.count;

    Component.onCompleted: {
        paginationContainer.refreshBtn();
    }

    onPagesSizeChanged: {
        console.log("ProductCollectionViewCommands onSelectedIndexChanged");
        if (paginationContainer.currentIndex > paginationContainer.pagesSize - 1){
            paginationContainer.currentIndex--;
        }

        paginationContainer.refreshBtn();
    }

    ListModel {
        id: listModel;
    }

    function refreshBtn(){
        listModel.clear();
        if (paginationContainer.pagesSize < 10){
            for (var i = 0; i < paginationContainer.pagesSize; i++){
                listModel.append({number: i+1, selected: paginationContainer.currentIndex + 1 === i + 1});
            }
        }
        else {
            [1, 2].map(function(v){listModel.append({number: v, selected: paginationContainer.currentIndex + 1 === v})});
            listModel.append({number: paginationContainer.currentIndex + 1 - 2 > 1 + 3 ? -1 : 1 + 2, selected: paginationContainer.currentIndex + 1 === 3});
            for (var k = Math.max(1 + 3, paginationContainer.currentIndex + 1 - 2); k <= Math.min(paginationContainer.pagesSize - 3, paginationContainer.currentIndex + 1 + 2); k++){
                listModel.append({number: k, selected: paginationContainer.currentIndex + 1 === k});
            }
            listModel.append({number: paginationContainer.currentIndex + 1 + 2 < paginationContainer.pagesSize - 3 ? -1 : paginationContainer.pagesSize - 2, selected: paginationContainer.currentIndex + 1 === paginationContainer.pagesSize - 2});
            [paginationContainer.pagesSize - 1, paginationContainer.pagesSize].map(function(v){listModel.append({number: v, selected: paginationContainer.currentIndex + 1 === v})});
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

        iconSource: buttonDecr.enabled ? "../../../" + "Icons/" + Style.theme + "/Left_On_Normal.svg":
                                         "../../../" + "Icons/" + Style.theme + "/Left_Off_Disabled.svg";

        onClicked: {
            if (paginationContainer.currentIndex >= 1){
                paginationContainer.currentIndex--;
                if (paginationContainer.pagesSize >= 10){
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
                width: 24;

                hasIcon: false;
                hasText: true;
                highlighted: Style.highlightedButtons !==undefined ? Style.highlightedButtons : containsMouse;


                enabled: model.number !== -1 && paginationContainer.currentIndex !== model.number - 1;
                textButton: model.number === -1 ? "..." : model.number;
                onClicked: {
                    paginationContainer.currentIndex = model.number - 1;
                    if (paginationContainer.pagesSize >= 10){
                        paginationContainer.refreshBtn();
                    }
                }

                Rectangle {
                    anchors.top: buttonDelegate.bottom;
                    width: parent.width;
                    height: 2;

                    color: Style.tabSelectedColor;
                    visible: model.index == paginationContainer.currentIndex;//model.selected;
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

        iconSource: buttonIncr.enabled ? "../../../" + "Icons/" + Style.theme + "/Right_On_Normal.svg":
                                         "../../../" + "Icons/" + Style.theme + "/Right_Off_Disabled.svg";

        onClicked: {
            if (paginationContainer.currentIndex < paginationContainer.pagesSize - 1){
                paginationContainer.currentIndex++;
                if (paginationContainer.pagesSize >= 10){
                    paginationContainer.refreshBtn();
                }
            }
        }
    }
}


