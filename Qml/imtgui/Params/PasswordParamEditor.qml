import QtQuick 2.0
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

TextInputElementView {
    name: model.Name;
    text: model.Value;
    echoMode: TextInput.Password;
    onEditingFinished: {
         model.Value = text;
    }
}
