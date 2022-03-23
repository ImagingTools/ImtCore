import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

Item {
    id: testMultiDocViewContainer;

    Component.onCompleted: {
        //rect1.forceActiveFocus();
    }

    Rectangle {
        id: rect1;

        width: 100;
        height: 100;

        focus: true;

        KeyNavigation.right: rect2;
        KeyNavigation.tab: rect2;

        color: focus ? "red" : "blue";
    }

    Rectangle {
        id: rect2;

        anchors.left: rect1.right;
        anchors.leftMargin: 20;

        width: 100;
        height: 100;

        KeyNavigation.right: rect3;
        KeyNavigation.tab: rect3;

        color: focus ? "red" : "blue";

    }

    Rectangle {
        id: rect3;

        anchors.left: rect2.right;
        anchors.leftMargin: 20;

        width: 100;
        height: 100;

        KeyNavigation.right: rect1;
        KeyNavigation.tab: rect1;

        color: focus ? "red" : "blue";
    }
}
