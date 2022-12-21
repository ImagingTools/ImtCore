import {QtObject} from './QtObject'
import {Signal} from '../utils/Signal'

export class Shortcut extends QtObject {
    constructor(args) {
        super(args)

        this.$cP('autoRepeat', true)
        this.$cP('context', Qt.WindowShortcut)
        this.$cP('enabled', true)
        this.$cP('nativeText', '')
        this.$cP('portableText', '')
        this.$cP('sequence', '')
        this.$cP('sequences', [])
        this.$cP('sequence', '')

        this.$s['activated'] = Signal()
        this.$s['activatedAmbiguously'] = Signal()
    }
}

QML.Shortcut = Shortcut