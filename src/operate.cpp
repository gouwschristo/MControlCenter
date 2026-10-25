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

#include "operate.h"
#include "helper.h"
#include "msi-ec_helper.h"
#include "settings.h"

Helper helper;
MsiEcHelper msiEcHelper;

const int cpuTempAddress = 0x68;
const int gpuTempAddress = 0x80;
const int cpuFanAddress = 0x71;
const int gpuFanAddress = 0x89;
const int batteryChargeAddress = 0x42;
int batteryThresholdAddress;
const int batteryThresholdAddress_0xEF = 0xEF;
const int batteryThresholdAddress_0xD7 = 0xD7;

const int keyboardBacklightModeAddress = 0x2C;
const int keyboardBacklightAlwaysOn = 0x00;
const int keyboardBacklightAutoTurnOff = 0x08;

int keyboardBacklightAddress;
const int keyboardBacklightAddress_0xD3 = 0xD3;
const int keyboardBacklightAddress_0xF3 = 0xF3;
const int keyboardBacklight0ff = 0x80;
const int keyboardBacklightLow = 0x81;
const int keyboardBacklightMid = 0x82;
const int keyboardBacklightHigh = 0x83;

const int usbPowerShareAddress = 0xBF;
const int usbPowerShareOff = 0x08;
const int usbPowerShareOn = 0x28;

const int coolerBoostAddress = 0x98;

const int fnSuperSwapAddress = 0xE8;

int fan1Address;
const int fan1Address_0xC9 = 0xC9;
const int fan1Address_0xCD = 0xCD;
const int fan2Address = 0xCB;

const int fan1SpeedSettingStartAddress = 0x72;
const int fan2SpeedSettingStartAddress = 0x8A;
const int fanSpeedSettingsCount = 7;
const int fan1TempSettingStartAddress = 0x6A;
const int fan2TempSettingStartAddress = 0x82;
const int fanTempSettingsCount = fanSpeedSettingsCount - 1;

int fanModeAddress;
const int fanModeAddress_0xD4 = 0xD4;
const int fanModeAddress_0xF4 = 0xF4;
const int shiftModeAddress = 0xF2;
const int fanModeAuto = 0x0D;
const int fanModeSilent = 0x1D;
const int fanModeBasic = 0x4D;
const int fanModeAdvanced = 0x8D;

const QString settingsGroup = "Settings/";

static QString curveToString(const QVector<int> &points) {
    QStringList out;
    for (int value : points)
        out << QString::number(value);
    return out.join(",");
}

Operate::Operate() = default;

void Operate::closeHelperApp() const {
    helper.quit();
}

bool Operate::isEcSysModuleLoaded() const {
    return helper.isEcSysModuleLoaded();
}

bool Operate::isMsiEcLoaded() const {
    return msiEcHelper.isMsiEcModuleLoaded();
}

bool Operate::loadEcSysModule() const {
    return helper.loadEcSysModule();
}

bool Operate::updateEcData() const {
    return helper.updateData();
}

void Operate::updateEcDataAsync() const {
    helper.updateDataAsync();
}

bool Operate::doProbe() const {
    fan1Address = detectFan1Address();
    batteryThresholdAddress = detectBatteryThresholdAddress();
    fanModeAddress = detectFanModeAddress();
    keyboardBacklightAddress = detectKeyboardBacklightAddress();

    return true;
}

std::string Operate::getEcVersion() const {
    if (msiEcHelper.isMsiEcModuleLoaded())
        return msiEcHelper.getFWVersion().toStdString();
    return helper.getValues(160, 12).toStdString();
}

std::string Operate::getEcBuild() const {
    if (msiEcHelper.isMsiEcModuleLoaded())
        return msiEcHelper.getFWReleaseDate().toStdString();
    std::string s = helper.getValues(172, 16).toStdString();
    if (s.size() < 16)
        return s;
    return s.substr(0, 2) + "/" + s.substr(2, 2) + "/" + s.substr(4, 4) + " " + s.substr(8, 8);
}

int Operate::getBatteryCharge() const {
    if (msiEcHelper.hasBatteryCapacity())
        return msiEcHelper.getBatteryCapacity();
    return helper.getValue(batteryChargeAddress) - 1;
}

int Operate::getBatteryThreshold() const {
    if (msiEcHelper.hasBatteryEndThreshold())
        return msiEcHelper.getBatteryEndThreshold();
    return helper.getValue(batteryThresholdAddress) - 128;
}

charging_state Operate::getChargingStatus() const {
    if (msiEcHelper.hasBatteryStatus()) {
        QString status = msiEcHelper.getBatteryStatus();
        if (status == "Charging")
            return charging_state::battery_charging;
        else if (status == "Discharging")
            return charging_state::battery_discharging;
        else if (status == "Not charging")
            return charging_state::battery_not_charging;
        else if (status == "Full")
            return charging_state::battery_fully_charged;
        // Fully charged no power doesn't exist in the kernel documentation
        else
            return charging_state::battery_unknown;
    }
    return charging_state::battery_unknown;
}

int Operate::getCpuTemp() const {
    if (msiEcHelper.hasCPURealtimeTemperature())
        return msiEcHelper.getCPURealtimeTemperature();
    return helper.getValue(cpuTempAddress);
}

std::optional<int> Operate::getGpuTemp() const {
    if (msiEcHelper.isMsiEcModuleLoaded())
        if (!msiEcHelper.hasGPURealtimeTemperature())
            return std::nullopt;
        else
            return msiEcHelper.getGPURealtimeTemperature();
    else
        return helper.getValue(gpuTempAddress);
}

int Operate::getFan1Speed() const {
    if (msiEcHelper.hasFanRpm())
        return msiEcHelper.getCPUFanRpm().value_or(0);
    // Read 2 bytes (big-endian)
    int value0 = helper.getValue(fan1Address);
    int value1 = helper.getValue(fan1Address - 1);
    int value = (value1 << 8) | value0;
    if (value > 0)
        return 480000 / value;
    return value;
}

std::optional<int> Operate::getFan2Speed() const {
    if (msiEcHelper.hasFanRpm())
        return msiEcHelper.getGPUFanRpm();
    if (msiEcHelper.isMsiEcModuleLoaded() && !msiEcHelper.hasGPURealtimeFanSpeed())
        return std::nullopt;
    // Read 2 bytes (big-endian)
    auto value0 = helper.getOptionalValue(fan2Address);
    auto value1 = helper.getOptionalValue(fan2Address - 1);
    if (!value0.has_value() || !value1.has_value())
        return std::nullopt;
    int value = (value1.value() << 8) | value0.value();
    if (value > 0)
        return 480000 / value;
    return value;
}

int Operate::getFan1Duty() const {
    if (msiEcHelper.hasCPURealtimeFanSpeed())
        return msiEcHelper.getCPURealtimeFanSpeed();
    return helper.getValue(cpuFanAddress);
}

std::optional<int> Operate::getFan2Duty() const {
    if (msiEcHelper.hasGPURealtimeFanSpeed())
        return msiEcHelper.getGPURealtimeFanSpeed();
    auto value = helper.getOptionalValue(gpuFanAddress);
    return value ? std::optional<int>(value.value()) : std::nullopt;
}

bool Operate::getFanCurveActive() const {
    if (msiEcHelper.hasFanCurve())
        return msiEcHelper.getFanCurveActive();
    return getFanMode() == fan_mode::advanced_fan_mode;
}

QVector<int> Operate::getFan1SpeedSettings() const {
    QVector<int> a;
    if (msiEcHelper.hasFanCurve()) {
        a.push_back(msiEcHelper.getCPUFanCurveIdle());
        const QStringList parts = msiEcHelper.getCPUFanCurve().split(':');
        if (parts.size() == 2) {
            for (const QString &s : parts[1].split(','))
                a.push_back(s.toInt());
        }
        if (a.size() == fanSpeedSettingsCount)
            return a;
        a.clear();
    }
    for (int i = 0; i < fanSpeedSettingsCount; i++)
        a.push_back(helper.getValue(fan1SpeedSettingStartAddress + i));
    return a;
}

QVector<int> Operate::getFan2SpeedSettings() const {
    QVector<int> a;
    if (msiEcHelper.hasFanCurve()) {
        a.push_back(msiEcHelper.getGPUFanCurveIdle());
        const QStringList parts = msiEcHelper.getGPUFanCurve().split(':');
        if (parts.size() == 2) {
            for (const QString &s : parts[1].split(','))
                a.push_back(s.toInt());
        }
        if (a.size() == fanSpeedSettingsCount)
            return a;
        a.clear();
    }
    for (int i = 0; i < fanSpeedSettingsCount; i++)
        a.push_back(helper.getValue(fan2SpeedSettingStartAddress + i));
    return a;
}

QVector<int> Operate::getFan1TempSettings() const {
    QVector<int> a;
    if (msiEcHelper.hasFanCurve()) {
        const QStringList parts = msiEcHelper.getCPUFanCurve().split(':');
        if (parts.size() == 2) {
            for (const QString &s : parts[0].split(','))
                a.push_back(s.toInt());
        }
        if (a.size() == fanTempSettingsCount)
            return a;
        a.clear();
    }
    for (int i = 0; i < fanTempSettingsCount; i++)
        a.push_back(helper.getValue(fan1TempSettingStartAddress + i));
    return a;
}

QVector<int> Operate::getFan2TempSettings() const {
    QVector<int> a;
    if (msiEcHelper.hasFanCurve()) {
        const QStringList parts = msiEcHelper.getGPUFanCurve().split(':');
        if (parts.size() == 2) {
            for (const QString &s : parts[0].split(','))
                a.push_back(s.toInt());
        }
        if (a.size() == fanTempSettingsCount)
            return a;
        a.clear();
    }
    for (int i = 0; i < fanTempSettingsCount; i++)
        a.push_back(helper.getValue(fan2TempSettingStartAddress + i));
    return a;
}

int Operate::getKeyboardBacklightMode() const {
    if (msiEcHelper.hasKeyboardBacklightMode())
        return msiEcHelper.getKeyboardBacklightMode().value_or(0);
    if (helper.getValue(keyboardBacklightModeAddress) == keyboardBacklightAutoTurnOff)
        return 1;
    return 0;
}

bool Operate::getKeyboardBacklightEnabled() const {
    if (msiEcHelper.hasKeyboardBacklightEnable())
        return msiEcHelper.getKeyboardBacklightEnable();
    return getKeyboardBrightness() != 0;
}

int Operate::getKeyboardBrightness() const {
    if (msiEcHelper.hasKeyboardBacklightBrightness())
        return msiEcHelper.getKeyboardBacklightBrightness();
    int value = helper.getValue(keyboardBacklightAddress);
    switch (value) {
        case keyboardBacklight0ff:
            return 0;
        case keyboardBacklightLow:
            return 1;
        case keyboardBacklightMid:
            return 2;
        case keyboardBacklightHigh:
            return 3;
        default:
            return 0;
    }
}

QString Operate::getUsbPowerShareState() const {
    if (msiEcHelper.hasUsbPowerShare())
        return msiEcHelper.getUsbPowerShare();
    int value = helper.getValue(usbPowerShareAddress);
    if (value == usbPowerShareOn)
        return "state0";
    if (value == usbPowerShareOff)
        return "off";
    return "";
}

bool Operate::getQc3State() const {
    if (msiEcHelper.hasQc3())
        return msiEcHelper.getQc3();
    return false;
}

bool Operate::getPanelState() const {
    if (msiEcHelper.hasPanel())
        return msiEcHelper.getPanel();
    return false;
}

bool Operate::getWebCamState() const {
    if (msiEcHelper.hasWebcam())
        return msiEcHelper.getWebcam();
    return false;
}

bool Operate::getWebCamBlockState() const {
    if (msiEcHelper.hasWebcamBlock())
        return msiEcHelper.getWebcamBlock();
    return false;
}

bool Operate::getFnSuperSwapState() const {
    if (msiEcHelper.hasFnWinSwap())
        return msiEcHelper.getFnWinSwap();
    if (helper.getValue(fnSuperSwapAddress) / 16 % 2 != 0)
        return true;
    return false;
}

bool Operate::getCoolerBoostState() const {
    if (msiEcHelper.hasCoolerBoost())
        return msiEcHelper.getCoolerBoost();
    if (helper.getValue(coolerBoostAddress) > 127)
        return true;
    return false;
}

bool Operate::getSuperBatteryState() const {
    if (msiEcHelper.hasSuperBattery())
        return msiEcHelper.getSuperBattery();
    return false;
}

user_mode Operate::getUserMode() const {
    if (msiEcHelper.hasShiftMode()) {
        shift_mode shiftMode = msiEcHelper.getShiftMode();
        fan_mode fanMode = getFanMode();
        switch (shiftMode) {
            case shift_mode::eco_mode:
                return fanMode == fan_mode::auto_fan_mode ?
                       user_mode::super_battery_mode : user_mode::custom_mode;
            case shift_mode::comfort_mode:
                if (fanMode == fan_mode::silent_fan_mode)
                    return user_mode::silent_mode;
                if (fanMode == fan_mode::auto_fan_mode)
                    return user_mode::balanced_mode;
                return user_mode::custom_mode;
            case shift_mode::turbo_mode:
                return fanMode == fan_mode::auto_fan_mode ?
                       user_mode::performance_mode : user_mode::custom_mode;
            case shift_mode::sport_mode:
            default:
                return user_mode::custom_mode;
        }
    }
    return user_mode::unknown_mode;
}

fan_mode Operate::getFanMode() const {
    if (msiEcHelper.hasFanMode())
        return msiEcHelper.getFanMode();
    switch (helper.getValue(fanModeAddress)) {
        case fanModeAuto:
            return fan_mode::auto_fan_mode;
        case fanModeSilent:
            return fan_mode::silent_fan_mode;
        case fanModeBasic:
            return fan_mode::basic_fan_mode;
        case fanModeAdvanced:
            return fan_mode::advanced_fan_mode;
        default:
            return fan_mode::unknown_fan_mode;
    }
}

QString Operate::getShiftModeName() const {
    if (msiEcHelper.hasShiftMode())
        return msiEcHelper.getShiftModeName();
    switch (helper.getValue(shiftModeAddress)) {
        case 0xc0: return "sport";
        case 0xc1: return "comfort";
        case 0xc2: return "eco";
        case 0xc3: return "user";
        case 0xc4: return "turbo";
        default:   return "unknown";
    }
}

QString Operate::getFanModeName() const {
    if (msiEcHelper.hasFanMode())
        return msiEcHelper.getFanModeName();
    switch (helper.getValue(fanModeAddress)) {
        case fanModeAuto:     return "auto";
        case fanModeSilent:   return "silent";
        case fanModeBasic:    return "basic";
        case fanModeAdvanced: return "advanced";
        default:              return "unknown";
    }
}

QStringList Operate::getAvailableShiftModeNames() const {
    if (msiEcHelper.hasShiftMode())
        return msiEcHelper.getAvailableShiftModesString().split('\n', Qt::SkipEmptyParts);
    return {"eco", "comfort", "sport", "turbo"};
}

QStringList Operate::getAvailableFanModeNames() const {
    if (msiEcHelper.hasFanMode())
        return msiEcHelper.getAvailableFanModesString().split('\n', Qt::SkipEmptyParts);
    return {"auto", "silent", "basic", "advanced"};
}

QString Operate::getDriverName() const {
    const QString path = msiEcHelper.getMsiEcPath();
    return path.isEmpty() ? QString("unknown") : path.section('/', -1);
}

int Operate::getBatteryStartThreshold() const {
    if (msiEcHelper.hasBatteryStartThreshold())
        return msiEcHelper.getBatteryStartThreshold();
    return qMax(getBatteryThreshold() - 10, 0);
}

bool Operate::setBatteryThreshold(int value) const {
    if (msiEcHelper.hasBatteryEndThreshold())
        return msiEcHelper.setBatteryEndThreshold(value);
    if (value != getBatteryThreshold())
        helper.putValue(batteryThresholdAddress, value + 128);
    return true;
}

bool Operate::setKeyboardBacklightMode(int value) const {
    if (msiEcHelper.hasKeyboardBacklightMode())
        return msiEcHelper.setKeyboardBacklightMode(value);
    int resValue = keyboardBacklightAlwaysOn;
    if (value == 1)
        resValue = keyboardBacklightAutoTurnOff;
    helper.putValue(keyboardBacklightModeAddress, resValue);
    return true;
}

bool Operate::setKeyboardBacklightEnabled(bool enabled) const {
    if (msiEcHelper.hasKeyboardBacklightEnable())
        return msiEcHelper.setKeyboardBacklightEnable(enabled);
    if (!enabled)
        return setKeyboardBrightness(0);
    return true;
}

bool Operate::setKeyboardBrightness(int value) const {
    if (msiEcHelper.hasKeyboardBacklightBrightness())
        return msiEcHelper.setKeyboardBacklightBrightness(value);
    int resValue;
    switch (value) {
        case 0:
            resValue = keyboardBacklight0ff;
            break;
        case 1:
            resValue = keyboardBacklightLow;
            break;
        case 2:
            resValue = keyboardBacklightMid;
            break;
        case 3:
            resValue = keyboardBacklightHigh;
            break;
        default:
            resValue = keyboardBacklight0ff;
    }
    helper.putValue(keyboardBacklightAddress, resValue);
    return true;
}

bool Operate::setUsbPowerShareState(const QString &state) const {
    if (msiEcHelper.hasUsbPowerShare())
        return msiEcHelper.setUsbPowerShare(state);
    int value = (state == "off") ? usbPowerShareOff : usbPowerShareOn;
    helper.putValue(usbPowerShareAddress, value);
    return true;
}

bool Operate::setQc3State(bool enabled) const {
    if (msiEcHelper.hasQc3())
        return msiEcHelper.setQc3(enabled);
    return false;
}

bool Operate::setPanelState(bool enabled) const {
    if (msiEcHelper.hasPanel())
        return msiEcHelper.setPanel(enabled);
    return false;
}

bool Operate::setWebCamState(bool enabled) const {
    if (msiEcHelper.hasWebcam())
        return msiEcHelper.setWebcam(enabled);
    return false;
}

bool Operate::setWebCamBlockState(bool enabled) const {
    if (msiEcHelper.hasWebcamBlock())
        return msiEcHelper.setWebcamBlock(enabled);
    return false;
}

bool Operate::setFnSuperSwapState(bool enabled) const {
    Settings::setValue(settingsGroup + "FnSuperSwap", enabled);
    if (msiEcHelper.hasFnWinSwap())
        return msiEcHelper.setFnWinSwap(enabled);
    if (getFnSuperSwapState() == enabled)
        return true;
    int value = helper.getValue(fnSuperSwapAddress) + (enabled ? 16 : -16);
    helper.putValue(fnSuperSwapAddress, value);
    return true;
}

bool Operate::setCoolerBoostState(bool enabled) const {
    if (msiEcHelper.hasCoolerBoost())
        return msiEcHelper.setCoolerBoost(enabled);
    int value = helper.getValue(coolerBoostAddress);
    if (enabled && (value < 128))
        helper.putValue(coolerBoostAddress, value + 128);
    if (!enabled && (value > 127))
        helper.putValue(coolerBoostAddress, value - 128);
    return true;
}

bool Operate::setSuperBatteryState(bool enabled) const {
    if (msiEcHelper.hasSuperBattery())
        return msiEcHelper.setSuperBattery(enabled);
    return true;
}

bool Operate::setUserMode(user_mode userMode) const {
    shift_mode shiftMode = shift_mode::comfort_mode;
    fan_mode fanMode = fan_mode::auto_fan_mode;
    bool superBattery = false;
    QString userModeStr;

    switch (userMode) {
        case user_mode::balanced_mode:
            userModeStr = "balanced_mode";
            break;
        case user_mode::performance_mode:
            shiftMode = shift_mode::turbo_mode;
            userModeStr = "performance_mode";
            break;
        case user_mode::silent_mode:
            fanMode = fan_mode::silent_fan_mode;
            userModeStr = "silent_mode";
            break;
        case user_mode::super_battery_mode:
            shiftMode = shift_mode::eco_mode;
            superBattery = true;
            userModeStr = "super_battery_mode";
            break;
        case user_mode::custom_mode:
        case user_mode::unknown_mode:
        default:
            return true;
    }

    bool ok = true;
    if (msiEcHelper.hasShiftMode())
        ok &= msiEcHelper.setShiftMode(shiftMode);
    if (msiEcHelper.hasFanMode())
        ok &= msiEcHelper.setFanMode(fanMode);
    if (msiEcHelper.hasSuperBattery())
        ok &= msiEcHelper.setSuperBattery(superBattery);

    // A preset uses the EC fan mode, not the custom curve.
    if (ok && msiEcHelper.hasFanCurve())
        ok &= setFanModeAdvanced(false);

    if (ok)
        Settings::setValue(settingsGroup + "UserMode", userModeStr);
    return ok;
}

bool Operate::setFan1SpeedSettings(QVector<int> value) const {
    if (value.size() != fanSpeedSettingsCount)
        return false;

    if (msiEcHelper.hasFanCurve()) {
        const QVector<int> temps = getFan1TempSettings();
        if (temps.size() != fanTempSettingsCount)
            return false;
        const QString curve = curveToString(temps) + ":" + curveToString(value.mid(1));
        bool ok = msiEcHelper.setCPUFanCurve(curve);
        if (msiEcHelper.hasFanCurveIdle())
            ok = msiEcHelper.setCPUFanCurveIdle(value.first()) && ok;
        if (ok)
            Settings::setValue(settingsGroup + QString("fan1SpeedSettings"), value);
        return ok;
    }

    for (int i = 0; i < value.size(); i++)
        helper.putValue(fan1SpeedSettingStartAddress + i, value[i]);
    Settings::setValue(settingsGroup + QString("fan1SpeedSettings"), value);
    return true;
}

bool Operate::setFan2SpeedSettings(QVector<int> value) const {
    if (value.size() != fanSpeedSettingsCount)
        return false;

    if (msiEcHelper.hasFanCurve()) {
        const QVector<int> temps = getFan2TempSettings();
        if (temps.size() != fanTempSettingsCount)
            return false;
        const QString curve = curveToString(temps) + ":" + curveToString(value.mid(1));
        bool ok = msiEcHelper.setGPUFanCurve(curve);
        if (msiEcHelper.hasFanCurveIdle())
            ok = msiEcHelper.setGPUFanCurveIdle(value.first()) && ok;
        if (ok)
            Settings::setValue(settingsGroup + QString("fan2SpeedSettings"), value);
        return ok;
    }

    for (int i = 0; i < value.size(); i++)
        helper.putValue(fan2SpeedSettingStartAddress + i, value[i]);
    Settings::setValue(settingsGroup + QString("fan2SpeedSettings"), value);
    return true;
}

bool Operate::setFan1TempSettings(QVector<int> value) const {
    if (value.size() != fanTempSettingsCount)
        return false;

    if (msiEcHelper.hasFanCurve()) {
        const QVector<int> speeds = getFan1SpeedSettings();
        if (speeds.size() != fanSpeedSettingsCount)
            return false;
        const QString curve = curveToString(value) + ":" + curveToString(speeds.mid(1));
        const bool ok = msiEcHelper.setCPUFanCurve(curve);
        if (ok)
            Settings::setValue(settingsGroup + QString("fan1TempSettings"), value);
        return ok;
    }

    for (int i = 0; i < value.size(); i++)
        helper.putValue(fan1TempSettingStartAddress + i, value[i]);
    Settings::setValue(settingsGroup + QString("fan1TempSettings"), value);
    return true;
}

bool Operate::setFan2TempSettings(QVector<int> value) const {
    if (value.size() != fanTempSettingsCount)
        return false;

    if (msiEcHelper.hasFanCurve()) {
        const QVector<int> speeds = getFan2SpeedSettings();
        if (speeds.size() != fanSpeedSettingsCount)
            return false;
        const QString curve = curveToString(value) + ":" + curveToString(speeds.mid(1));
        const bool ok = msiEcHelper.setGPUFanCurve(curve);
        if (ok)
            Settings::setValue(settingsGroup + QString("fan2TempSettings"), value);
        return ok;
    }

    for (int i = 0; i < value.size(); i++)
        helper.putValue(fan2TempSettingStartAddress + i, value[i]);
    Settings::setValue(settingsGroup + QString("fan2TempSettings"), value);
    return true;
}

bool Operate::setFanMode(int value) const {
    helper.putValue(fanModeAddress, value);
    return true;
}

bool Operate::setFanModeAdvanced(bool enabled) const {
    bool ok;
    if (msiEcHelper.hasFanCurve()) {
        ok = setFanCurveActive(enabled);
    } else {
        helper.putValue(fanModeAddress, enabled ? fanModeAdvanced : fanModeAuto);
        ok = true;
    }
    if (ok)
        Settings::setValue(settingsGroup + "fanModeAdvanced", enabled);
    return ok;
}

bool Operate::setFanCurveActive(bool active) const {
    if (msiEcHelper.hasFanCurve())
        return msiEcHelper.setFanCurveActive(active);
    return setFanModeAdvanced(active);
}

bool Operate::setFanModeByName(const QString &name) const {
    if (msiEcHelper.hasFanMode()) {
        const bool ok = msiEcHelper.setFanModeName(name);
        // Keep the saved curve state in sync with the EC. "basic" and the
        // user curve share the same activation bit, so read it back.
        if (ok && msiEcHelper.hasFanCurve())
            Settings::setValue(settingsGroup + "fanModeAdvanced",
                               msiEcHelper.getFanCurveActive());
        return ok;
    }

    int value;
    if (name == "auto")
        value = fanModeAuto;
    else if (name == "silent")
        value = fanModeSilent;
    else if (name == "basic")
        value = fanModeBasic;
    else if (name == "advanced")
        value = fanModeAdvanced;
    else
        return false;

    helper.putValue(fanModeAddress, value);
    return true;
}

bool Operate::setShiftModeByName(const QString &name) const {
    if (msiEcHelper.hasShiftMode())
        return msiEcHelper.setShiftModeName(name);

    int value;
    if (name == "sport")
        value = 0xc0;
    else if (name == "comfort")
        value = 0xc1;
    else if (name == "eco")
        value = 0xc2;
    else if (name == "user")
        value = 0xc3;
    else if (name == "turbo")
        value = 0xc4;
    else
        return false;

    helper.putValue(shiftModeAddress, value);
    return true;
}

int Operate::getValue(int address) const {
    helper.updateData();
    return helper.getValue(address);
}

void Operate::setValue(int address, int value) const {
    helper.putValue(address, value);
}

bool Operate::isBatteryThresholdSupport() const {
    return msiEcHelper.hasBatteryEndThreshold() || batteryThresholdAddress != 0;
}

bool Operate::isKeyboardBacklightModeSupport() const {
    if (msiEcHelper.hasKeyboardBacklightMode())
        return true;

    // Older models expose the mode through the raw 0xD3 backlight byte.
    return keyboardBacklightAddress == keyboardBacklightAddress_0xD3;
}

bool Operate::isKeyboardBacklightSupport() const {
    if (msiEcHelper.hasKeyboardBacklightBrightness())
        return true;
    return false;
}

bool Operate::isKeyboardBacklightEnableSupport() const {
    return msiEcHelper.hasKeyboardBacklightEnable();
}

bool Operate::isUsbPowerShareSupport() const {
    if (msiEcHelper.hasUsbPowerShare())
        return true;
    return (helper.getValue(usbPowerShareAddress) == usbPowerShareOff ||
            helper.getValue(usbPowerShareAddress) == usbPowerShareOn);
}

bool Operate::isQc3Support() const {
    return msiEcHelper.hasQc3();
}

bool Operate::isPanelSupport() const {
    return msiEcHelper.hasPanel();
}

bool Operate::isWebCamOffSupport() const {
    return msiEcHelper.hasWebcam() || msiEcHelper.hasWebcamBlock();
}

bool Operate::isWebCamBlockSupport() const {
    return msiEcHelper.hasWebcamBlock();
}

void Operate::loadSettings() const {
    Settings s;

    // Capture this before applying a preset: setUserMode() clears the curve
    // and would otherwise overwrite the saved state we are about to restore.
    const bool hasFanAdvanced = s.isValueExist(settingsGroup + "fanModeAdvanced");
    const bool fanAdvanced = hasFanAdvanced &&
                             s.getValue(settingsGroup + "fanModeAdvanced").toBool();

    if (getUserMode() != user_mode::unknown_mode && s.isValueExist(settingsGroup + "UserMode")) {
        QString value = s.getValue(settingsGroup + "UserMode").toString();
        if (value == "balanced_mode")
            setUserMode(user_mode::balanced_mode);
        else if (value == "performance_mode")
            setUserMode(user_mode::performance_mode);
        else if (value == "silent_mode")
            setUserMode(user_mode::silent_mode);
        else if (value == "super_battery_mode")
            setUserMode(user_mode::super_battery_mode);
    }

    if (s.isValueExist(settingsGroup + "FnSuperSwap"))
        setFnSuperSwapState(s.getValue(settingsGroup + "FnSuperSwap").toBool());
    if (isUsbPowerShareSupport() && s.isValueExist(settingsGroup + "UsbPowerShare")) {
        QString state = s.getValue(settingsGroup + "UsbPowerShare").toString();
        if (state == "true")
            state = "state0";
        else if (state == "false")
            state = "off";
        setUsbPowerShareState(state);
    }

    if (s.isValueExist(settingsGroup + "fan1SpeedSettings"))
        setFan1SpeedSettings(s.getValueVector(settingsGroup + "fan1SpeedSettings"));
    if (s.isValueExist(settingsGroup + "fan2SpeedSettings"))
        setFan2SpeedSettings(s.getValueVector(settingsGroup + "fan2SpeedSettings"));
    if (s.isValueExist(settingsGroup + "fan1TempSettings"))
        setFan1TempSettings(s.getValueVector(settingsGroup + "fan1TempSettings"));
    if (s.isValueExist(settingsGroup + "fan2TempSettings"))
        setFan2TempSettings(s.getValueVector(settingsGroup + "fan2TempSettings"));
    if (hasFanAdvanced)
        setFanModeAdvanced(fanAdvanced);
}

void Operate::handleWakeEvent() const {
    Settings s;
    if (s.isValueExist(settingsGroup + "fanModeAdvanced"))
        setFanModeAdvanced(s.getValue(settingsGroup + "fanModeAdvanced").toBool());
}

int Operate::detectFan1Address() const {
    int value_0xC9 = helper.getValue(fan1Address_0xC9);
    if (int value_0xCD = helper.getValue(fan1Address_0xCD); value_0xCD > 0)
        return fan1Address_0xCD;
    if (value_0xC9 > 0 && value_0xC9 < 50)
        return fan1Address_0xCD;
    return fan1Address_0xC9;
}

int Operate::detectBatteryThresholdAddress() const {
    if (128 <= helper.getValue(batteryThresholdAddress_0xEF) && helper.getValue(batteryThresholdAddress_0xEF) <= 228)
        return batteryThresholdAddress_0xEF;
    if (128 <= helper.getValue(batteryThresholdAddress_0xD7) && helper.getValue(batteryThresholdAddress_0xD7) <= 228)
        return batteryThresholdAddress_0xD7;
    return 0;
}

int Operate::detectFanModeAddress() const {
    if (int fanModeValue = helper.getValue(fanModeAddress_0xD4);
            fanModeValue == fanModeAuto ||
            fanModeValue == fanModeSilent ||
            fanModeValue == fanModeBasic ||
            fanModeValue == fanModeAdvanced)
        return fanModeAddress_0xD4;
    return fanModeAddress_0xF4;
}

int Operate::detectKeyboardBacklightAddress() const {
    int value_0xD3 = helper.getValue(keyboardBacklightAddress_0xD3);
    if (value_0xD3 == keyboardBacklight0ff ||
        value_0xD3 == keyboardBacklightLow ||
        value_0xD3 == keyboardBacklightMid ||
        value_0xD3 == keyboardBacklightHigh) {
        return keyboardBacklightAddress_0xD3;
    }
    
    int value_0xF3 = helper.getValue(keyboardBacklightAddress_0xF3);
    if (value_0xF3 == keyboardBacklight0ff ||
        value_0xF3 == keyboardBacklightLow ||
        value_0xF3 == keyboardBacklightMid ||
        value_0xF3 == keyboardBacklightHigh) {
        return keyboardBacklightAddress_0xF3;
    }

    return -1;
}
