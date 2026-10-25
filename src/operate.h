/* Copyright (C) 2022  Dmitry Serov
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

#ifndef OPERATE_H
#define OPERATE_H


#include <optional>
#include <string>
#include <QStringList>
#include <QVector>

enum class charging_state {
    battery_charging,
    battery_discharging,
    battery_not_charging,
    battery_fully_charged,
    battery_unknown
};

enum class shift_mode {
    eco_mode,
    comfort_mode,
    sport_mode,
    turbo_mode,
    unknown_mode
};

enum class user_mode {
    performance_mode,
    balanced_mode,
    silent_mode,
    super_battery_mode,
    custom_mode,
    unknown_mode
};

enum class fan_mode {
    auto_fan_mode,
    silent_fan_mode,
    basic_fan_mode,
    advanced_fan_mode,
    unknown_fan_mode
};

class Operate {
public:
    Operate();
    void closeHelperApp() const;
    [[nodiscard]] bool isEcSysModuleLoaded() const;
    [[nodiscard]] bool isMsiEcLoaded() const;
    [[nodiscard]] bool loadEcSysModule() const;
    [[nodiscard]] bool updateEcData() const;
    void updateEcDataAsync() const;
    bool doProbe() const;
    [[nodiscard]] std::string getEcVersion() const;
    [[nodiscard]] std::string getEcBuild() const;
    [[nodiscard]] int getBatteryCharge() const;
    [[nodiscard]] int getBatteryThreshold() const;
    [[nodiscard]] charging_state getChargingStatus() const;
    [[nodiscard]] int getCpuTemp() const;
    [[nodiscard]] std::optional<int> getGpuTemp() const;
    [[nodiscard]] int getFan1Speed() const;
    [[nodiscard]] std::optional<int> getFan2Speed() const;
    [[nodiscard]] int getFan1Duty() const;
    [[nodiscard]] std::optional<int> getFan2Duty() const;
    [[nodiscard]] bool getFanCurveActive() const;
    [[nodiscard]] QVector<int> getFan1SpeedSettings() const;
    [[nodiscard]] QVector<int> getFan2SpeedSettings() const;
    [[nodiscard]] QVector<int> getFan1TempSettings() const;
    [[nodiscard]] QVector<int> getFan2TempSettings() const;

    [[nodiscard]] int getKeyboardBacklightMode() const;
    [[nodiscard]] bool getKeyboardBacklightEnabled() const;
    [[nodiscard]] int getKeyboardBrightness() const;
    [[nodiscard]] QString getUsbPowerShareState() const;
    [[nodiscard]] bool getQc3State() const;
    [[nodiscard]] bool getPanelState() const;
    [[nodiscard]] bool getWebCamState() const;
    [[nodiscard]] bool getWebCamBlockState() const;
    [[nodiscard]] bool getFnSuperSwapState() const;
    [[nodiscard]] bool getCoolerBoostState() const;
    [[nodiscard]] user_mode getUserMode() const;
    [[nodiscard]] fan_mode getFanMode() const;
    [[nodiscard]] QString getShiftModeName() const;
    [[nodiscard]] QString getFanModeName() const;
    [[nodiscard]] QStringList getAvailableShiftModeNames() const;
    [[nodiscard]] QStringList getAvailableFanModeNames() const;
    [[nodiscard]] QString getDriverName() const;
    [[nodiscard]] int getBatteryStartThreshold() const;

    bool setBatteryThreshold(int value) const;
    bool setKeyboardBacklightMode(int value) const;
    bool setKeyboardBacklightEnabled(bool enabled) const;
    bool setKeyboardBrightness(int value) const;
    bool setUsbPowerShareState(const QString &state) const;
    bool setQc3State(bool enabled) const;
    bool setPanelState(bool enabled) const;
    bool setWebCamState(bool enabled) const;
    bool setWebCamBlockState(bool enabled) const;
    bool setFnSuperSwapState(bool enabled) const;
    bool setCoolerBoostState(bool enabled) const;
    bool setUserMode(user_mode userMode) const;
    bool setFan1SpeedSettings(QVector<int> value) const;
    bool setFan2SpeedSettings(QVector<int> value) const;
    bool setFan1TempSettings(QVector<int> value) const;
    bool setFan2TempSettings(QVector<int> value) const;
    bool setFanMode(int value) const;
    bool setFanModeAdvanced(bool enabled) const;
    bool setFanModeByName(const QString &name) const;
    bool setShiftModeByName(const QString &name) const;
    bool setFanCurveActive(bool active) const;

    [[nodiscard]] int getValue(int address) const;
    void setValue(int address, int value) const;

    [[nodiscard]] bool isBatteryThresholdSupport() const;
    [[nodiscard]] bool isKeyboardBacklightModeSupport() const;
    [[nodiscard]] bool isKeyboardBacklightSupport() const;
    [[nodiscard]] bool isKeyboardBacklightEnableSupport() const;
    [[nodiscard]] bool isUsbPowerShareSupport() const;
    [[nodiscard]] bool isQc3Support() const;
    [[nodiscard]] bool isPanelSupport() const;
    [[nodiscard]] bool isWebCamOffSupport() const;
    [[nodiscard]] bool isWebCamBlockSupport() const;

    void loadSettings() const;
    void handleWakeEvent() const;

    void putSuperBatteryModeValue(bool enabled) const;
private:
    int detectFan1Address() const;
    int detectBatteryThresholdAddress() const;
    int detectFanModeAddress() const;
    int detectKeyboardBacklightAddress() const;
};

#endif // OPERATE_H
