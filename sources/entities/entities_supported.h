/******************************************************************************
 *
 * Copyright (C) 2020 Marton Borzak <hello@martonborzak.com>
 *
 * This file is part of the YIO-Remote software project.
 *
 * YIO-Remote software is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * YIO-Remote software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with YIO-Remote software. If not, see <https://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *****************************************************************************/
#pragma once

#include <QObject>

#include "blind.h"
#include "climate.h"
#include "light.h"
#include "mediaplayer.h"
#include "remote.h"
#include "switch.h"
#include "weather.h"

class EntitiesSupported : public QObject {
    Q_OBJECT

 public:
    /// ADD NEW ENTITY TYPE HERE
    enum SupportedEntityTypes { LIGHT, BLIND, MEDIA_PLAYER, REMOTE, WEATHER, CLIMATE, SWITCH };
    Q_ENUM(SupportedEntityTypes)
};
