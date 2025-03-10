/******************************************************************************
 *
 * Copyright (C) 2018-2019 Marton Borzak <hello@martonborzak.com>
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

#include <QMetaEnum>
#include <QObject>
#include <QString>
#include <QVariant>

#include "entity.h"
#include "yio-interface/entities/mediaplayerinterface.h"

class MediaPlayer : public Entity, MediaPlayerInterface {
    Q_OBJECT
    Q_INTERFACES(MediaPlayerInterface)

 public:
    // properties
    Q_PROPERTY(int volume READ volume NOTIFY volumeChanged)
    Q_PROPERTY(bool muted READ muted NOTIFY mutedChanged)
    Q_PROPERTY(QString mediaType READ mediaType NOTIFY mediaTypeChanged)
    Q_PROPERTY(QString mediaTitle READ mediaTitle NOTIFY mediaTitleChanged)
    Q_PROPERTY(QString mediaArtist READ mediaArtist NOTIFY mediaArtistChanged)
    Q_PROPERTY(QString mediaImage READ mediaImage NOTIFY mediaImageChanged)
    Q_PROPERTY(int mediaDuration READ mediaDuration NOTIFY mediaDurationChanged)  // in seconds
    Q_PROPERTY(int mediaProgress READ mediaProgress NOTIFY mediaProgressChanged)  // in seconds
    Q_PROPERTY(QString source READ source NOTIFY sourceChanged)
    Q_PROPERTY(QObject* searchModel READ searchModel NOTIFY searchModelChanged)
    Q_PROPERTY(QObject* browseModel READ browseModel NOTIFY browseModelChanged)
    Q_PROPERTY(QVariant recentSearches READ recentSearches WRITE setrecentSearches NOTIFY recentSearchesChanged)

    int     volume() override { return m_volume; }
    int     mediaDuration() override { return m_mediaDuration; }
    int     mediaProgress() override { return m_mediaProgress; }
    bool    muted() override { return m_muted; }
    QString mediaType() override { return m_mediaType; }
    QString mediaImage() override { return m_mediaImage; }
    QString mediaTitle() override { return m_mediaTitle; }
    QString mediaArtist() override { return m_mediaArtist; }
    QString source() override { return m_source; }

    // extension for media browsing
    QObject* searchModel() { return m_searchModel; }
    QObject* browseModel() { return m_browseModel; }
    QVariant recentSearches() { return m_recentSearches; }

    void setrecentSearches(QVariant list);

    // commands
    Q_INVOKABLE void play();
    Q_INVOKABLE void pause();
    Q_INVOKABLE void stop();
    Q_INVOKABLE void previous();
    Q_INVOKABLE void next();
    Q_INVOKABLE void setVolume(int value);
    Q_INVOKABLE void volumeUp();
    Q_INVOKABLE void volumeDown();

    bool isOn() override { return m_state == MediaPlayerDef::ON || m_state == MediaPlayerDef::PLAYING; }
    bool supportsOn() override;
    bool updateAttrByIndex(int attrIndex, const QVariant& value) override;
    void turnOn() override;
    void turnOff() override;

    // extension for media browsing
    Q_INVOKABLE void browse(QString command);  // Command item_key, "TOP", "BACK", "PLAY"
    Q_INVOKABLE void playMedia(const QString& itemKey, const QString& type);
    Q_INVOKABLE void addToQueue(const QString& itemKey, const QString& type);
    Q_INVOKABLE void search(const QString& searchText, const QString& itemKey);  // Search
    Q_INVOKABLE void search(const QString& searchText);
    Q_INVOKABLE void getAlbum(const QString& id);
    Q_INVOKABLE void getPlaylist(const QString& id);

    void setSearchModel(QObject* model) override;
    void setBrowseModel(QObject* model) override;

    // constructor
    MediaPlayer(const QVariantMap& config, IntegrationInterface* integrationObj, QObject* parent = nullptr);

 signals:
    void volumeChanged();
    void mutedChanged();
    void mediaTypeChanged();
    void mediaTitleChanged();
    void mediaArtistChanged();
    void mediaImageChanged();
    void mediaDurationChanged();
    void mediaProgressChanged();
    void sourceChanged();
    void searchModelChanged();
    void browseModelChanged(QObject* model);
    void recentSearchesChanged();

 public:
    static QString Type;

 private:
    int     m_volume;
    int     m_mediaDuration;
    int     m_mediaProgress;
    bool    m_muted;
    QString m_mediaType;
    QString m_mediaImage;
    QString m_mediaTitle;
    QString m_mediaArtist;
    QString m_source;

    // extension for media browsing
    QObject* m_searchModel = nullptr;
    /*                         {
     *                         albums:          array of {id, title, subtitle, image_url, array of commands}
     *                         tracks:          array of {id, title, subtitle, image_url, array of commands}
     *                         artists:         array of {id, title, subtitle, image_url, array of commands}
     *                         playlists:       array of {id, title, subtitle, image_url, array of commands}
     *                         }
     */
    QObject* m_browseModel = nullptr;
    QVariant m_recentSearches;
};
