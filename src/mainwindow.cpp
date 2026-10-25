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

#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "operate.h"
#include "settings.h"
#include <QTimer>
#include <QMessageBox>
#include <QSignalBlocker>
#include <QGuiApplication>
#include <QClipboard>
#include <QTextStream>

Operate operate;
PowerMonitor powerMonitor;

bool isActive = false;
bool isUpdateDataError = false;

QTimer *realtimeUpdateTimer = new QTimer;

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    connect(ui->advancedFanControlCheckBox, &QCheckBox::toggled, this, &MainWindow::setFanModeAdvanced);

    connect(ui->fan1Speed1Slider, &QSlider::valueChanged, this, [this]() {
        ui->fan1Speed1Label->setText(QString("%1%").arg(ui->fan1Speed1Slider->value()));
        checkFanSettingsChanged();
    });
    connect(ui->fan1Speed2Slider, &QSlider::valueChanged, this, [this]() {
        ui->fan1Speed2Label->setText(QString("%1%").arg(ui->fan1Speed2Slider->value()));
        checkFanSettingsChanged();
    });
    connect(ui->fan1Speed3Slider, &QSlider::valueChanged, this, [this]() {
        ui->fan1Speed3Label->setText(QString("%1%").arg(ui->fan1Speed3Slider->value()));
        checkFanSettingsChanged();
    });
    connect(ui->fan1Speed4Slider, &QSlider::valueChanged, this, [this]() {
        ui->fan1Speed4Label->setText(QString("%1%").arg(ui->fan1Speed4Slider->value()));
        checkFanSettingsChanged();
    });
    connect(ui->fan1Speed5Slider, &QSlider::valueChanged, this, [this]() {
        ui->fan1Speed5Label->setText(QString("%1%").arg(ui->fan1Speed5Slider->value()));
        checkFanSettingsChanged();
    });
    connect(ui->fan1Speed6Slider, &QSlider::valueChanged, this, [this]() {
        ui->fan1Speed6Label->setText(QString("%1%").arg(ui->fan1Speed6Slider->value()));
        checkFanSettingsChanged();
    });
    connect(ui->fan1Speed7Slider, &QSlider::valueChanged, this, [this]() {
        ui->fan1Speed7Label->setText(QString("%1%").arg(ui->fan1Speed7Slider->value()));
        checkFanSettingsChanged();
    });

    connect(ui->fan1Speed2TempSpinBox, &QSpinBox::textChanged, this, [this]() {
        ui->fan1Speed1TempLabel->setText(QString("< %1 °C").arg(ui->fan1Speed2TempSpinBox->value()));
        checkFanSettingsChanged();
    });
    connect(ui->fan1Speed3TempSpinBox, &QSpinBox::textChanged, this, [this]() {
        checkFanSettingsChanged();
    });
    connect(ui->fan1Speed4TempSpinBox, &QSpinBox::textChanged, this, [this]() {
        checkFanSettingsChanged();
    });
    connect(ui->fan1Speed5TempSpinBox, &QSpinBox::textChanged, this, [this]() {
        checkFanSettingsChanged();
    });
    connect(ui->fan1Speed6TempSpinBox, &QSpinBox::textChanged, this, [this]() {
        checkFanSettingsChanged();
    });
    connect(ui->fan1Speed7TempSpinBox, &QSpinBox::textChanged, this, [this]() {
        checkFanSettingsChanged();
    });

    connect(ui->fan2Speed1Slider, &QSlider::valueChanged, this, [this]() {
        ui->fan2Speed1Label->setText(QString("%1%").arg(ui->fan2Speed1Slider->value()));
        checkFanSettingsChanged();
    });
    connect(ui->fan2Speed2Slider, &QSlider::valueChanged, this, [this]() {
        ui->fan2Speed2Label->setText(QString("%1%").arg(ui->fan2Speed2Slider->value()));
        checkFanSettingsChanged();
    });
    connect(ui->fan2Speed3Slider, &QSlider::valueChanged, this, [this]() {
        ui->fan2Speed3Label->setText(QString("%1%").arg(ui->fan2Speed3Slider->value()));
        checkFanSettingsChanged();
    });
    connect(ui->fan2Speed4Slider, &QSlider::valueChanged, this, [this]() {
        ui->fan2Speed4Label->setText(QString("%1%").arg(ui->fan2Speed4Slider->value()));
        checkFanSettingsChanged();
    });
    connect(ui->fan2Speed5Slider, &QSlider::valueChanged, this, [this]() {
        ui->fan2Speed5Label->setText(QString("%1%").arg(ui->fan2Speed5Slider->value()));
        checkFanSettingsChanged();
    });
    connect(ui->fan2Speed6Slider, &QSlider::valueChanged, this, [this]() {
        ui->fan2Speed6Label->setText(QString("%1%").arg(ui->fan2Speed6Slider->value()));
        checkFanSettingsChanged();
    });
    connect(ui->fan2Speed7Slider, &QSlider::valueChanged, this, [this]() {
        ui->fan2Speed7Label->setText(QString("%1%").arg(ui->fan2Speed7Slider->value()));
        checkFanSettingsChanged();
    });

    connect(ui->fan2Speed2TempSpinBox, &QSpinBox::textChanged, this, [this]() {
        ui->fan2Speed1TempLabel->setText(QString("< %1 °C").arg(ui->fan2Speed2TempSpinBox->value()));
        checkFanSettingsChanged();
    });
    connect(ui->fan2Speed3TempSpinBox, &QSpinBox::textChanged, this, [this]() {
        checkFanSettingsChanged();
    });
    connect(ui->fan2Speed4TempSpinBox, &QSpinBox::textChanged, this, [this]() {
        checkFanSettingsChanged();
    });
    connect(ui->fan2Speed5TempSpinBox, &QSpinBox::textChanged, this, [this]() {
        checkFanSettingsChanged();
    });
    connect(ui->fan2Speed6TempSpinBox, &QSpinBox::textChanged, this, [this]() {
        checkFanSettingsChanged();
    });
    connect(ui->fan2Speed7TempSpinBox, &QSpinBox::textChanged, this, [this]() {
        checkFanSettingsChanged();
    });

    connect(ui->fanSpeedResetButton, &QPushButton::clicked, this, &MainWindow::loadDefaultFanSettings);
    connect(ui->fanSpeedApplyButton, &QPushButton::clicked, this, &MainWindow::setFanSpeedSettings);
    connect(ui->copyDiagnosticsButton, &QPushButton::clicked, this, &MainWindow::copyDiagnostics);
    connect(ui->keyboardBrightnessSlider, &QSlider::valueChanged, this, [this](int value) {
        ui->keyboardBrightnessValueLabel->setText(QString::number(value));
    });
    connect(&powerMonitor, &PowerMonitor::currentChargerState, this, &MainWindow::on_ChargerStateChange);
    connect(&powerMonitor, &PowerMonitor::currentPowerProfile, this, &MainWindow::on_PowerProfileChange);
    connect(&gameMonitor, &GameMonitor::gameStateChanged, this, &MainWindow::onGameStateChanged);

    connect(qApp, &QGuiApplication::saveStateRequest, this, &MainWindow::saveStateRequest);

    MainWindow::setWindowIcon(QIcon(":/images/AppIcon"));
    Settings s;
    if (s.isValueExist("MainWindow/Width") && s.isValueExist("MainWindow/Height")) {
        const int savedWidth = s.getValue("MainWindow/Width").toInt();
        const int savedHeight = s.getValue("MainWindow/Height").toInt();
        // Older builds saved a cramped 580x480 window. Keep the new design
        // size unless the user has already resized to something roomy.
        if (savedWidth >= 900 && savedHeight >= 700)
            MainWindow::resize(savedWidth, savedHeight);
    }

    createTrayIcon();
    updateGameStatusLabel();

    ui->tabWidget->tabBar()->setExpanding(true);
    ui->tabWidget->setTabToolTip(0, tr("Live system status and one-click performance profiles."));
    ui->tabWidget->setTabToolTip(1, tr("Charge limits and automatic charger/battery profile switching."));
    ui->tabWidget->setTabToolTip(2, tr("Custom CPU and GPU fan curves."));
    ui->tabWidget->setTabToolTip(3, tr("Keyboard backlight on/off, mode and brightness."));
    ui->tabWidget->setTabToolTip(4, tr("Versions, project links and diagnostics."));
    ui->tabWidget->setTabToolTip(5, tr("Raw EC memory access. Hidden by default."));
    // Disable debug tab
    ui->tabWidget->setTabVisible(5, false);
    setTabsEnabled(false);

    if (!operate.isMsiEcLoaded()) {
        QMessageBox::critical(nullptr, this->windowTitle(), tr("The msi-ec module is not loaded/installed.\n"
                                                               "Check the <About> page for more info."));
    }

    if (!operate.isEcSysModuleLoaded() && !operate.loadEcSysModule())
        QMessageBox::critical(nullptr, this->windowTitle(), tr("The ec_sys module couldn't be detected, it might be required to control the fans."));



    if(operate.updateEcData())
        updateData();

    connect(realtimeUpdateTimer, &QTimer::timeout, this, &MainWindow::realtimeUpdate);
    setUpdateInterval(1000);

    // Timer to detect sleep and reapply Advanced Mode Fan if necessary
    connect(&timerSleepWatcher, &QTimer::timeout, this, &MainWindow::timerSleepTimeout);
    timerSleepWatcher.setInterval(10 * 1000);
    timerSleepWatcher.start();

    ui->QtVersionValue->setText(QT_VERSION_STR);
    ui->versionValueLabel->setText(MControlCenter_VERSION);
    ui->autoAcDcProfilesGroupBox->setChecked(s.getValue("Settings/autoAcDcProfilesState").toBool());
    ui->userModeOnBatteryComboBox->setCurrentIndex(s.getValue("Settings/UserModeOnBattery").toInt());
    ui->userModeOnChargerComboBox->setCurrentIndex(s.getValue("Settings/UserModeOnCharger").toInt());
    ui->autoPPDCheckBox->setChecked(s.getValue("Settings/autoPPDstate").toBool());
    if (s.isValueExist("Settings/gameModeOnBatteryState"))
        ui->gameModeOnBatteryCheckBox->setChecked(s.getValue("Settings/gameModeOnBatteryState").toBool());
    ui->autoGameModeCheckBox->setChecked(s.getValue("Settings/autoGameModeState").toBool());

    // Label the vendor USB power-share profiles.
    for (int i = 0; i < ui->usbPowerShareComboBox->count(); i++) {
        const QString tip = i == 0 ? tr("USB power sharing disabled")
                                   : tr("Vendor USB power-share profile %1 (EC 0xF7)").arg(i - 1);
        ui->usbPowerShareComboBox->setItemData(i, tip, Qt::ToolTipRole);
    }
    ui->keyboardBrightnessSlider->setToolTip(
        tr("0 = minimum brightness while the keyboard is on; use the On checkbox to turn the backlight off"));
}

MainWindow::~MainWindow() {
    disconnect(&gameMonitor, nullptr, this, nullptr);
    gameMonitor.stop();
    restoreGameMode();
    delete ui;
}

void MainWindow::setUpdateDataError(bool error) {
    isUpdateDataError = error;
}

void MainWindow::setTabsEnabled(bool enabled) {
    ui->overviewTab->setEnabled(enabled);
    ui->modeFormWidget->setEnabled(enabled);
    ui->batteryTab->setEnabled(enabled);
    ui->fanControlTab->setEnabled(enabled);
    ui->keyboardTab->setEnabled(enabled);
    ui->debugTab->setEnabled(enabled);

    if (modeTrayMenu)
        modeTrayMenu->setEnabled(enabled);
    if (batteryTrayMenu)
        batteryTrayMenu->setEnabled(enabled);
}

void MainWindow::startRealtimeUpdate() const {
    realtimeUpdateTimer->start();
}

void MainWindow::stopRealtimeUpdate() const {
    realtimeUpdateTimer->stop();
}

void MainWindow::setUpdateInterval(int msec) const {
    realtimeUpdateTimer->setInterval(msec);
}

void MainWindow::realtimeUpdate() {
    operate.updateEcDataAsync();
    updateData();
}

void MainWindow::updateData() {
    if (!isUpdateDataError && (operate.isMsiEcLoaded() || operate.isEcSysModuleLoaded())) {
        if (!isActive) {
            operate.doProbe();
            setTabsEnabled(true);
            loadConfigs();
            isActive = true;
        }
        updateBatteryCharge();
        updateChargingStatus();
        updateCpuTemp();
        updateGpuTemp();
        updateFanMode();
        updateShiftMode();
        updateUserMode();
        updateFan1Speed();
        updateFan2Speed();
        updateKeyboardBrightness();
        updateKeyboardBacklightMode();
        updateWebCamState();
        updateWebCamBlockState();
        updateUsbPowerShareState();
        updateQc3State();
        updatePanelState();
        updateFnSuperSwapState();
        updateCoolerBoostState();
        updateTrayTooltip();
    } else {
        setTabsEnabled(false);
        ui->MsiEcStatusLabel->setText(tr("Failed to load both msi-ec/ec_sys"));
        isActive = false;
    }
}

void MainWindow::loadConfigs() {
    ui->ecVersionValueLabel->setText(QString::fromStdString(operate.getEcVersion()));
    ui->ecBuildValueLabel->setText(QString::fromStdString(operate.getEcBuild()));
    ui->MsiEcStatusLabel->setText(QString("%1 (ec_sys: %2)")
        .arg(operate.getDriverName(), operate.isEcSysModuleLoaded() ? tr("yes") : tr("no")));

    operate.loadSettings();

    {
        QSignalBlocker shiftBlocker(ui->shiftModeComboBox);
        QSignalBlocker fanBlocker(ui->fanModeComboBox);
        ui->shiftModeComboBox->clear();
        ui->shiftModeComboBox->addItems(operate.getAvailableShiftModeNames());
        ui->fanModeComboBox->clear();
        ui->fanModeComboBox->addItems(operate.getAvailableFanModeNames());
    }

    updateUserMode();
    updateCoolerBoostState();
    updateShiftMode();
    updateFanMode();

    if (operate.isBatteryThresholdSupport()) {
        updateBatteryThreshold();
    } else {
        ui->batteryTab->setEnabled(false);
        if (batteryTrayMenu)
            batteryTrayMenu->setEnabled(false);
    }

    updateFanSpeedSettings();

    if (operate.isKeyboardBacklightModeSupport()) {
        updateKeyboardBacklightMode();
    } else {
        ui->keyboardBacklightModeComboBox->setEnabled(false);
    }

    if (operate.isKeyboardBacklightSupport()) {
        updateKeyboardBrightness();
        ui->kbdBacklightEnableCheckBox->setEnabled(operate.isKeyboardBacklightEnableSupport());
    } else {
        ui->keyboardBrightnessSlider->setEnabled(false);
        ui->kbdBacklightEnableCheckBox->setEnabled(false);
        ui->tabWidget->removeTab(3);
    }

    if (operate.isUsbPowerShareSupport()) {
        updateUsbPowerShareState();
    } else {
        ui->usbPowerShareComboBox->setEnabled(false);
    }

    if (operate.isQc3Support()) {
        updateQc3State();
    } else {
        ui->qc3CheckBox->setEnabled(false);
    }

    if (operate.isPanelSupport()) {
        updatePanelState();
    } else {
        ui->panelCheckBox->setEnabled(false);
    }

    if (operate.isWebCamOffSupport()) {
        updateWebCamState();
    } else {
        ui->webCamCheckBox->setEnabled(false);
    }

    if (operate.isWebCamBlockSupport()) {
        updateWebCamBlockState();
    } else {
        ui->webcamBlockCheckBox->setEnabled(false);
    }

    updateFnSuperSwapState();
}

QString MainWindow::intToQString(int value) const {
    if (value < 0)
        return "-";
    return QString::number(value);
}

void MainWindow::updateBatteryCharge() {
    ui->BatteryChargeValueLabel->setText(intToQString(operate.getBatteryCharge()) + " %");
}

void MainWindow::updateBatteryThreshold() {
    if (operate.updateEcData()) {
        int batteryThreshold = operate.getBatteryThreshold();
        if (batteryThreshold == 0)
            ui->batteryThresholdValueLabel->setText("100 %");
        else
            ui->batteryThresholdValueLabel->setText(
                QString("%1 % (start %2 %)")
                    .arg(batteryThreshold)
                    .arg(operate.getBatteryStartThreshold()));

        QSignalBlocker b1(ui->bestMobilityRadioButton);
        QSignalBlocker b2(ui->bestBatteryRadioButton);
        QSignalBlocker b3(ui->balancedBatteryRadioButton);
        QSignalBlocker b4(ui->customBatteryThresholdRadioButton);
        QSignalBlocker b5(ui->customBatteryThresholdSpinBox);

        switch (batteryThreshold) {
            case 0:
            case 100:
                ui->bestMobilityRadioButton->setChecked(true);
                batteryThreshold = 100;
                break;
            case 60:
                ui->bestBatteryRadioButton->setChecked(true);
                break;
            case 80:
                ui->balancedBatteryRadioButton->setChecked(true);
                break;
            default:
                ui->customBatteryThresholdRadioButton->setChecked(true);
                ui->customBatteryApplyButton->setEnabled(
                        batteryThreshold != ui->customBatteryThresholdSpinBox->value());
                break;
        }

        ui->customBatteryThresholdSpinBox->setValue(batteryThreshold);
    }
}

void MainWindow::updateChargingStatus() {
    QString chargingStatus;
    switch (operate.getChargingStatus()) {
        case charging_state::battery_charging:
            chargingStatus = tr("Charging");
            break;
        case charging_state::battery_discharging:
            chargingStatus = tr("Discharging");
            break;
        case charging_state::battery_not_charging:
            chargingStatus = tr("Not charging");
            break;
        case charging_state::battery_fully_charged:
            chargingStatus = tr("Fully charged");
            break;
        default:
            chargingStatus = tr("Unknown");
            break;
    }
    ui->chargingStatusValueLabel->setText(chargingStatus);
}

void MainWindow::updateCpuTemp() {
    ui->cpuTempValueLabel->setText(intToQString(operate.getCpuTemp()) + " °C");
}

void MainWindow::updateGpuTemp() {
    std::optional<int> temp = operate.getGpuTemp();
    if (temp.has_value()) {
        ui->gpuTempValueLabel->setVisible(true);
        ui->gpuTempLabel->setVisible(true);
        if (temp.value() != 0)
            ui->gpuTempValueLabel->setText(intToQString(temp.value()) + " °C");
        else
            ui->gpuTempValueLabel->setText(tr("OFF"));
    } else {
        ui->gpuTempValueLabel->setVisible(false);
        ui->gpuTempLabel->setVisible(false);
    }
}

void MainWindow::updateFan1Speed() {
    int speed = operate.getFan1Speed();
    int duty = operate.getFan1Duty();
    if (speed != 0)
        ui->fan1ValueLabel->setText(
            QString("%1 %2 (%3 %)").arg(speed).arg(tr("rpm")).arg(duty));
    else
        ui->fan1ValueLabel->setText(tr("OFF"));
}

void MainWindow::updateFan2Speed() {
    std::optional<int> speed = operate.getFan2Speed();
    if (speed.has_value()) {
        ui->fan2ValueLabel->setVisible(true);
        ui->gpuFanLabel->setVisible(true);
        const int duty = operate.getFan2Duty().value_or(0);
        if (speed.value() != 0)
            ui->fan2ValueLabel->setText(
                QString("%1 %2 (%3 %)").arg(speed.value()).arg(tr("rpm")).arg(duty));
        else
            ui->fan2ValueLabel->setText(tr("OFF"));
    } else {
        ui->fan2ValueLabel->setVisible(false);
        ui->gpuFanLabel->setVisible(false);
        ui->fanControlTabWidget->setTabEnabled(1, false);
    }
}

void MainWindow::updateKeyboardBacklightMode() {
    QSignalBlocker blocker(ui->keyboardBacklightModeComboBox);
    ui->keyboardBacklightModeComboBox->setCurrentIndex(operate.getKeyboardBacklightMode());
}

void MainWindow::updateKeyboardBrightness() const {
    QSignalBlocker sliderBlocker(ui->keyboardBrightnessSlider);
    QSignalBlocker checkBlocker(ui->kbdBacklightEnableCheckBox);
    const int brightness = operate.getKeyboardBrightness();
    ui->keyboardBrightnessSlider->setSliderPosition(brightness);
    ui->keyboardBrightnessValueLabel->setText(QString::number(brightness));
    if (operate.isKeyboardBacklightEnableSupport())
        ui->kbdBacklightEnableCheckBox->setChecked(operate.getKeyboardBacklightEnabled());
}

void MainWindow::updateUsbPowerShareState() {
    const QString state = operate.getUsbPowerShareState();
    int index = 0; // off
    if (state.startsWith("state"))
        index = state.mid(5).toInt() + 1;
    QSignalBlocker blocker(ui->usbPowerShareComboBox);
    ui->usbPowerShareComboBox->setCurrentIndex(qBound(0, index, ui->usbPowerShareComboBox->count() - 1));
}

void MainWindow::updateQc3State() {
    QSignalBlocker blocker(ui->qc3CheckBox);
    ui->qc3CheckBox->setChecked(operate.getQc3State());
}

void MainWindow::updatePanelState() {
    QSignalBlocker blocker(ui->panelCheckBox);
    ui->panelCheckBox->setChecked(operate.getPanelState());
}

void MainWindow::updateWebCamState() const {
    QSignalBlocker blocker(ui->webCamCheckBox);
    ui->webCamCheckBox->setChecked(operate.getWebCamState());
}

void MainWindow::updateWebCamBlockState() const {
    QSignalBlocker blocker(ui->webcamBlockCheckBox);
    ui->webcamBlockCheckBox->setChecked(operate.getWebCamBlockState());
}

void MainWindow::updateFnSuperSwapState() const {
    QSignalBlocker blocker(ui->fnSuperSwapCheckBox);
    ui->fnSuperSwapCheckBox->setChecked(operate.getFnSuperSwapState());
}

void MainWindow::updateCoolerBoostState() const {
    const bool enabled = operate.getCoolerBoostState();
    QSignalBlocker blocker(ui->coolerBoostCheckBox);
    ui->coolerBoostCheckBox->setChecked(enabled);
    coolerBoostAction->setChecked(enabled);
}

void MainWindow::updateUserMode() {
    // Best effort: the raw-EC fallback in Operate needs this snapshot, but the
    // driver-backed getters below do not depend on it succeeding.
    (void)operate.updateEcData();
    {
        QSignalBlocker b1(ui->highPerformanceModeRadioButton);
        QSignalBlocker b2(ui->balancedModeRadioButton);
        QSignalBlocker b3(ui->silentModeRadioButton);
        QSignalBlocker b4(ui->superBatteryModeRadioButton);
        QSignalBlocker b5(ui->customModeRadioButton);

        switch (operate.getUserMode()) {
            case user_mode::balanced_mode:
                ui->balancedModeRadioButton->setChecked(true);
                balancedMode->setChecked(true);
                break;
            case user_mode::performance_mode:
                ui->highPerformanceModeRadioButton->setChecked(true);
                highPerformanceMode->setChecked(true);
                break;
            case user_mode::silent_mode:
                ui->silentModeRadioButton->setChecked(true);
                silentMode->setChecked(true);
                break;
            case user_mode::super_battery_mode:
                ui->superBatteryModeRadioButton->setChecked(true);
                superBatteryMode->setChecked(true);
                break;
            case user_mode::custom_mode:
                ui->customModeRadioButton->setChecked(true);
                break;
            case user_mode::unknown_mode:
                //fall to default, happens on some models after booting
            default:
                if (!operate.isMsiEcLoaded()){
                    ui->modeFormWidget->setDisabled(true);
                    if (modeTrayMenu)
                        modeTrayMenu->setDisabled(true);
                } else {
                    ui->superBatteryModeRadioButton->setChecked(false);
                    ui->silentModeRadioButton->setChecked(false);
                    ui->balancedModeRadioButton->setChecked(false);
                    ui->highPerformanceModeRadioButton->setChecked(false);
                    ui->customModeRadioButton->setChecked(false);
                }
                break;
        }
    }
}

void MainWindow::updateFanMode() {
    const QString name = operate.getFanModeName();
    QSignalBlocker blocker(ui->fanModeComboBox);
    const int index = ui->fanModeComboBox->findText(name);
    ui->fanModeComboBox->setCurrentIndex(index);
}

void MainWindow::updateShiftMode() {
    const QString name = operate.getShiftModeName();
    QSignalBlocker blocker(ui->shiftModeComboBox);
    const int index = ui->shiftModeComboBox->findText(name);
    ui->shiftModeComboBox->setCurrentIndex(index);
}

void MainWindow::updateFanSpeedSettings() {
    {
        QSignalBlocker blocker(ui->advancedFanControlCheckBox);
        ui->advancedFanControlCheckBox->setChecked(operate.getFanCurveActive());
    }
    ui->fanControlTabWidget->setEnabled(ui->advancedFanControlCheckBox->isChecked());

    QVector fan1SpeedSettings = operate.getFan1SpeedSettings();
    QVector fan1TempSettings = operate.getFan1TempSettings();
    QVector fan2SpeedSettings = operate.getFan2SpeedSettings();
    QVector fan2TempSettings = operate.getFan2TempSettings();

    ui->fan1Speed1Slider->setValue(fan1SpeedSettings[0]);
    ui->fan1Speed2Slider->setValue(fan1SpeedSettings[1]);
    ui->fan1Speed3Slider->setValue(fan1SpeedSettings[2]);
    ui->fan1Speed4Slider->setValue(fan1SpeedSettings[3]);
    ui->fan1Speed5Slider->setValue(fan1SpeedSettings[4]);
    ui->fan1Speed6Slider->setValue(fan1SpeedSettings[5]);
    ui->fan1Speed7Slider->setValue(fan1SpeedSettings[6]);

    ui->fan1Speed1TempLabel->setText(QString("< %1 °C").arg(fan1TempSettings[0]));
    ui->fan1Speed2TempSpinBox->setValue(fan1TempSettings[0]);
    ui->fan1Speed3TempSpinBox->setValue(fan1TempSettings[1]);
    ui->fan1Speed4TempSpinBox->setValue(fan1TempSettings[2]);
    ui->fan1Speed5TempSpinBox->setValue(fan1TempSettings[3]);
    ui->fan1Speed6TempSpinBox->setValue(fan1TempSettings[4]);
    ui->fan1Speed7TempSpinBox->setValue(fan1TempSettings[5]);

    ui->fan2Speed1Slider->setValue(fan2SpeedSettings[0]);
    ui->fan2Speed2Slider->setValue(fan2SpeedSettings[1]);
    ui->fan2Speed3Slider->setValue(fan2SpeedSettings[2]);
    ui->fan2Speed4Slider->setValue(fan2SpeedSettings[3]);
    ui->fan2Speed5Slider->setValue(fan2SpeedSettings[4]);
    ui->fan2Speed6Slider->setValue(fan2SpeedSettings[5]);
    ui->fan2Speed7Slider->setValue(fan2SpeedSettings[6]);

    ui->fan2Speed1TempLabel->setText(QString("< %1 °C").arg(fan2TempSettings[0]));
    ui->fan2Speed2TempSpinBox->setValue(fan2TempSettings[0]);
    ui->fan2Speed3TempSpinBox->setValue(fan2TempSettings[1]);
    ui->fan2Speed4TempSpinBox->setValue(fan2TempSettings[2]);
    ui->fan2Speed5TempSpinBox->setValue(fan2TempSettings[3]);
    ui->fan2Speed6TempSpinBox->setValue(fan2TempSettings[4]);
    ui->fan2Speed7TempSpinBox->setValue(fan2TempSettings[5]);

    checkFanSettingsChanged();
}

void MainWindow::setBestMobility() {
    applyResult(operate.setBatteryThreshold(100), tr("charge limit"));
    updateBatteryThreshold();
}

void MainWindow::setBalancedBattery() {
    applyResult(operate.setBatteryThreshold(80), tr("charge limit"));
    updateBatteryThreshold();
}

void MainWindow::setBestBattery() {
    applyResult(operate.setBatteryThreshold(60), tr("charge limit"));
    updateBatteryThreshold();
}

void MainWindow::setHighPerformanceMode() {
    applyResult(operate.setUserMode(user_mode::performance_mode), tr("performance mode"));
    updateUserMode();
}

void MainWindow::setBalancedMode() {
    applyResult(operate.setUserMode(user_mode::balanced_mode), tr("balanced mode"));
    updateUserMode();
}

void MainWindow::setSilentMode() {
    applyResult(operate.setUserMode(user_mode::silent_mode), tr("silent mode"));
    updateUserMode();
}

void MainWindow::setSuperBatteryMode() {
    applyResult(operate.setUserMode(user_mode::super_battery_mode), tr("super battery mode"));
    updateUserMode();
}

void MainWindow::setCoolerBoostState(bool enabled) const {
    applyResult(operate.setCoolerBoostState(enabled), tr("cooler boost"));
    if (operate.updateEcData())
        updateCoolerBoostState();
}

QVector<int> MainWindow::getFan1SpeedValues() const {
    QVector<int> fan1SpeedSettings;

    fan1SpeedSettings.push_back(ui->fan1Speed1Slider->value());
    fan1SpeedSettings.push_back(ui->fan1Speed2Slider->value());
    fan1SpeedSettings.push_back(ui->fan1Speed3Slider->value());
    fan1SpeedSettings.push_back(ui->fan1Speed4Slider->value());
    fan1SpeedSettings.push_back(ui->fan1Speed5Slider->value());
    fan1SpeedSettings.push_back(ui->fan1Speed6Slider->value());
    fan1SpeedSettings.push_back(ui->fan1Speed7Slider->value());

    return fan1SpeedSettings;
}

QVector<int> MainWindow::getFan2SpeedValues() const {
    QVector<int> fan2SpeedSettings;

    fan2SpeedSettings.push_back(ui->fan2Speed1Slider->value());
    fan2SpeedSettings.push_back(ui->fan2Speed2Slider->value());
    fan2SpeedSettings.push_back(ui->fan2Speed3Slider->value());
    fan2SpeedSettings.push_back(ui->fan2Speed4Slider->value());
    fan2SpeedSettings.push_back(ui->fan2Speed5Slider->value());
    fan2SpeedSettings.push_back(ui->fan2Speed6Slider->value());
    fan2SpeedSettings.push_back(ui->fan2Speed7Slider->value());

    return fan2SpeedSettings;
}

QVector<int> MainWindow::getFan1TempValues() const {
    QVector<int> fan1TempSettings;

    fan1TempSettings.push_back(ui->fan1Speed2TempSpinBox->value());
    fan1TempSettings.push_back(ui->fan1Speed3TempSpinBox->value());
    fan1TempSettings.push_back(ui->fan1Speed4TempSpinBox->value());
    fan1TempSettings.push_back(ui->fan1Speed5TempSpinBox->value());
    fan1TempSettings.push_back(ui->fan1Speed6TempSpinBox->value());
    fan1TempSettings.push_back(ui->fan1Speed7TempSpinBox->value());

    return fan1TempSettings;
}

QVector<int> MainWindow::getFan2TempValues() const {
    QVector<int> fan2TempSettings;

    fan2TempSettings.push_back(ui->fan2Speed2TempSpinBox->value());
    fan2TempSettings.push_back(ui->fan2Speed3TempSpinBox->value());
    fan2TempSettings.push_back(ui->fan2Speed4TempSpinBox->value());
    fan2TempSettings.push_back(ui->fan2Speed5TempSpinBox->value());
    fan2TempSettings.push_back(ui->fan2Speed6TempSpinBox->value());
    fan2TempSettings.push_back(ui->fan2Speed7TempSpinBox->value());

    return fan2TempSettings;
}

void MainWindow::setFanSpeedSettings() {
    bool ok = operate.setFan1SpeedSettings(getFan1SpeedValues());
    ok = operate.setFan1TempSettings(getFan1TempValues()) && ok;
    ok = operate.setFan2SpeedSettings(getFan2SpeedValues()) && ok;
    ok = operate.setFan2TempSettings(getFan2TempValues()) && ok;
    applyResult(ok, tr("fan curve"));
    if (operate.updateEcData())
        checkFanSettingsChanged();
}

void MainWindow::loadDefaultFanSettings() {
    const QVector<int> cpuSpeed{0, 40, 50, 60, 70, 80, 90};
    const QVector<int> cpuTemp{50, 56, 62, 70, 75, 80};
    const QVector<int> gpuSpeed{0, 45, 52, 59, 69, 79, 89};
    const QVector<int> gpuTemp{55, 60, 65, 70, 75, 80};

    const bool ok = operate.setFan1SpeedSettings(cpuSpeed) &&
                    operate.setFan1TempSettings(cpuTemp) &&
                    operate.setFan2SpeedSettings(gpuSpeed) &&
                    operate.setFan2TempSettings(gpuTemp);
    applyResult(ok, tr("default fan curve"));
    updateFanSpeedSettings();
}

void MainWindow::setFanModeAdvanced(bool enabled) const {
    const bool ok = operate.setFanModeAdvanced(enabled);
    applyResult(ok, tr("fan curve activation"));
    if (!ok)
        return;
    ui->fanControlTabWidget->setEnabled(enabled);
    ui->fanSpeedResetButton->setEnabled(enabled);
    ui->fanSpeedApplyButton->setEnabled(enabled);
}

void MainWindow::checkFanSettingsChanged() const {
    bool fanSettingChanged = (getFan1SpeedValues() != operate.getFan1SpeedSettings() ||
                              getFan2SpeedValues() != operate.getFan2SpeedSettings() ||
                              getFan1TempValues() != operate.getFan1TempSettings() ||
                              getFan2TempValues() != operate.getFan2TempSettings());
    const bool active = ui->advancedFanControlCheckBox->isChecked();
    ui->fanSpeedApplyButton->setEnabled(active && fanSettingChanged);
    ui->fanSpeedResetButton->setEnabled(active && fanSettingChanged);
}

void MainWindow::applyResult(bool ok, const QString &what) const {
    if (!ok)
        statusBar()->showMessage(tr("Failed to apply %1 — the EC rejected the value").arg(what), 5000);
}

void MainWindow::cycleProfile() {
    user_mode next = user_mode::balanced_mode;
    switch (operate.getUserMode()) {
        case user_mode::balanced_mode:
            next = user_mode::performance_mode;
            break;
        case user_mode::performance_mode:
            next = user_mode::silent_mode;
            break;
        case user_mode::silent_mode:
            next = user_mode::super_battery_mode;
            break;
        case user_mode::super_battery_mode:
        case user_mode::custom_mode:
        case user_mode::unknown_mode:
        default:
            next = user_mode::balanced_mode;
            break;
    }
    applyResult(operate.setUserMode(next), tr("profile"));
    updateUserMode();
}

void MainWindow::copyDiagnostics() const {
    const auto join = [](const QVector<int> &values) {
        QStringList out;
        for (int value : values)
            out << QString::number(value);
        return out.join(",");
    };

    QString text;
    QTextStream out(&text);
    out << "MControlCenter (ng)\n";
    out << "driver: " << operate.getDriverName() << '\n';
    out << "ec: " << QString::fromStdString(operate.getEcVersion()) << " / "
        << QString::fromStdString(operate.getEcBuild()) << '\n';
    out << "shift: " << operate.getShiftModeName()
        << "  fan: " << operate.getFanModeName()
        << "  curve: " << (operate.getFanCurveActive() ? "active" : "inactive") << '\n';
    out << "cpu: " << operate.getCpuTemp() << " C, " << operate.getFan1Duty() << " %, "
        << operate.getFan1Speed() << " rpm\n";
    const auto gpuTemp = operate.getGpuTemp();
    out << "gpu: " << (gpuTemp ? QString::number(*gpuTemp) : QString("-")) << " C, "
        << operate.getFan2Duty().value_or(0) << " %, "
        << operate.getFan2Speed().value_or(0) << " rpm\n";
    out << "cpu curve: " << join(operate.getFan1TempSettings()) << ':'
        << join(operate.getFan1SpeedSettings().mid(1)) << '\n';
    out << "gpu curve: " << join(operate.getFan2TempSettings()) << ':'
        << join(operate.getFan2SpeedSettings().mid(1)) << '\n';

    QGuiApplication::clipboard()->setText(text);
    statusBar()->showMessage(tr("Diagnostics copied to clipboard"), 2000);
}

void MainWindow::updateTrayTooltip() const {
    if (!trayIcon)
        return;

    const auto gpuTemp = operate.getGpuTemp();
    trayIcon->setToolTip(
        QString("MControlCenter\nCPU %1 °C · %2 rpm (%3 %)\nGPU %4 °C · %5 rpm (%6 %)\nShift: %7 · Fan: %8\nCurve: %9")
            .arg(operate.getCpuTemp())
            .arg(operate.getFan1Speed())
            .arg(operate.getFan1Duty())
            .arg(gpuTemp ? QString::number(*gpuTemp) : QString("-"))
            .arg(operate.getFan2Speed().value_or(0))
            .arg(operate.getFan2Duty().value_or(0))
            .arg(operate.getShiftModeName())
            .arg(operate.getFanModeName())
            .arg(operate.getFanCurveActive() ? tr("active") : tr("inactive")));
}

void MainWindow::showEvent(QShowEvent *event) {
    startRealtimeUpdate();
    activateWindow();
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if (trayIcon && trayIcon->isVisible()) {
        stopRealtimeUpdate();
        MainWindow::hide();
        event->ignore();
        return;
    }
    quitApp();
}

void MainWindow::quitApp() const {
    operate.closeHelperApp();
    Settings::setValue("MainWindow/Width", MainWindow::width());
    Settings::setValue("MainWindow/Height", MainWindow::height());
    (void) QCoreApplication::quit();
}

void MainWindow::timerSleepTimeout() {
    qint64 timeNow = QDateTime::currentMSecsSinceEpoch();
    if (timeLastWatcherInterval == 0) {
        timeLastWatcherInterval = timeNow;
        return;
    }
    qint64 msecsSinceTimeout = timeNow - timeLastWatcherInterval;
    timeLastWatcherInterval = timeNow;
    if (msecsSinceTimeout > timerSleepWatcher.interval() + 5000) {
        // Went to sleep for at least 5 seconds
        operate.handleWakeEvent();
    }
}

void MainWindow::setModeFromSelection(PowerProfile profile) {
    switch (profile) {
    case PowerProfile::Performance:
        setHighPerformanceMode();
        break;
    case PowerProfile::Balanced:
        setBalancedMode();
        break;
    case PowerProfile::Silent:
        setSilentMode();
        break;
    case PowerProfile::PowerSaver:
        setSuperBatteryMode();
        break;
    case PowerProfile::Unknown:
    default:;
    }
}

void MainWindow::on_ChargerStateChange(bool isCharging) {
    chargerConnected = isCharging;

    const bool gameRunning = ui->autoGameModeCheckBox->isChecked() && gameMonitor.isGameActive();
    const bool wantGameOverlay = gameRunning && gameModeAllowedNow();

    if (wantGameOverlay) {
        if (!gameModeActive)
            applyGameMode(gameMonitor.gameDescription());
        return;
    }

    const bool hadOverlay = gameModeActive;
    if (hadOverlay)
        restoreGameMode();

    if (ui->autoAcDcProfilesGroupBox->isChecked()) {
        // restoreGameMode() already applied the AC/DC base if an overlay was
        // active; otherwise apply the base for this charger state now.
        if (!hadOverlay)
            applyAcDcProfile();
    } else {
        ui->autoPPDCheckBox->setEnabled(1);
    }

    updateGameStatusLabel();
}

void MainWindow::on_PowerProfileChange(const PowerProfile profile) {
    if (ui->autoPPDCheckBox->isChecked()) {
        switch (profile) {
        case PowerProfile::Performance:
            setHighPerformanceMode();
            ui->highPerformanceModeRadioButton->setChecked(true);
            break;
        case PowerProfile::Balanced:
            setBalancedMode();
            ui->balancedModeRadioButton->setChecked(true);
            break;
        case PowerProfile::PowerSaver:
            setSuperBatteryMode();
            ui->superBatteryModeRadioButton->setChecked(true);
            break;
        case PowerProfile::Unknown:
            default:;
        }
    }
}

void MainWindow::onGameStateChanged(bool active, const QString &description) {
    if (active) {
        if (gameModeAllowedNow())
            applyGameMode(description);
    } else {
        restoreGameMode();
    }
    updateGameStatusLabel();
}

void MainWindow::applyGameMode(const QString &description) {
    if (gameModeActive || !gameModeAllowedNow())
        return;

    (void)operate.updateEcData();

    // PPD holds are tied to this D-Bus connection and are released when MCC
    // disconnects, so a crash cannot leave a stale performance profile.
    gameModePpdCookie = powerMonitor.holdPowerProfile(
            "performance", tr("Game running: %1").arg(description), "mcontrolcenter");

    if (ui->autoPPDCheckBox->isChecked() && gameModePpdCookie != 0) {
        // PPD will emit ActiveProfile=performance and on_PowerProfileChange()
        // applies the EC preset. Handle the case where PPD was already on
        // performance, in which case no change signal is emitted.
        if (operate.getUserMode() != user_mode::performance_mode)
            setHighPerformanceMode();
        gameModeManualEc = false;
        gameModeActive = true;
        statusBar()->showMessage(tr("Game mode: High Performance"), 3000);
        updateGameStatusLabel();
        return;
    }

    savedShiftMode = operate.getShiftModeName();
    savedFanMode = operate.getFanModeName();
    savedSuperBattery = operate.getSuperBatteryState();
    savedFanCurveActive = operate.getFanCurveActive();

    bool ok = operate.setShiftModeByName("turbo");
    ok = operate.setFanModeByName("auto") && ok;
    ok = operate.setSuperBatteryState(false) && ok;
    if (savedFanCurveActive)
        ok = operate.setFanCurveActive(false) && ok;

    gameModeManualEc = true;
    gameModeActive = true;

    updateUserMode();
    updateFanMode();
    updateShiftMode();
    updateFanSpeedSettings();
    updateTrayTooltip();

    if (ok)
        statusBar()->showMessage(tr("Game mode: High Performance"), 3000);
    else
        statusBar()->showMessage(tr("Game mode: High Performance was only partially applied"), 4000);

    updateGameStatusLabel();
}

void MainWindow::restoreGameMode() {
    if (!gameModeActive)
        return;

    if (gameModePpdCookie != 0) {
        powerMonitor.releasePowerProfile(gameModePpdCookie);
        gameModePpdCookie = 0;
        // Refresh the EC preset from PPD even if we missed the change signal.
        powerMonitor.queryPowerProfile();
    }

    if (ui->autoAcDcProfilesGroupBox->isChecked()) {
        // AC/DC is the base owner while the game overlay is inactive. Apply
        // the profile for the charger state at this moment.
        applyAcDcProfile();
    } else if (gameModeManualEc) {
        (void)operate.updateEcData();

        bool ok = operate.setShiftModeByName(savedShiftMode);
        ok = operate.setFanModeByName(savedFanMode) && ok;
        ok = operate.setSuperBatteryState(savedSuperBattery) && ok;
        if (savedFanCurveActive)
            ok = operate.setFanCurveActive(true) && ok;

        updateUserMode();
        updateFanMode();
        updateShiftMode();
        updateFanSpeedSettings();
        updateTrayTooltip();

        if (!ok)
            statusBar()->showMessage(tr("Could not fully restore the pre-game profile"), 4000);
    }

    gameModeActive = false;
    gameModeManualEc = false;
    updateGameStatusLabel();
}

bool MainWindow::gameModeAllowedNow() const {
    return ui->autoGameModeCheckBox->isChecked() &&
           (ui->gameModeOnBatteryCheckBox->isChecked() || chargerConnected);
}

void MainWindow::updateGameStatusLabel() {
    if (!gameMonitor.isGameActive()) {
        ui->gameModeValueLabel->setText(tr("Inactive"));
        return;
    }

    const QString description = gameMonitor.gameDescription();
    if (gameModeActive)
        ui->gameModeValueLabel->setText(tr("Active — %1").arg(description));
    else
        ui->gameModeValueLabel->setText(tr("Detected — %1, not applied").arg(description));
}

void MainWindow::applyAcDcProfile() {
    if (!ui->autoAcDcProfilesGroupBox->isChecked())
        return;

    Settings s;
    const int battery = s.getValue("Settings/UserModeOnBattery").toInt();
    const int charger = s.getValue("Settings/UserModeOnCharger").toInt();
    setModeFromSelection(static_cast<PowerProfile>(chargerConnected ? charger : battery));
}

void MainWindow::on_bestMobilityRadioButton_toggled(bool checked) {
    if (checked)
        setBestMobility();
}

void MainWindow::on_balancedBatteryRadioButton_toggled(bool checked) {
    if (checked)
        setBalancedBattery();
}

void MainWindow::on_bestBatteryRadioButton_toggled(bool checked) {
    if (checked)
        setBestBattery();
}

void MainWindow::on_customBatteryThresholdRadioButton_toggled(bool checked) {
    if (checked) {
        ui->customBatteryThresholdSpinBox->setEnabled(true);
    } else {
        ui->customBatteryThresholdSpinBox->setEnabled(false);
        ui->customBatteryApplyButton->setEnabled(false);
    }
}

void MainWindow::on_customBatteryThresholdSpinBox_valueChanged(int arg1) {
    ui->customBatteryApplyButton->setEnabled(
            ui->customBatteryThresholdRadioButton->isChecked() && operate.getBatteryThreshold() != arg1);
}

void MainWindow::on_customBatteryApplyButton_clicked() {
    applyResult(operate.setBatteryThreshold(ui->customBatteryThresholdSpinBox->value()),
                tr("charge limit"));
    updateBatteryThreshold();
}

void MainWindow::on_ReadValueButton_clicked() {
    bool ok = false;
    const int address = ui->addressEdit->displayText().trimmed().toInt(&ok, 0);
    if (!ok || address < 0 || address > 255) {
        statusBar()->showMessage(tr("Enter a valid EC address (0x00–0xFF)."), 4000);
        return;
    }
    ui->ValueSpinBox->setValue(operate.getValue(address));
}

void MainWindow::on_WriteValueButton_clicked() const {
    bool ok = false;
    const int address = ui->addressEdit->displayText().trimmed().toInt(&ok, 0);
    if (!ok || address < 0 || address > 255) {
        statusBar()->showMessage(tr("Enter a valid EC address (0x00–0xFF)."), 4000);
        return;
    }
    operate.setValue(address, ui->ValueSpinBox->value());
}

void MainWindow::on_usbPowerShareComboBox_currentIndexChanged(int index) const {
    const QString state = index <= 0 ? QString("off") : QString("state%1").arg(index - 1);
    applyResult(operate.setUsbPowerShareState(state), tr("USB power sharing"));
}

void MainWindow::on_qc3CheckBox_toggled(bool checked) const {
    applyResult(operate.setQc3State(checked), tr("Quick Charge"));
}

void MainWindow::on_panelCheckBox_toggled(bool checked) const {
    applyResult(operate.setPanelState(checked), tr("panel overdrive"));
}

void MainWindow::on_webCamCheckBox_clicked(bool checked) const {
    applyResult(operate.setWebCamState(checked), tr("webcam"));
    if (operate.updateEcData()) {
        updateWebCamState();
    }
}

void MainWindow::on_webcamBlockCheckBox_clicked(bool checked) const {
    applyResult(operate.setWebCamBlockState(checked), tr("webcam block"));
    if (operate.updateEcData()) {
        updateWebCamBlockState();
    }
}

void MainWindow::on_shiftModeComboBox_currentIndexChanged(int index) {
    const QString name = ui->shiftModeComboBox->itemText(index);
    applyResult(operate.setShiftModeByName(name), tr("shift mode"));
    updateUserMode();
}

void MainWindow::on_fanModeComboBox_currentIndexChanged(int index) {
    const QString name = ui->fanModeComboBox->itemText(index);
    applyResult(operate.setFanModeByName(name), tr("fan mode"));
    updateUserMode();
}

void MainWindow::on_fnSuperSwapCheckBox_clicked(bool checked) const {
    applyResult(operate.setFnSuperSwapState(checked), tr("Fn/Win swap"));
    updateFnSuperSwapState();
}

void MainWindow::on_coolerBoostCheckBox_clicked(bool checked) const {
    if (operate.getCoolerBoostState() != checked)
        setCoolerBoostState(checked);
}

void MainWindow::on_keyboardBrightnessSlider_valueChanged(int value) const {
    bool ok = true;
    if (value > 0)
        ok = operate.setKeyboardBacklightEnabled(true);
    ok = operate.setKeyboardBrightness(value) && ok;
    applyResult(ok, tr("keyboard brightness"));
    if (operate.updateEcData()) {
        updateKeyboardBrightness();
    }
}

void MainWindow::on_kbdBacklightEnableCheckBox_toggled(bool checked) const {
    applyResult(operate.setKeyboardBacklightEnabled(checked), tr("keyboard backlight"));
    if (operate.updateEcData()) {
        updateKeyboardBrightness();
    }
}

void MainWindow::on_keyboardBacklightModeComboBox_currentIndexChanged(int index) const {
    applyResult(operate.setKeyboardBacklightMode(index), tr("keyboard backlight mode"));
}

void MainWindow::on_userModeOnBatteryComboBox_currentIndexChanged(int index) const {
    Settings::setValue("Settings/UserModeOnBattery", index);
    powerMonitor.queryChargerState();
}

void MainWindow::on_userModeOnChargerComboBox_currentIndexChanged(int index) const {
    Settings::setValue("Settings/UserModeOnCharger", index);
    powerMonitor.queryChargerState();
}

void MainWindow::on_autoAcDcProfilesGroupBox_toggled(bool checked) {
    if(checked) {
        if (!powerMonitor.connectToUpower()) {
            QMessageBox::critical(nullptr, this->windowTitle(), tr("Couldn't connect to UPower to get charger status.\n"
                                                                   "Make sure that UPower is installed and running then restart the system."));
            ui->autoAcDcProfilesGroupBox->setChecked(0);
            ui->autoAcDcProfilesGroupBox->setEnabled(0);
            return;
        }

        // Game following and charger-based switching are layered: the game
        // overlay wins while it is active, then the AC/DC base is reapplied.
        powerMonitor.disconnectFromPowerProfiles();
        ui->autoPPDCheckBox->setChecked(0);
        ui->autoPPDCheckBox->setEnabled(0);
        powerMonitor.queryChargerState();
    } else {
        ui->autoPPDCheckBox->setEnabled(1);
        if (!ui->autoGameModeCheckBox->isChecked())
            powerMonitor.disconnectFromUpower();
    }

    Settings::setValue("Settings/autoAcDcProfilesState", checked);
}

void MainWindow::on_autoPPDCheckBox_toggled(bool checked) {
    if (gameModeActive)
        restoreGameMode();

    if (checked) {

        if (!powerMonitor.connectToPowerProfiles()) {
            QMessageBox::critical(nullptr, this->windowTitle(), tr("Couldn't connect to Power Profiles Daemon.\n"
                                                                   "Make sure that either Power Profiles Daemon or TuneD is installed and restart the system."));
            ui->autoPPDCheckBox->setChecked(0);
            return;
        }

        powerMonitor.disconnectFromUpower();
        ui->highPerformanceModeRadioButton->setEnabled(0);
        ui->balancedModeRadioButton->setEnabled(0);
        ui->silentModeRadioButton->setEnabled(0);
        ui->superBatteryModeRadioButton->setEnabled(0);
        ui->autoAcDcProfilesGroupBox->setChecked(0);
        ui->autoAcDcProfilesGroupBox->setEnabled(0);
        powerMonitor.queryPowerProfile();
    } else {
        ui->highPerformanceModeRadioButton->setEnabled(1);
        ui->balancedModeRadioButton->setEnabled(1);
        ui->silentModeRadioButton->setEnabled(1);
        ui->superBatteryModeRadioButton->setEnabled(1);
        ui->autoAcDcProfilesGroupBox->setEnabled(1);
    }
    Settings::setValue("Settings/autoPPDstate", checked);

    if (ui->autoGameModeCheckBox->isChecked() && gameMonitor.isGameActive())
        applyGameMode(gameMonitor.gameDescription());
}

void MainWindow::on_autoGameModeCheckBox_toggled(bool checked) {
    if (checked) {
        if (!powerMonitor.connectToUpower()) {
            statusBar()->showMessage(tr("Could not connect to UPower; assuming AC power for game mode."), 4000);
            chargerConnected = true;
        } else {
            powerMonitor.queryChargerState();
        }
        ui->gameModeOnBatteryCheckBox->setEnabled(true);
        gameMonitor.start();
    } else {
        gameMonitor.stop();
        restoreGameMode();
        ui->gameModeOnBatteryCheckBox->setEnabled(false);
        if (!ui->autoAcDcProfilesGroupBox->isChecked())
            powerMonitor.disconnectFromUpower();
    }
    Settings::setValue("Settings/autoGameModeState", checked);
}

void MainWindow::on_gameModeOnBatteryCheckBox_toggled(bool checked) {
    Settings::setValue("Settings/gameModeOnBatteryState", checked);

    if (!ui->autoGameModeCheckBox->isChecked() || !gameMonitor.isGameActive())
        return;

    if (gameModeAllowedNow() && !gameModeActive)
        applyGameMode(gameMonitor.gameDescription());
    else if (!gameModeAllowedNow() && gameModeActive)
        restoreGameMode();
}

void MainWindow::on_highPerformanceModeRadioButton_toggled(bool checked) {
    if (checked)
        setHighPerformanceMode();
}

void MainWindow::on_balancedModeRadioButton_toggled(bool checked) {
    if (checked)
        setBalancedMode();
}

void MainWindow::on_silentModeRadioButton_toggled(bool checked) {
    if (checked)
        setSilentMode();
}

void MainWindow::on_superBatteryModeRadioButton_toggled(bool checked) {
    if (checked)
        setSuperBatteryMode();
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason) {
    switch (reason) {
        case QSystemTrayIcon::Trigger:
        case QSystemTrayIcon::DoubleClick:
            if (!MainWindow::isHidden()) {
                MainWindow::close();
            } else {
                MainWindow::showNormal();
            }
            break;
        case QSystemTrayIcon::MiddleClick:
            break;
        default:;
    }
}

void MainWindow::createTrayIcon() {
    createActions();

    modeTrayMenu = new QMenu(tr("Mode"));
    modeTrayActions = new QActionGroup(this);
    modeTrayActions->setExclusive(true);


    modeTrayActions->addAction(highPerformanceMode);
    modeTrayActions->addAction(balancedMode);
    modeTrayActions->addAction(silentMode);
    modeTrayActions->addAction(superBatteryMode);

    modeTrayMenu->addAction(highPerformanceMode);
    modeTrayMenu->addAction(balancedMode);
    modeTrayMenu->addAction(silentMode);
    modeTrayMenu->addAction(superBatteryMode);

    fanTrayMenu = new QMenu(tr("Cooling"));
    fanTrayMenu->addAction(coolerBoostAction);

    batteryTrayMenu = new QMenu(tr("Charge limit"));
    batteryTrayMenu->addAction(bestMobilityAction);
    batteryTrayMenu->addAction(balancedBatteryAction);
    batteryTrayMenu->addAction(bestBatteryAction);

    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addMenu(modeTrayMenu);
    trayIconMenu->addMenu(fanTrayMenu);
    trayIconMenu->addMenu(batteryTrayMenu);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(cycleProfileAction);
    trayIconMenu->addAction(diagnosticsAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
    auto icon = QIcon(":/images/AppIcon");
    trayIcon->setIcon(icon);
    trayIcon->setToolTip("MControlCenter");

    trayIcon->show();

    connect(trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::iconActivated);
}

void MainWindow::createActions() {
    restoreAction = new QAction(tr("Show"), this);
    connect(restoreAction, &QAction::triggered, this, &MainWindow::showNormal);

    highPerformanceMode = new QAction(ui->highPerformanceModeRadioButton->text(), this);
    highPerformanceMode->setCheckable(true);

    balancedMode = new QAction(ui->balancedModeRadioButton->text(), this);
    balancedMode->setCheckable(true);

    silentMode = new QAction(ui->silentModeRadioButton->text(), this);
    silentMode->setCheckable(true);

    superBatteryMode = new QAction(ui->superBatteryModeRadioButton->text(), this);
    superBatteryMode->setCheckable(true);

    connect(highPerformanceMode, &QAction::triggered, this, &MainWindow::setHighPerformanceMode);
    connect(balancedMode, &QAction::triggered, this, &MainWindow::setBalancedMode);
    connect(silentMode, &QAction::triggered, this, &MainWindow::setSilentMode);
    connect(superBatteryMode, &QAction::triggered, this, &MainWindow::setSuperBatteryMode);

    coolerBoostAction = new QAction(tr("Cooler Boost"), this);
    coolerBoostAction->setCheckable(true);

    connect(coolerBoostAction, &QAction::triggered, this, &MainWindow::setCoolerBoostState);

    bestMobilityAction = new QAction(ui->bestMobilityRadioButton->text() + " (100%)", this);
    balancedBatteryAction = new QAction(ui->balancedBatteryRadioButton->text() + " (80%)", this);
    bestBatteryAction = new QAction(ui->bestBatteryRadioButton->text() + " (60%)", this);

    connect(bestMobilityAction, &QAction::triggered, this, &MainWindow::setBestMobility);
    connect(balancedBatteryAction, &QAction::triggered, this, &MainWindow::setBalancedBattery);
    connect(bestBatteryAction, &QAction::triggered, this, &MainWindow::setBestBattery);

    quitAction = new QAction(tr("Quit"), this);
    connect(quitAction, &QAction::triggered, this, &MainWindow::quitApp);

    cycleProfileAction = new QAction(tr("Next profile"), this);
    connect(cycleProfileAction, &QAction::triggered, this, &MainWindow::cycleProfile);

    diagnosticsAction = new QAction(tr("Copy diagnostics"), this);
    connect(diagnosticsAction, &QAction::triggered, this, &MainWindow::copyDiagnostics);
}

void MainWindow::saveStateRequest(QSessionManager &sessionManager) {
    sessionManager.setRestartHint(QSessionManager::RestartNever);
}
