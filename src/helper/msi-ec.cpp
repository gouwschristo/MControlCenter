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

#include "msi-ec.h"
#include <QFile>

// Available entries: https://github.com/BeardOverflow/msi-ec?tab=readme-ov-file#usage
// The ng fork adds fan curves, fan RPM, USB power-share, QC3, panel and
// keyboard-backlight enable/mode attributes.
static QString detectMsiEcPath() {
    const QString ng = "/sys/devices/platform/msi-ec-ng";
    if (QFile::exists(ng + "/fw_version"))
        return ng;
    return "/sys/devices/platform/msi-ec";
}

const QString msi_ec_path = detectMsiEcPath();
const QString msi_ec_webcam = msi_ec_path + "/webcam";
const QString msi_ec_webcam_block = msi_ec_path + "/webcam_block";
const QString msi_ec_fn_key = msi_ec_path + "/fn_key";
const QString msi_ec_win_key = msi_ec_path + "/win_key";
const QString msi_ec_cooler_boost = msi_ec_path + "/cooler_boost";
const QString msi_ec_available_shift_modes = msi_ec_path + "/available_shift_modes";
const QString msi_ec_shift_mode = msi_ec_path + "/shift_mode";
const QString msi_ec_super_battery = msi_ec_path + "/super_battery";
const QString msi_ec_available_fan_modes = msi_ec_path + "/available_fan_modes";
const QString msi_ec_fan_mode = msi_ec_path + "/fan_mode";
const QString msi_ec_fw_version = msi_ec_path + "/fw_version";
const QString msi_ec_fw_release_date = msi_ec_path + "/fw_release_date";
const QString msi_ec_cpu_realtime_temperature = msi_ec_path + "/cpu/realtime_temperature";
const QString msi_ec_cpu_realtime_fan_speed = msi_ec_path + "/cpu/realtime_fan_speed";
const QString msi_ec_cpu_basic_fan_speed = msi_ec_path + "/cpu/basic_fan_speed";
const QString msi_ec_gpu_realtime_temperature = msi_ec_path + "/gpu/realtime_temperature";
const QString msi_ec_gpu_realtime_fan_speed = msi_ec_path + "/gpu/realtime_fan_speed";
const QString msi_ec_bat1 = "/sys/class/power_supply/BAT1";
const QString msi_ec_bat1_start_threshold = msi_ec_bat1 + "/charge_control_start_threshold";
const QString msi_ec_bat1_end_threshold = msi_ec_bat1 + "/charge_control_end_threshold";
const QString msi_ec_bat1_capacity = msi_ec_bat1 + "/capacity";
const QString msi_ec_bat1_status = msi_ec_bat1 + "/status";
// /sys/class/leds/platform::<led_name>/brightness
const QString msi_ec_kbd_backlight_brightness = "/sys/class/leds/msiacpi::kbd_backlight/brightness";
// ng fork additions
const QString msi_ec_cpu_fan_curve = msi_ec_path + "/cpu/fan_curve";
const QString msi_ec_gpu_fan_curve = msi_ec_path + "/gpu/fan_curve";
const QString msi_ec_cpu_fan_curve_idle = msi_ec_path + "/cpu/fan_curve_idle";
const QString msi_ec_gpu_fan_curve_idle = msi_ec_path + "/gpu/fan_curve_idle";
const QString msi_ec_fan_curve_active = msi_ec_path + "/fan_curve_active";
const QString msi_ec_cpu_fan_rpm = msi_ec_path + "/cpu/fan_rpm";
const QString msi_ec_gpu_fan_rpm = msi_ec_path + "/gpu/fan_rpm";
const QString msi_ec_usb_power_share = msi_ec_path + "/usb_power_share";
const QString msi_ec_qc3 = msi_ec_path + "/qc3";
const QString msi_ec_panel = msi_ec_path + "/panel";
const QString msi_ec_kbd_backlight_enable = msi_ec_path + "/kbd_backlight_enable";
const QString msi_ec_kbd_backlight_mode = msi_ec_path + "/kbd_backlight_mode";

QString MsiEc::readFile(QString path) const {
    if (QFile file(path); file.exists() && file.open(QIODevice::ReadOnly)) {
        // Remove only the last '\n'
        return file.readAll().chopped(1);
    }
    return "";
}

bool MsiEc::readFileOnOff(QString path) const {
    return readFile(path) == "on";
}

bool MsiEc::writeFile(QString path, QString value) const {
    QFile file(path);
    if (!file.exists()) {
        fprintf(stderr, "missing sysfs attribute: %s\n", qPrintable(path));
        return false;
    }
    if (!file.open(QIODevice::WriteOnly)) {
        fprintf(stderr, "cannot open %s: %s\n", qPrintable(path),
                qPrintable(file.errorString()));
        return false;
    }

    const QByteArray data = value.toUtf8();
    const bool written = file.write(data) == data.size();
    file.close();
    if (!written) {
        fprintf(stderr, "write failed: %s\n", qPrintable(path));
        return false;
    }

    // sysfs attributes are the source of truth: verify what the kernel took.
    const QString readback = readFile(path);
    if (readback != value) {
        fprintf(stderr, "readback mismatch on %s: wrote '%s', got '%s'\n",
                qPrintable(path), qPrintable(value), qPrintable(readback));
        return false;
    }
    return true;
}

bool MsiEc::writeFileOnOff(QString path, bool on) const {
    return writeFile(path, on ? "on" : "off");
}

bool MsiEc::isMsiEcModuleLoaded() const {
    if (QFile::exists(msi_ec_fw_version)) {
        return true;
    }
    fprintf(stderr, "%s\n", qPrintable("The msi_ec kernel module is not loaded"));
    return false;
}

//////////////// webcam ////////////////

bool MsiEc::hasWebcam() const {
    return QFile::exists(msi_ec_webcam);
}
bool MsiEc::getWebcam() const {
    return readFileOnOff(msi_ec_webcam);
}
bool MsiEc::setWebcam(bool enable) const {
    return writeFileOnOff(msi_ec_webcam, enable);
}

//////////////// webcam_block ////////////////

bool MsiEc::hasWebcamBlock() const {
    return QFile::exists(msi_ec_webcam_block);
}
bool MsiEc::getWebcamBlock() const {
    return readFileOnOff(msi_ec_webcam_block);
}
bool MsiEc::setWebcamBlock(bool enable) const {
    return writeFileOnOff(msi_ec_webcam_block, enable);
}

//////////////// fn_key ////////////////

bool MsiEc::hasFnKey() const {
    return QFile::exists(msi_ec_fn_key);
}
QString MsiEc::getFnKey() const {
    return readFile(msi_ec_fn_key);
}
bool MsiEc::setFnKey(QString side) const {
    return writeFile(msi_ec_fn_key, side);
}

//////////////// win_key ////////////////

bool MsiEc::hasWinKey() const {
    return QFile::exists(msi_ec_win_key);
}
QString MsiEc::getWinKey() const {
    return readFile(msi_ec_win_key);
}
bool MsiEc::setWinKey(QString side) const {
    return writeFile(msi_ec_win_key, side);
}

//////////////// fn_win_swap ////////////////

bool MsiEc::hasFnWinSwap() const {
    return hasFnKey();
}
bool MsiEc::getFnWinSwap() const {
    // fn_key and win_key report the physical position of the keys.
    // On this EC family bit 4 = 1 means win_key is left and fn_key is right,
    // i.e. the keys are swapped. msi-ec exposes fn_key as "right" then.
    return getFnKey() == "right";
}
bool MsiEc::setFnWinSwap(bool swap) const {
    return setFnKey(swap ? "right" : "left");
}

//////////////// cooler_boost ////////////////

bool MsiEc::hasCoolerBoost() const {
    return QFile::exists(msi_ec_cooler_boost);
}
bool MsiEc::getCoolerBoost() const {
    return readFileOnOff(msi_ec_cooler_boost);
}
bool MsiEc::setCoolerBoost(bool enable) const {
    return writeFileOnOff(msi_ec_cooler_boost, enable);
}

//////////////// shift_mode ////////////////

bool MsiEc::hasShiftMode() const {
    return QFile::exists(msi_ec_shift_mode);
}
QString MsiEc::getAvailableShiftModes() const {
    return readFile(msi_ec_available_shift_modes);
}
QString MsiEc::getShiftMode() const {
    return readFile(msi_ec_shift_mode);
}
bool MsiEc::setShiftMode(QString mode) const {
    return writeFile(msi_ec_shift_mode, mode);
}

//////////////// super_battery ////////////////

bool MsiEc::hasSuperBattery() const {
    return QFile::exists(msi_ec_super_battery);
}
bool MsiEc::getSuperBattery() const {
    return readFileOnOff(msi_ec_super_battery);
}
bool MsiEc::setSuperBattery(bool enable) const {
    return writeFileOnOff(msi_ec_super_battery, enable);
}

//////////////// fan_mode ////////////////

bool MsiEc::hasFanMode() const {
    return QFile::exists(msi_ec_fan_mode);
}
QString MsiEc::getAvailableFanModes() const {
    return readFile(msi_ec_available_fan_modes);
}
QString MsiEc::getFanMode() const {
    return readFile(msi_ec_fan_mode);
}
bool MsiEc::setFanMode(QString mode) const {
    return writeFile(msi_ec_fan_mode, mode);
}

//////////////// fw_version ////////////////

QString MsiEc::getFWVersion() const {
    return readFile(msi_ec_fw_version);
}

//////////////// fw_release_date ////////////////

QString MsiEc::getFWReleaseDate() const {
    return readFile(msi_ec_fw_release_date);
}

//////////////// CPU ////////////////

// cpu/realtime_temperature 0-100 (celsius scale)
bool MsiEc::hasCPURealtimeTemperature() const {
    return QFile::exists(msi_ec_cpu_realtime_temperature);
}
int MsiEc::getCPURealtimeTemperature() const {
    return readFile(msi_ec_cpu_realtime_temperature).toInt();
}

// cpu/realtime_fan_speed 0-100 (percent)
bool MsiEc::hasCPURealtimeFanSpeed() const {
    return QFile::exists(msi_ec_cpu_realtime_fan_speed);
}
int MsiEc::getCPURealtimeFanSpeed() const {
    return readFile(msi_ec_cpu_realtime_fan_speed).toInt();
}

// cpu/basic_fan_speed 0-100 (percent)
bool MsiEc::hasCPUBasicFanSpeed() const {
    return QFile::exists(msi_ec_cpu_basic_fan_speed);
}
int MsiEc::getCPUBasicFanSpeed() const {
    return readFile(msi_ec_cpu_basic_fan_speed).toInt();
}
bool MsiEc::setCPUBasicFanSpeed(int value) const {
    return writeFile(msi_ec_cpu_basic_fan_speed, QString::number(value));
}

//////////////// GPU ////////////////

// gpu/realtime_temperature 0-100 (celsius scale)
bool MsiEc::hasGPURealtimeTemperature() const {
    return QFile::exists(msi_ec_gpu_realtime_temperature);
}
int MsiEc::getGPURealtimeTemperature() const {
    return readFile(msi_ec_gpu_realtime_temperature).toInt();
}

// gpu/realtime_fan_speed 0-100 (percent)
bool MsiEc::hasGPURealtimeFanSpeed() const {
    return QFile::exists(msi_ec_gpu_realtime_fan_speed);
}
int MsiEc::getGPURealtimeFanSpeed() const {
    return readFile(msi_ec_gpu_realtime_fan_speed).toInt();
}

//////////////// Charge control ////////////////

// BAT1/charge_control_start_threshold 0-100 (percent)
bool MsiEc::hasBatteryStartThreshold() const {
    return QFile::exists(msi_ec_bat1_start_threshold);
}
int MsiEc::getBatteryStartThreshold() const {
    return readFile(msi_ec_bat1_start_threshold).toInt();
}
bool MsiEc::setBatteryStartThreshold(int value) const {
    return writeFile(msi_ec_bat1_start_threshold, QString::number(value));
}

// BAT1/charge_control_end_threshold 0-100 (percent)
bool MsiEc::hasBatteryEndThreshold() const {
    return QFile::exists(msi_ec_bat1_end_threshold);
}
int MsiEc::getBatteryEndThreshold() const {
    return readFile(msi_ec_bat1_end_threshold).toInt();
}
bool MsiEc::setBatteryEndThreshold(int value) const {
    return writeFile(msi_ec_bat1_end_threshold, QString::number(value));
}

// BAT1/capacity 0-100 (percent)
bool MsiEc::hasBatteryCapacity() const {
    return QFile::exists(msi_ec_bat1_capacity);
}

int MsiEc::getBatteryCapacity() const {
    return readFile(msi_ec_bat1_capacity).toInt();
}

// BAT1/status 0-100 (percent)
bool MsiEc::hasBatteryStatus() const {
    return QFile::exists(msi_ec_bat1_status);
}

QString MsiEc::getBatteryStatus() const {
    return readFile(msi_ec_bat1_status);
}

//////////////// Keyboard Backlight ////////////////

// kbd_backlight/brightness 0-3
bool MsiEc::hasKeyboardBacklightBrightness() const {
    return QFile::exists(msi_ec_kbd_backlight_brightness);
}
int MsiEc::getKeyboardBacklightBrightness() const {
    return readFile(msi_ec_kbd_backlight_brightness).toInt();
}
bool MsiEc::setKeyboardBacklightBrightness(int value) const {
    return writeFile(msi_ec_kbd_backlight_brightness, QString::number(value));
}

//////////////// Keyboard backlight enable / mode (ng) ////////////////

bool MsiEc::hasKeyboardBacklightEnable() const {
    return QFile::exists(msi_ec_kbd_backlight_enable);
}
bool MsiEc::getKeyboardBacklightEnable() const {
    return readFileOnOff(msi_ec_kbd_backlight_enable);
}
bool MsiEc::setKeyboardBacklightEnable(bool enable) const {
    return writeFileOnOff(msi_ec_kbd_backlight_enable, enable);
}

bool MsiEc::hasKeyboardBacklightMode() const {
    return QFile::exists(msi_ec_kbd_backlight_mode);
}
int MsiEc::getKeyboardBacklightMode() const {
    return readFile(msi_ec_kbd_backlight_mode).toInt();
}
bool MsiEc::setKeyboardBacklightMode(int mode) const {
    return writeFile(msi_ec_kbd_backlight_mode, QString::number(mode));
}

//////////////// Fan curves (ng) ////////////////

bool MsiEc::hasFanCurve() const {
    return QFile::exists(msi_ec_cpu_fan_curve) && QFile::exists(msi_ec_gpu_fan_curve);
}
QString MsiEc::getCPUFanCurve() const {
    return readFile(msi_ec_cpu_fan_curve);
}
bool MsiEc::setCPUFanCurve(QString curve) const {
    return writeFile(msi_ec_cpu_fan_curve, curve);
}
QString MsiEc::getGPUFanCurve() const {
    return readFile(msi_ec_gpu_fan_curve);
}
bool MsiEc::setGPUFanCurve(QString curve) const {
    return writeFile(msi_ec_gpu_fan_curve, curve);
}
bool MsiEc::getFanCurveActive() const {
    return readFileOnOff(msi_ec_fan_curve_active);
}
bool MsiEc::setFanCurveActive(bool active) const {
    return writeFileOnOff(msi_ec_fan_curve_active, active);
}
bool MsiEc::hasFanCurveIdle() const {
    return QFile::exists(msi_ec_cpu_fan_curve_idle) &&
           QFile::exists(msi_ec_gpu_fan_curve_idle);
}
int MsiEc::getCPUFanCurveIdle() const {
    return readFile(msi_ec_cpu_fan_curve_idle).toInt();
}
bool MsiEc::setCPUFanCurveIdle(int value) const {
    return writeFile(msi_ec_cpu_fan_curve_idle, QString::number(value));
}
int MsiEc::getGPUFanCurveIdle() const {
    return readFile(msi_ec_gpu_fan_curve_idle).toInt();
}
bool MsiEc::setGPUFanCurveIdle(int value) const {
    return writeFile(msi_ec_gpu_fan_curve_idle, QString::number(value));
}

QString MsiEc::getMsiEcPath() const {
    return msi_ec_path;
}

//////////////// Fan RPM (ng) ////////////////

bool MsiEc::hasFanRpm() const {
    return QFile::exists(msi_ec_cpu_fan_rpm) && QFile::exists(msi_ec_gpu_fan_rpm);
}
int MsiEc::getCPUFanRpm() const {
    return readFile(msi_ec_cpu_fan_rpm).toInt();
}
int MsiEc::getGPUFanRpm() const {
    return readFile(msi_ec_gpu_fan_rpm).toInt();
}

//////////////// USB power-share (ng) ////////////////

bool MsiEc::hasUsbPowerShare() const {
    return QFile::exists(msi_ec_usb_power_share);
}
QString MsiEc::getUsbPowerShare() const {
    return readFile(msi_ec_usb_power_share);
}
bool MsiEc::setUsbPowerShare(QString state) const {
    return writeFile(msi_ec_usb_power_share, state);
}

//////////////// Quick Charge QC3 (ng) ////////////////

bool MsiEc::hasQc3() const {
    return QFile::exists(msi_ec_qc3);
}
bool MsiEc::getQc3() const {
    return readFileOnOff(msi_ec_qc3);
}
bool MsiEc::setQc3(bool enable) const {
    return writeFileOnOff(msi_ec_qc3, enable);
}

//////////////// Panel / OD (ng) ////////////////

bool MsiEc::hasPanel() const {
    return QFile::exists(msi_ec_panel);
}
bool MsiEc::getPanel() const {
    return readFileOnOff(msi_ec_panel);
}
bool MsiEc::setPanel(bool enable) const {
    return writeFileOnOff(msi_ec_panel, enable);
}
