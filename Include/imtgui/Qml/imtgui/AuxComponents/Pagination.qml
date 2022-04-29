import QtQuick 2.0
import Acf 1.0;

Row {
    id: paginationContainer;

    height: 30;
    spacing: 10;

    property int pagesSize;
    property int currentValue: 1;
    property int currentIndex: 0;

    Component.onCompleted: {
        paginationContainer.refreshBtn();
    }

    ListModel {
        id: listModel;
    }

    function refreshBtn() {
            listModel.clear();
            if (paginationContainer.pagesSize < 10){
                for (var i = 0; i < paginationContainer.pagesSize; i++){
                    listModel.append({number: i+1, selected: currentValue === i + 1});
                }
            }
            else {
                [1, 2].map(v => listModel.append({number: v, selected: currentValue === v}));
                listModel.append({number: currentValue - 2 > 1 + 3 ? -1 : 1 + 2, selected: currentValue === 3});
                for (var k = Math.max(1 + 3, currentValue - 2); k <= Math.min(paginationContainer.pagesSize - 3, currentValue + 2); k++) {
                    listModel.append({number: k, selected: currentValue === k});
                }
                listModel.append({number: currentValue + 2 < paginationContainer.pagesSize - 3 ? -1 : paginationContainer.pagesSize - 2, selected: currentValue === paginationContainer.pagesSize - 2});
                [paginationContainer.pagesSize - 1, paginationContainer.pagesSize].map(v => listModel.append({number: v, selected: currentValue === v}));
            }
            repeaterPagination.model = listModel
        }

    AuxButton {
        id: buttonDecr;

        height: 20;
        width: 24;

        hasIcon: true;
        hasText: false;

        iconSource: "../../../" + "Icons/" + Style.theme + "/Left_On_Normal.svg";

        onClicked: {
            if (paginationContainer.currentValue - 1 >= 1){
                paginationContainer.currentValue--;;
                paginationContainer.refreshBtn();;
            }
        }
    }

    Repeater {
        id: repeaterPagination;
        delegate: AuxButton {
            id: buttonDelegate;

            height: 20;
            width: 24;

            hasIcon: false;
            hasText: true;

            enabled: model.number !== -1;
            textButton: model.number === -1 ? "..." : model.number;
            onClicked: {
                paginationContainer.currentIndex = model.index;
                paginationContainer.currentValue = model.number
                paginationContainer.refreshBtn()
            }

            Rectangle {
                anchors.top: buttonDelegate.bottom;
                width: parent.width;
                height: 2;

                color: Style.tabSelectedColor;
                visible: model.selected;
            }
        }
    }

    AuxButton {
        id: buttonIncr;

        height: 20;
        width: 24;

        hasIcon: true;
        hasText: false;

        iconSource: "../../../" + "Icons/" + Style.theme + "/Right_On_Normal.svg";

        onClicked: {
            if (paginationContainer.currentValue + 1 <= paginationContainer.pagesSize){
                paginationContainer.currentValue++;
                paginationContainer.refreshBtn();
            }
        }
    }
}


