import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtcontrols 1.0

ControlBase {
    id: progressBar;

    decoratorComponent: Style.isQtStyle ? DecoratorsQt.progressBarDecorator: Decorators.progressBarDecorator;

    property int barHeight: 20;

    property bool hasText: true;
    property bool hasTitle: true;

    property string title: qsTr("Прогресс:");
    property string text: ""; //+ progressBar.percent + "%";

    property int percent: position * 100;
    property real from: 0.0;
    property real to: 1.0;
    property real value: 0.0;
    property real position: (value - from)/(to - from);
    property bool indeterminate: false;

}

