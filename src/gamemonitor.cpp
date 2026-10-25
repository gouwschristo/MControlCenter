/* Copyright (C) 2026 Dmitry Serov
 *
 * This file is part of MControlCenter.
 *
 * MControlCenter is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * MControlCenter is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with MControlCenter. If not, see <https://www.gnu.org/licenses/>.
 */

#include "gamemonitor.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>

#include <unistd.h>

namespace {

QString environmentValue(const QByteArray &environ, const char *name) {
    const QByteArray prefix = QByteArray(name) + '=';
    for (const QByteArray &entry : environ.split('\0')) {
        if (entry.startsWith(prefix))
            return QString::fromUtf8(entry.mid(prefix.size())).trimmed();
    }
    return {};
}

QString processName(const QString &procPath) {
    QFile file(procPath + "/comm");
    if (!file.open(QIODevice::ReadOnly))
        return {};
    return QString::fromUtf8(file.readAll()).trimmed();
}

} // namespace

GameMonitor::GameMonitor(QObject *parent) : QObject(parent) {
    timer.setInterval(2000);
    connect(&timer, &QTimer::timeout, this, &GameMonitor::scan);
}

void GameMonitor::start() {
    if (timer.isActive())
        return;
    scan();
    timer.start();
}

void GameMonitor::stop() {
    timer.stop();
    if (!active)
        return;
    active = false;
    description.clear();
    emit gameStateChanged(false, {});
}

QString GameMonitor::detectGame() const {
    const QDir proc("/proc");
    const QStringList entries = proc.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
    const qint64 selfPid = QCoreApplication::applicationPid();
    const uint uid = getuid();

    for (const QString &entry : entries) {
        bool isPid = false;
        const qint64 pid = entry.toLongLong(&isPid);
        if (!isPid || pid == selfPid)
            continue;

        const QString procPath = "/proc/" + entry;
        const QFileInfo procInfo(procPath);
        if (!procInfo.exists() || procInfo.ownerId() != uid)
            continue;

        // Steam's own client processes may carry SteamAppId=0; ignore them
        // before reading the larger environ file.
        const QString comm = processName(procPath);
        if (comm == "steam" || comm.startsWith("steamwebhelper"))
            continue;

        QFile environFile(procPath + "/environ");
        if (!environFile.open(QIODevice::ReadOnly))
            continue;

        const QByteArray environ = environFile.readAll();
        if (environ.isEmpty())
            continue;

        const QString steamGameId = environmentValue(environ, "SteamGameId");
        if (!steamGameId.isEmpty() && steamGameId != "0")
            return QStringLiteral("Steam %1").arg(steamGameId);

        const QString steamAppId = environmentValue(environ, "SteamAppId");
        if (!steamAppId.isEmpty() && steamAppId != "0")
            return QStringLiteral("Steam %1").arg(steamAppId);

        const QString lutrisGame = environmentValue(environ, "LUTRIS_GAME_UUID");
        if (!lutrisGame.isEmpty())
            return QStringLiteral("Lutris %1").arg(lutrisGame);
    }

    return {};
}

void GameMonitor::scan() {
    const QString game = detectGame();
    const bool nowActive = !game.isEmpty();

    if (nowActive == active) {
        if (nowActive)
            description = game;
        return;
    }

    active = nowActive;
    description = game;
    emit gameStateChanged(active, description);
}
