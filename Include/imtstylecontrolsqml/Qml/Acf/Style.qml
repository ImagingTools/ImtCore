pragma Singleton

import imtguigql 1.0

ClientStyle {
    id: styleContainer;

	// Same qrc paths as imtstylecontrolsqml.qrc (`/Fonts`). A relative ../Fonts path breaks on
	// desktop (resolves under qrc:/qml/) and the UI falls back to Segoe UI — barred capital I.
	mainFontSource: "qrc:/Fonts/Ubuntu-Regular.ttf";
	boldFontSource: "qrc:/Fonts/Ubuntu-Bold.ttf";
	fontFamily: "Ubuntu";
	fontFamilyBold: "Ubuntu";

    buttonWidthM: 20;

    dialogBackgroundColor: styleContainer.backgroundColor2;

	sizePanelsHeight: 50;

	enableTopPanel: false;

}
