/******************************************************************************
 *
 * Copyright (C) 2019 Christian Riedl <ric@rts.co.at>
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

#include <QDateTime>
#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QLoggingCategory>
#include <QMutex>
#include <QObject>
#include <QQueue>
#include <QTextStream>

#include "yio-interface/plugininterface.h"

class Logger : public QObject {
    Q_OBJECT

 public:
    // Unfortunately it is not possible to use QtMsgType as logLevel, using an int
    Q_PROPERTY(int logLevel READ logLevel WRITE setLogLevel)  // default log level
    Q_PROPERTY(bool fileEnabled READ fileEnabled WRITE setFileEnabled)
    Q_PROPERTY(bool queueEnabled READ queueEnabled WRITE setQueueEnabled)
    Q_PROPERTY(bool consoleEnabled READ consoleEnabled WRITE setConsoleEnabled)
    Q_PROPERTY(bool showSourcePos READ showSourcePos WRITE setShowSourcePos)

    // write log functions intended for QML
    Q_INVOKABLE void write(const QString& msg);
    Q_INVOKABLE void writeDebug(const QString& msg);
    Q_INVOKABLE void writeInfo(const QString& msg);
    Q_INVOKABLE void writeWarning(const QString& msg);

    Q_INVOKABLE int toMsgType(const QString& msgType);

    // set category log level, if category is not existing it is created
    Q_INVOKABLE void setCategoryLogLevel(const QString& category, int level);
    Q_INVOKABLE int  getCategoryLogLevel(const QString& category);

    // important when a plugin is unloaded
    Q_INVOKABLE void removeCategory(const QString& category);

    // for use in QML or from YIO API
    Q_INVOKABLE QJsonArray  getQueuedMessages(int maxCount, int logLevel, const QStringList& categories);
    Q_INVOKABLE QJsonObject getInformation();

    Q_INVOKABLE int  getFileCount();
    Q_INVOKABLE void purgeFiles(int purgeHours);

    // path :       directory for log file, if empty no log file
    // logLevel :   default log level
    // debug :      output to QtCreator DEBUG
    // showSource : show qDebug ... source and line
    // queueSize :  maximum Queue size
    // purgeHours : purge at start
    explicit Logger(const QString& path, QString logLevel = "DEBUG", bool console = true, bool showSource = false,
                    int queueSize = 100, int purgeHours = 12, QObject* parent = nullptr);
    ~Logger();

    QtMsgType      logLevel() { return m_logLevel; }
    void           setLogLevel(int logLevel);
    bool           fileEnabled() { return m_fileEnabled; }
    void           setFileEnabled(bool value) { m_fileEnabled = value; }
    bool           queueEnabled() { return m_queueEnabled; }
    void           setQueueEnabled(bool value) { m_queueEnabled = value; }
    bool           consoleEnabled() { return m_consoleEnabled; }
    void           setConsoleEnabled(bool value) { m_consoleEnabled = value; }
    bool           showSourcePos() { return m_showSource; }
    void           setShowSourcePos(bool value) { m_showSource = value; }
    static Logger* getInstance() { return s_instance; }

    // for use from C++ to register a Logging category
    // used by integrations to register plugin logging
    void defineLogCategory(const QString& category, int level, QLoggingCategory* loggingCategory = nullptr,
                           PluginInterface* plugin = nullptr);

 private:
    struct SCategory {
        explicit SCategory(QtMsgType logLevel, QLoggingCategory* logCategory = nullptr,
                           PluginInterface* plugin = nullptr)
            : logCategory(logCategory), plugin(plugin), logLevel(logLevel), logLevelMask(logLevelToMask(logLevel)) {
            memset(count, 0, sizeof(count));
        }
        QLoggingCategory* logCategory;                      // logCategory
        PluginInterface*  plugin;                           // plugin
        QtMsgType         logLevel;                         // !!ored with overall log level
        quint16           logLevelMask;                     // required because QtMsgType has strange sorting
        quint16           count[QtMsgType::QtInfoMsg + 1];  // counts errors per msg type
    };
    struct SMessage {
        SMessage(QtMsgType type, uint timestamp, const QString& category, const QString& message,
                 const QString& sourcePosition)
            : type(type), timestamp(timestamp), category(category), message(message), sourcePosition(sourcePosition) {}
        QtMsgType type;
        uint      timestamp;  // unix time
        QString   category;
        QString   message;
        QString   sourcePosition;
    };

    void processMessage(QtMsgType type, const char* category, const char* source, int line, const QString& msg,
                        bool writeanyHow = false);
    void writeFile(const SMessage& message, const QDateTime& dt);
    void writeQueue(const SMessage& message);
    void writeConsole(const SMessage& message);

    static void    messageOutput(QtMsgType type, const QMessageLogContext& context, const QString& msg);
    static quint16 logLevelToMask(QtMsgType logLevel);

    static Logger*     s_instance;
    static QStringList s_msgTypeString;    // Strings used for logging
    static QtMsgType   s_msgTypeSorted[];  // required because QtMsgType has strange sorting

    QHash<QString, SCategory*> m_categories;  // categories
    QtMsgType                  m_logLevel;    // overall log level
    quint16          m_logLevelMask;          // overall log level mask, required because QtMsgType has strange sorting
    bool             m_consoleEnabled;        // output to console
    bool             m_fileEnabled;           // output to log file
    bool             m_queueEnabled;          // output to queue for JSON API
    bool             m_showSource;            // Show source file and line
    int              m_lastDay;               // Every day we create a new file
    int              m_maxQueueSize;          // Maximum Queue size
    QString          m_directory;             // For files
    QFile*           m_file;                  // File
    QQueue<SMessage> m_queue;                 // Queue
    QMutex           m_queueMutex;            // Locking for queue
};
