/*
 * Copyright 2012  Reza Fatahilah Shah <rshah0385@kireihana.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 1.1
import org.kde.plasma.core 0.1 as PlasmaCore
import org.kde.plasma.components 0.1 as PlasmaComponents
import org.kde.plasma.extras 0.1 as PlasmaExtras
import org.kde.qtextracomponents 0.1

PlasmaExtras.ScrollArea {
    id: imageWidget
    property alias image: comicPicture.image
    property bool fullView: false
    property alias tooltipText: tooltip.mainText

    width: comicPicture.nativeWidth
    height: comicPicture.nativeHeight

    Flickable {
        id: viewContainer
        anchors.fill:parent

        contentWidth: fullView ? comicPicture.nativeWidth : viewContainer.width
        contentHeight: fullView ? comicPicture.nativeHeight : viewContainer.height
        clip: true
        
        QImageItem {
            id: comicPicture
            anchors.fill: parent
            smooth: true
            fillMode: QImageItem.PreserveAspectFit

            MouseArea {
                id:mouseArea
                anchors.fill: comicPicture
                hoverEnabled: true
                preventStealing: false
                acceptedButtons: Qt.LeftButton | Qt.MiddleButton

                PlasmaCore.ToolTip {
                    id: tooltip
                    target: mouseArea
                }

                onClicked: {
                    if (mouse.button == Qt.MiddleButton && comicApplet.middleClick) {
                        fullDialog.open();
                    }
                }

                ButtonBar {
                    id: buttonBar
                    visible: (comicApplet.arrowsOnHover && mouseArea.containsMouse)
                    opacity: 0
                    anchors {
                        horizontalCenter: parent.horizontalCenter
                        //bottom: imageWidget.bottom
                        //bottomMargin:10
                    }
                    states: State {
                        name: "show"; when: (comicApplet.arrowsOnHover && mouseArea.containsMouse)
                        PropertyChanges { target: buttonBar; opacity: 1; }
                    }

                    transitions: Transition {
                        from: ""; to: "show"; reversible: true
                        NumberAnimation { properties: "opacity"; duration: 250; easing.type: Easing.InOutQuad }
                    }

                    onPrevClicked: {
                        console.log("Previous clicked");
                        //busyIndicator.visible = true;
                        comicApplet.updateComic(comicData.prev);
                    }
                    onNextClicked: {
                        console.log("Next clicked");
                        //busyIndicator.visible = true;
                        comicApplet.updateComic(comicData.next);
                    }
                    onZoomClicked: {
                        fullDialog.open();
                    }
                }
            }
        }
    }
}
