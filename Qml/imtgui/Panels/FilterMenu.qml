import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

ControlBase {
    id: filterContainer;

    decorator: Style.filterPanelDecorator;

    heightFromDecorator: true;

    signal filterChanged(string filterId, string filterValue);
    signal close();
    signal clear();
}
