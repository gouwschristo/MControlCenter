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

#ifndef GAMEMONITOR_H
#define GAMEMONITOR_H

#include <QObject>
#include <QString>
#include <QTimer>

/**
 * Detects whether a game process launched by a supported launcher is running.
 *
 * Steam exports SteamGameId/SteamAppId in the environment of game processes;
 * Lutris exports LUTRIS_GAME_UUID. The monitor scans /proc for these markers so
 * MCC does not need a per-game wrapper or an external GameMode service.
 */
class GameMonitor : public QObject {
Q_OBJECT

public:
    explicit GameMonitor(QObject *parent = nullptr);

    void start();
    void stop();

    [[nodiscard]] bool isGameActive() const { return active; }
    [[nodiscard]] QString gameDescription() const { return description; }

signals:
    void gameStateChanged(bool active, const QString &description);

private slots:
    void scan();

private:
    [[nodiscard]] QString detectGame() const;

    QTimer timer;
    bool active = false;
    QString description;
};

#endif // GAMEMONITOR_H
