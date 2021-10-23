/*
 SPDX-FileCopyrightText: 2021 Ismael Asensio <isma.af@gmail.com>

 SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15

import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.kquickcontrolsaddons 2.0

import org.kde.kwin 2.0 as KWin


KWin.Switcher {
    id: tabBox
    currentIndex: thumbnailView.currentIndex

    PlasmaCore.Dialog {
        id: dialog
        location: PlasmaCore.Types.Floating
        visible: tabBox.visible
        //backgroundHints: PlasmaCore.Dialog.NoBackground
        flags: Qt.X11BypassWindowManagerHint
        x: 0
        y: 0

        mainItem: ColumnLayout {
            id: dialogMainItem
            width: tabBox.screenGeometry.width
            height: tabBox.screenGeometry.height

            PathView {
                id: thumbnailView

                focus: true
                Layout.fillWidth: true
                Layout.fillHeight: true

                preferredHighlightBegin: 0.5
                preferredHighlightEnd: 0.5
                highlightRangeMode: PathView.StrictlyEnforceRange

                path: Path {
                    startX: thumbnailView.width * 0.1; startY: thumbnailView.height * 0.45
                    PathAttribute { name: "z"; value: 0 }
                    PathAttribute { name: "scale"; value: 0.75 }
                    PathAttribute { name: "rotation"; value: 70 }
                    PathPercent { value: 0 }

                    PathLine { x: thumbnailView.width * 0.25 ; y: thumbnailView.height * 0.45 }
                    PathAttribute { name: "z"; value: 90 }
                    PathAttribute { name: "scale"; value: 0.75 }
                    PathAttribute { name: "rotation"; value: 70 }
                    PathPercent { value: 0.495 }

                    //Center Item
                    PathLine { x: thumbnailView.width * 0.5 ; y: thumbnailView.height * 0.65 }
                    PathAttribute { name: "z"; value: 100 }
                    PathAttribute { name: "scale"; value: 1 }
                    PathAttribute { name: "rotation"; value: 0 }
                    PathPercent { value: 0.5 }

                    PathLine { x: thumbnailView.width * 0.75 ; y: thumbnailView.height * 0.45 }
                    PathAttribute { name: "z"; value: 90 }
                    PathAttribute { name: "scale"; value: 0.75 }
                    PathAttribute { name: "rotation"; value: -70 }
                    PathPercent { value: 0.505 }

                    PathLine { x: thumbnailView.width * 0.9 ; y: thumbnailView.height * 0.45 }
                    PathAttribute { name: "z"; value: 0 }
                    PathAttribute { name: "scale"; value: 0.75 }
                    PathAttribute { name: "rotation"; value: -70 }
                    PathPercent { value: 1 }
                }

                model: tabBox.model

                delegate: Item {
                    id: delegateItem

                    width: thumbnailView.width / 2
                    height: thumbnailView.height / 2
                    scale: PathView.scale
                    z: PathView.z

                    KWin.ThumbnailItem {
                        id: thumbnail
                        wId: windowId
                        anchors.fill: parent
                    }

                    transform: Rotation {
                        origin { x: width/2; y: height/2 }
                        axis { x: 0; y: 1; z: 0 }
                        angle: delegateItem.PathView.rotation
                    }
                }

                Keys.onUpPressed: decrementCurrentIndex()
                Keys.onLeftPressed: decrementCurrentIndex()
                Keys.onDownPressed: incrementCurrentIndex()
                Keys.onRightPressed: incrementCurrentIndex()
            }

            RowLayout {
                Layout.preferredHeight: PlasmaCore.Units.iconSizes.huge
                Layout.margins: PlasmaCore.Units.gridUnit
                Layout.alignment: Qt.AlignCenter
                spacing: PlasmaCore.Units.largeSpacing

                PlasmaCore.IconItem {
                    source: tabBox.model.data(tabBox.model.index(tabBox.currentIndex, 0), Qt.UserRole + 3) // IconRole
                    Layout.alignment: Qt.AlignCenter
                }

                QQC2.Label {
                    font.bold: true
                    font.pointSize: 16
                    text: tabBox.model.data(tabBox.model.index(tabBox.currentIndex, 0), Qt.UserRole + 1) // CaptionRole
                    Layout.alignment: Qt.AlignCenter
                }
            }
        }
    }

    onCurrentIndexChanged: {
        thumbnailView.currentIndex = tabBox.currentIndex
    }
}
