import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

Item {
    property alias firstElementImageSource: accountsMultiDocView.firstElementImageSource;

    MultiDocWorkspaceView {
        id: accountsMultiDocView;
            anchors.fill: parent;
        //    color: "red";
        Component.onCompleted: {
            accountsMultiDocView.addToHeadersArray("", "Accounts", "../../imtlicgui/AccountCollectionView.qml")
        }

    }
}
