import QtQuick 2.0
import Acf 1.0
import 'AuxComponents'
import imtlicgui 1.0

Item {
    id: window;

    anchors.fill: parent;

    property TreeItemModel localSettings;

    onLocalSettingsChanged: {
        thumbnailDecorator.localSettings = localSettings;
    }

    function updateModels() {
        thumbnailDecorator.updateModels();
        treeViewModel.updateModel();
        featureDependenciesModel.updateModel();
        lisenseFeaturesModel.updateModel();
    }

    ThumbnailDecorator {
        id: thumbnailDecorator;

        anchors.fill: parent;
    }

    TreeViewModel {
        id: treeViewModel;
    }

    FeatureDependenciesModel {
        id: featureDependenciesModel;
    }

    LicenseFeaturesModel {
        id: lisenseFeaturesModel;
    }
}
