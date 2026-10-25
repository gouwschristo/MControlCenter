/* Copyright (C) 2022  Jérôme Lécuyer, Dmitry Serov
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

#ifndef MSI_EC_HELPER_H
#define MSI_EC_HELPER_H

#include "operate.h"
#include <QtCore/QObject>
#include <QtDBus/QDBusInterface>

class MsiEcHelper : public QObject {
    Q_OBJECT
public:
    MsiEcHelper();

    [[nodiscard]] bool isMsiEcModuleLoaded();

    // webcam on/off
    [[nodiscard]] bool hasWebcam() const;
    [[nodiscard]] bool getWebcam() const;
    [[nodiscard]] bool setWebcam(bool enable) const;

    // webcam_block on/off
    [[nodiscard]] bool hasWebcamBlock() const;
    [[nodiscard]] bool getWebcamBlock() const;
    [[nodiscard]] bool setWebcamBlock(bool enable) const;

    // fn_win_swap swap/no swap
    [[nodiscard]] bool hasFnWinSwap() const;
    [[nodiscard]] bool getFnWinSwap() const;
    [[nodiscard]] bool setFnWinSwap(bool swap) const;

    // cooler_boost
    [[nodiscard]] bool hasCoolerBoost() const;
    [[nodiscard]] bool getCoolerBoost() const;
    [[nodiscard]] bool setCoolerBoost(bool enable) const;

    // shift_mode & available_shift_modes
    [[nodiscard]] bool hasShiftMode() const;
    [[nodiscard]] QList<shift_mode> getAvailableShiftModes() const;
    [[nodiscard]] QString getAvailableShiftModesString() const;
    [[nodiscard]] shift_mode getShiftMode() const;
    [[nodiscard]] QString getShiftModeName() const;
    [[nodiscard]] bool setShiftMode(shift_mode mode) const;
    [[nodiscard]] bool setShiftModeName(QString mode) const;

    // super_battery
    [[nodiscard]] bool hasSuperBattery() const;
    [[nodiscard]] bool getSuperBattery() const;
    [[nodiscard]] bool setSuperBattery(bool enable) const;

    // fan_mode & available_fan_modes
    [[nodiscard]] bool hasFanMode() const;
    [[nodiscard]] QList<fan_mode> getAvailableFanModes() const;
    [[nodiscard]] QString getAvailableFanModesString() const;
    [[nodiscard]] fan_mode getFanMode() const;
    [[nodiscard]] QString getFanModeName() const;
    [[nodiscard]] bool setFanMode(fan_mode mode) const;
    [[nodiscard]] bool setFanModeName(QString mode) const;

    // fw_version
    [[nodiscard]] QString getFWVersion() const;
    // fw_release_date
    [[nodiscard]] QString getFWReleaseDate() const;

    // cpu/realtime_temperature 0-100 (celsius scale)
    [[nodiscard]] bool hasCPURealtimeTemperature() const;
    [[nodiscard]] int getCPURealtimeTemperature() const;
    // cpu/realtime_fan_speed 0-100 (percent)
    [[nodiscard]] bool hasCPURealtimeFanSpeed() const;
    [[nodiscard]] int getCPURealtimeFanSpeed() const;
    // cpu/basic_fan_speed 0-100 (percent)
    [[nodiscard]] bool hasCPUBasicFanSpeed() const;
    [[nodiscard]] int getCPUBasicFanSpeed() const;
    [[nodiscard]] bool setCPUBasicFanSpeed(int value) const;

    // gpu/realtime_temperature 0-100 (celsius scale)
    [[nodiscard]] bool hasGPURealtimeTemperature() const;
    [[nodiscard]] std::optional<int> getGPURealtimeTemperature() const;
    // gpu/realtime_fan_speed 0-100 (percent)
    [[nodiscard]] bool hasGPURealtimeFanSpeed() const;
    [[nodiscard]] std::optional<int> getGPURealtimeFanSpeed() const;

    // BAT1/charge_control_start_threshold 0-100 (percent)
    [[nodiscard]] bool hasBatteryStartThreshold() const;
    [[nodiscard]] int getBatteryStartThreshold() const;
    [[nodiscard]] bool setBatteryStartThreshold(int value) const;

    // BAT1/charge_control_end_threshold 0-100 (percent)
    [[nodiscard]] bool hasBatteryEndThreshold() const;
    [[nodiscard]] int getBatteryEndThreshold() const;
    [[nodiscard]] bool setBatteryEndThreshold(int value) const;

    // BAT1/capacity 0-100 (percent)
    [[nodiscard]] bool hasBatteryCapacity() const;
    [[nodiscard]] int getBatteryCapacity() const;

    // BAT1/status
    [[nodiscard]] bool hasBatteryStatus() const;
    [[nodiscard]] QString getBatteryStatus() const;

    // kbd_backlight/brightness 0-3
    [[nodiscard]] bool hasKeyboardBacklightBrightness() const;
    [[nodiscard]] int getKeyboardBacklightBrightness() const;
    [[nodiscard]] bool setKeyboardBacklightBrightness(int value) const;

    // kbd_backlight_enable (ng)
    [[nodiscard]] bool hasKeyboardBacklightEnable() const;
    [[nodiscard]] bool getKeyboardBacklightEnable() const;
    [[nodiscard]] bool setKeyboardBacklightEnable(bool enable) const;

    // kbd_backlight_mode (ng)
    [[nodiscard]] bool hasKeyboardBacklightMode() const;
    [[nodiscard]] std::optional<int> getKeyboardBacklightMode() const;
    [[nodiscard]] bool setKeyboardBacklightMode(int mode) const;

    // fan curves (ng)
    [[nodiscard]] bool hasFanCurve() const;
    [[nodiscard]] QString getCPUFanCurve() const;
    [[nodiscard]] bool setCPUFanCurve(QString curve) const;
    [[nodiscard]] QString getGPUFanCurve() const;
    [[nodiscard]] bool setGPUFanCurve(QString curve) const;
    [[nodiscard]] bool getFanCurveActive() const;
    [[nodiscard]] bool setFanCurveActive(bool active) const;
    [[nodiscard]] bool hasFanCurveIdle() const;
    [[nodiscard]] int getCPUFanCurveIdle() const;
    [[nodiscard]] bool setCPUFanCurveIdle(int value) const;
    [[nodiscard]] int getGPUFanCurveIdle() const;
    [[nodiscard]] bool setGPUFanCurveIdle(int value) const;

    // platform device path (msi-ec-ng or msi-ec)
    [[nodiscard]] QString getMsiEcPath() const;

    // fan RPM (ng)
    [[nodiscard]] bool hasFanRpm() const;
    [[nodiscard]] std::optional<int> getCPUFanRpm() const;
    [[nodiscard]] std::optional<int> getGPUFanRpm() const;

    // usb_power_share (ng)
    [[nodiscard]] bool hasUsbPowerShare() const;
    [[nodiscard]] QString getUsbPowerShare() const;
    [[nodiscard]] bool setUsbPowerShare(QString state) const;

    // qc3 (ng)
    [[nodiscard]] bool hasQc3() const;
    [[nodiscard]] bool getQc3() const;
    [[nodiscard]] bool setQc3(bool enable) const;

    // panel (ng)
    [[nodiscard]] bool hasPanel() const;
    [[nodiscard]] bool getPanel() const;
    [[nodiscard]] bool setPanel(bool enable) const;

private:
    QDBusInterface *iface;
    void printError(QDBusError const &error) const;

    template <typename T>
    [[nodiscard]] std::optional<T> getOptionalValue(QString method) const;
    template <typename T>
    [[nodiscard]] T getValue(QString method, T defaultValue) const;
    template <typename T>
    [[nodiscard]] bool setValue(QString method, T value) const;
};

#endif // MSI_EC_HELPER_H
