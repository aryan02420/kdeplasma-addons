/*
 * Copyright 2012  Luís Gabriel Lima <lampih@gmail.com>
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

Item {
    id: root

    property string direction
    property alias speed: speedDisplay.number
    property alias unit: unitLabel.text

    implicitHeight: windArrows.naturalSize.height
    implicitWidth: windArrows.naturalSize.width

    QtObject {
        id: resizeOpts
        property real rate: root.height / root.implicitHeight
    }

    PlasmaCore.Svg {
        id: windSvg
        imagePath: "weatherstation/wind_arrows"
    }

    PlasmaCore.SvgItem {
        id: windArrows
        anchors.fill: parent
        svg: windSvg
        elementId: "wind:" + root.direction
    }

    LCDDisplay {
        id: speedDisplay
        anchors.centerIn: parent
        height: Math.round(0.8 * implicitHeight * resizeOpts.rate)
    }

    Text {
        id: unitLabel
        anchors{
            top: speedDisplay.bottom
            topMargin: Math.round(3 * resizeOpts.rate)
            horizontalCenter: parent.horizontalCenter
        }
        font {
            family: "Bitstream Vera Sans"
            pixelSize: 9 * resizeOpts.rate
        }
        color: "#202020"
    }
}