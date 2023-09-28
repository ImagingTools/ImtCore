import QtQuick 2.0
import Acf 1.0
import imtcontrols 1.0
//import 'Application'

ApplicationMain{
    id: window;

    function updateAllModels(){
        console.log("settingsProviderLocal.updateModel", Style.textColor);
        thumbnailDecoratorGui.updateModels();

        applicationInfoProvider.updateModel();
    }
}

