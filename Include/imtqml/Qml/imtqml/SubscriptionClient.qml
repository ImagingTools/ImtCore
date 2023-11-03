import QtQuick 2.0
import Acf 1.0
import imtgui 1.0

GqlModel {
    id: container;
    property string subscriptionId;

    property UuidGenerator uuidGenerator: UuidGenerator {}

    Component.onCompleted: {
        subscriptionId = uuidGenerator.generateUUID();
    }
}
