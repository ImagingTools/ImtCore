import QtQuick 2.0
import Acf 1.0
import imtgui 1.0

TextInputElementView {
    name: model.Name;
    text: model.Value;
    controlWidth: 300;
    readOnly: true;
}
