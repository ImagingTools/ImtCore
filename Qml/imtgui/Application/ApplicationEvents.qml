import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

QtObject {
	id: root
	
	signal localizationChanged(string langId)
	signal designSchemeChanged(string scheme)
	signal appWidthChanged(int width)
	signal appHeightChanged(int height)
	
	Component.onCompleted: {
		Events.subscribeEvent("OnLocalizationChanged", localizationChanged);
		Events.subscribeEvent("DesignSchemeChanged", designSchemeChanged);
		Events.subscribeEvent("AppWidthChanged", appWidthChanged);
		Events.subscribeEvent("AppHeightChanged", appHeightChanged);
	}

	Component.onDestruction: {
		Events.unSubscribeEvent("OnLocalizationChanged", localizationChanged);
		Events.unSubscribeEvent("DesignSchemeChanged", designSchemeChanged);
		Events.unSubscribeEvent("AppWidthChanged", appWidthChanged);
		Events.unSubscribeEvent("AppHeightChanged", appHeightChanged);
	}
}


