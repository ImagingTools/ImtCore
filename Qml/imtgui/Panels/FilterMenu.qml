import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtcolgui 1.0

ControlBase {
    id: filterContainer;

    decorator: Style.filterPanelDecorator;

    heightFromDecorator: true;
	
	property CollectionFilter complexFilter: CollectionFilter {}

    signal close();
    signal clear();
}

