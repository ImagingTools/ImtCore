export var Qt = {
    NoButton: 0x00000000,
    AllButtons: 0x07ffffff,
    LeftButton: 0x00000001,
    RightButton: 0x00000002,
    MiddleButton: 0x00000004,

    NoModifier: 0x00000000,
    ShiftModifier: 0x02000000,
    ControlModifier: 0x04000000,
    AltModifier: 0x08000000,
    MetaModifier: 0x10000000,

    ArrowCursor: 'default',
    UpArrowCursor: 'none',
    CrossCursor: 'crosshair',
    IBeamCursor: 'text',
    WaitCursor: 'wait',
    BusyCursor: 'progress',
    ForbiddenCursor: 'not-allowed',
    PointingHandCursor: 'pointer',
    WhatsThisCursor: 'help',
    DragMoveCursor: 'none',
    DragLinkCursor: 'none',
    SizeVerCursor: 'n-resize',
    SizeHorCursor: 'e-resize',
    SizeBDiagCursor: 'ne-resize',
    SizeFDiagCursor: 'nw-resize',
    SizeAllCursor: 'all-scroll',
    SplitVCursor: 'row-resize',
    SplitHCursor: 'col-resize',
    OpenHandCursor: 'grab',
    ClosedHandCursor: 'grabbing',
    DragCopyCursor: 'none',

    WindowShortcut: 0,
    ApplicationShortcut: 1,

    Unchecked: 0,
    PartiallyChecked: 1,
    Checked: 2,
    
    openUrlExternally: function(url){
        return window.open(url, '_blank')
    },
    qsTr: function(sourceText){
        return sourceText
    }
}