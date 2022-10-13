import {QtObject} from './QtObject'

export class ListElement extends QtObject  {
    constructor(parent) {
        super(parent)
        parent.append(this)
    }
    $domCreate(){}
}

QML.ListElement = ListElement