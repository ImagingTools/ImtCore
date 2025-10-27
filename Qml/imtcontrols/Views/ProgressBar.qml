import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

ControlBase {
    id: progressBar;

    decorator: Style.progressBarDecorator;

	width: Style.sizeHintXS
	/**
		\Height is calculated.
	*/
	heightFromDecorator: true;

	property int barHeight: Style.controlHeightS;

    property bool hasText: true;
    property bool hasTitle: true;

	property string title: qsTr("Progress:");
    property string text: ""; //+ progressBar.percent + "%";

	property int percent: position > 1 ? 100 :  Math.round(position * 100);
    property real from: 0.0;
    property real to: 1.0;
    property real value: 0.0;
    property real position: (value - from)/(to - from);
    property bool indeterminate: false;
    property bool barIsGradient: false;
    property int barRadius: 4;
    property string borderColor: "lightgrey";
    property string backgroundColor: "#ffffff";
    property Gradient gradient: Gradient{};
	property int fontSize: Style.fontSizeL;
}

