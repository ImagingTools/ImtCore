import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

Item {
    id: testMultiDocViewContainer;

    anchors.fill: parent;

    Component.onCompleted: {

        for (var i = 0; i < 10; i++){
            modelCb1.append({'name': i})
        }

        for (var i = 0; i < 20; i++){
            modelCb2.append({'name': i + 20})
        }
    }

    ListModel {
        id: modelCb1;
    }

    ListModel {
        id: modelCb2;
    }

    ComboBox {
        id: cb1;

        width: 100;
        height: 30;

        radius: 3;

        model: modelCb1;
    }

    ComboBox {
        id: cb2;

        anchors.top: cb1.bottom;
        anchors.topMargin: 10;

        width: 100;
        height: 30;

        radius: 3;

        model: modelCb2;
    }

    ComboBox {
        id: cb3;

        anchors.top: cb2.top;
        anchors.left: cb2.right;
        anchors.leftMargin: 50;

        width: 100;
        height: 30;

        radius: 3;

        model: modelCb2;
    }

    Column {
        x: 0;
        y: 200;

        width: 100;
        height: 200;

        spacing: 20;

        ComboBox {
            id: c3;

            width: 100;
            height: 30;

            radius: 3;

            model: modelCb2;
        }

        ComboBox {
            id: c4;

            width: 100;
            height: 30;

            radius: 3;

            model: modelCb2;
        }
    }
}
