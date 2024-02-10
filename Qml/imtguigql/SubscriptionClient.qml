import QtQuick 2.0
import Acf 1.0
import imtcontrols 1.0


GqlModel {
    id: container;
    property string subscriptionId;

    Component.onCompleted: {
        subscriptionId = UuidGenerator.generateUUID();
    }
}
