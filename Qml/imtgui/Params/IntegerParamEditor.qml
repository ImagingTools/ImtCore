import QtQuick 2.0
import Acf 1.0
import imtgui 1.0

IntegerInputElementView {
    name: model.Name;
    text: model.Value;
    onEditingFinished: {
         model.Value = text;
    }
}

