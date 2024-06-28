pragma Singleton

import QtQuick 2.0
import Acf 1.0
import imtguigql 1.0

ClientStyle {
    id: styleContainer;

    size_mainMargin: 11;

    buttonWidthMedium: 20;

    dialogBackgroundColor: styleContainer.backgroundColor;

    property int comboBoxHeight: 27;
    property int size_panelsHeight: 40;
}
