/*
 * View3D - desktop wrapper around QtQuick3D's View3D.
 *
 * The imt3dgui module exposes 3D types under the same names that the JQML
 * web compiler / runtime understands (see Tools/JQML/v2/components/View3D.js).
 * Applications can therefore write `import imt3dgui 1.0` and use a single
 * QML source tree on both desktop (backed by QtQuick3D) and web (backed by
 * three.js).
 */
import QtQuick 2.15
import QtQuick3D 6.5 as QQ3D

QQ3D.View3D {
}
