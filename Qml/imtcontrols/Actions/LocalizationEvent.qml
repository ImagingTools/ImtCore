import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

QtObject {
    id: root;

	signal localizationChanged(string langId);

	Component.onCompleted: {
		Events.subscribeEvent("OnLocalizationChanged", root.localizationChanged);
	}

	Component.onDestruction: {
		Events.unSubscribeEvent("OnLocalizationChanged", root.localizationChanged);
	}
}

