import {QtObject} from './QtObject'

export class ListElement extends QtObject  {
    constructor(args) {
        super(args)
        args.parent.append(this)
    }
    $domCreate(){}
}

QML.ListElement = ListElement