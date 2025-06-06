// This file is a part of "Candle" application.
// Copyright 2015-2016 Hayrullin Denis Ravilevich

#include "frmsettings.h"
#include "ui_frmsettings.h"
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QDebug>
#include <QScrollBar>
#include <QColorDialog>
#include <QHostAddress>
#include <QProcess>


frmSettings::frmSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::frmSettings)
{
    ui->setupUi(this);

    this->setLocale(QLocale::C);
    m_intValidator.setBottom(1);
    m_intValidator.setTop(999);
    ui->cboFps->setValidator(&m_intValidator);
    ui->cboFontSize->setValidator(&m_intValidator);

    foreach (QGroupBox *box, this->findChildren<QGroupBox*>()) {
        ui->listCategories->addItem(box->title());
        ui->listCategories->item(ui->listCategories->count() - 1)->setData(Qt::UserRole, box->objectName());
    }

    ui->listCategories->item(0)->setSelected(true);
    connect(ui->scrollSettings->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(onScrollBarValueChanged(int)));

    // Set input mask for IP address
    ui->txtIP->setInputMask("000.000.000.000");

    ui->hotkeysTree->setColumnCount(2);
    QStringList hotkeyLabels;
    hotkeyLabels.append("Key");
    hotkeyLabels.append("Function");
    ui->hotkeysTree->setHeaderLabels(hotkeyLabels);
}

frmSettings::~frmSettings()
{
    delete ui;
}

int frmSettings::exec()
{
    // Store settings to undo
    m_storedValues.clear();
    m_storedChecks.clear();
    m_storedCombos.clear();
    m_storedColors.clear();

    foreach (QAbstractSpinBox* sb, this->findChildren<QAbstractSpinBox*>())
    {
        m_storedValues.append(sb->property("value").toDouble());
    }

    foreach (QAbstractButton* cb, this->findChildren<QAbstractButton*>())
    {
        m_storedChecks.append(cb->isChecked());
    }

    foreach (QComboBox* cb, this->findChildren<QComboBox*>())
    {
        m_storedCombos.append(cb->currentText());
    }

    foreach (ColorPicker* pick, this->findChildren<ColorPicker*>())
    {
        m_storedColors.append(pick->color());
    }

    return QDialog::exec();
}

void frmSettings::undo()
{
    foreach (QAbstractSpinBox* sb, this->findChildren<QAbstractSpinBox*>())
    {
        sb->setProperty("value", m_storedValues.takeFirst());
    }

    foreach (QAbstractButton* cb, this->findChildren<QAbstractButton*>())
    {
        cb->setChecked(m_storedChecks.takeFirst());
    }
    foreach (QComboBox* cb, this->findChildren<QComboBox*>())
    {
        cb->setCurrentText(m_storedCombos.takeFirst());
    }
    foreach (ColorPicker* pick, this->findChildren<ColorPicker*>())
    {
        pick->setColor(m_storedColors.takeFirst());
    }
}

void frmSettings::on_listCategories_currentRowChanged(int currentRow)
{
    // Scroll to selected groupbox
    QGroupBox *box = this->findChild<QGroupBox*>(ui->listCategories->item(currentRow)->data(Qt::UserRole).toString());
    if (box) {
        ui->scrollSettings->ensureWidgetVisible(box);
    }
}

void frmSettings::onScrollBarValueChanged(int value)
{
    Q_UNUSED(value)

    // Search for first full visible groupbox
    for (int i = 0; i < ui->listCategories->count(); i++) {
        QGroupBox *box = this->findChild<QGroupBox*>(ui->listCategories->item(i)->data(Qt::UserRole).toString());
        if (box) {
            if (!box->visibleRegion().isEmpty() && box->visibleRegion().boundingRect().y() == 0) {
                // Select corresponding item in categories list
                ui->listCategories->setCurrentRow(i);
                return;
            }
        }
    }
}

QString frmSettings::IPAddress()
{
    return ui->txtIP->text();
}

void frmSettings::setIPAddress(QString ip)
{
    ui->txtIP->setText(ip);
}

unsigned int frmSettings::Port()
{
    return ui->txtPort->value();
}

void frmSettings::setPort(unsigned int port)
{
    ui->txtPort->setValue(port);
}

double frmSettings::toolDiameter()
{
    return ui->txtToolDiameter->value();
}

void frmSettings::setToolDiameter(double diameter)
{
    ui->txtToolDiameter->setValue(diameter);
}

double frmSettings::toolLength()
{
    return ui->txtToolLength->value();
}

void frmSettings::setToolLength(double length)
{
    ui->txtToolLength->setValue(length);
}

bool frmSettings::antialiasing()
{
    return ui->chkAntialiasing->isChecked();
}

void frmSettings::setAntialiasing(bool antialiasing)
{
    ui->chkAntialiasing->setChecked(antialiasing);
}

bool frmSettings::zBuffer()
{
    return ui->chkZBuffer->isChecked();
}

void frmSettings::setZBuffer(bool zBuffer)
{
    ui->chkZBuffer->setChecked(zBuffer);
}

double frmSettings::lineWidth()
{
    return ui->txtLineWidth->value();
}

void frmSettings::setLineWidth(double lineWidth)
{
    ui->txtLineWidth->setValue(lineWidth);
}

double frmSettings::arcLength()
{
    return ui->txtArcLength->value();
}

void frmSettings::setArcLength(double arcPrecision)
{
    ui->txtArcLength->setValue(arcPrecision);
}

double frmSettings::arcDegree()
{
    return ui->txtArcDegree->value();
}

void frmSettings::setArcDegree(double arcDegree)
{
    ui->txtArcDegree->setValue(arcDegree);
}

double frmSettings::arcPrecision()
{
    return ui->radArcDegreeMode->isChecked() ? ui->txtArcDegree->value() : ui->txtArcLength->value();
}

bool frmSettings::arcDegreeMode()
{
    return ui->radArcDegreeMode->isChecked();
}

void frmSettings::setArcDegreeMode(bool arcDegreeMode)
{
    ui->radArcDegreeMode->setChecked(arcDegreeMode);
}

bool frmSettings::showProgramCommands()
{
    return ui->chkShowProgramCommands->isChecked();
}

void frmSettings::setShowProgramCommands(bool showAllCommands)
{
    ui->chkShowProgramCommands->setChecked(showAllCommands);
}

bool frmSettings::showUICommands()
{
    return ui->chkShowUICommands->isChecked();
}

void frmSettings::setShowUICommands(bool showUICommands)
{
    ui->chkShowUICommands->setChecked(showUICommands);
}

QString frmSettings::safePositionCommand()
{
    return ui->txtSafeCommand->text();
}

void frmSettings::setSafePositionCommand(QString command)
{
    ui->txtSafeCommand->setText(command);
}

bool frmSettings::moveOnRestore()
{
    return ui->chkMoveOnRestore->isChecked();
}

void frmSettings::setMoveOnRestore(bool value)
{
    ui->chkMoveOnRestore->setChecked(value);
}

int frmSettings::restoreMode()
{
    return ui->cboRestoreMode->currentIndex();
}

void frmSettings::setRestoreMode(int value)
{
    ui->cboRestoreMode->setCurrentIndex(value);
}

int frmSettings::spindleSpeedMin()
{
    return ui->txtSpindleSpeedMin->value();
}

void frmSettings::setSpindleSpeedMin(int speed)
{
    ui->txtSpindleSpeedMin->setValue(speed);
}

int frmSettings::spindleSpeedMax()
{
    return ui->txtSpindleSpeedMax->value();
}

void frmSettings::setSpindleSpeedMax(int speed)
{
    ui->txtSpindleSpeedMax->setValue(speed);
}

int frmSettings::laserPowerMin()
{
    return ui->txtLaserPowerMin->value();
}

void frmSettings::setLaserPowerMin(int value)
{
    ui->txtLaserPowerMin->setValue(value);
}

int frmSettings::laserPowerMax()
{
    return ui->txtLaserPowerMax->value();
}

void frmSettings::setLaserPowerMax(int value)
{
    ui->txtLaserPowerMax->setValue(value);
}

int frmSettings::rapidSpeed()
{
    return ui->txtRapidSpeed->value();
}

void frmSettings::setRapidSpeed(int rapidSpeed)
{
    ui->txtRapidSpeed->setValue(rapidSpeed);
}

int frmSettings::heightmapProbingFeed()
{
    return ui->txtHeightMapProbingFeed->value();
}

void frmSettings::setHeightmapProbingFeed(int heightmapProbingFeed)
{
    ui->txtHeightMapProbingFeed->setValue(heightmapProbingFeed);
}

int frmSettings::acceleration()
{
    return ui->txtAcceleration->value();
}

void frmSettings::setAcceleration(int acceleration)
{
    ui->txtAcceleration->setValue(acceleration);
}

int frmSettings::queryStateTime()
{
    return ui->txtQueryStateTime->value();
}

void frmSettings::setQueryStateTime(int queryStateTime)
{
    ui->txtQueryStateTime->setValue(queryStateTime);
}

int frmSettings::toolType()
{
    return ui->cboToolType->currentIndex();
}

void frmSettings::setToolType(int toolType)
{
    ui->cboToolType->setCurrentIndex(toolType);
}

double frmSettings::toolAngle()
{
    return ui->txtToolAngle->value();
}

void frmSettings::setToolAngle(double toolAngle)
{
    ui->txtToolAngle->setValue(toolAngle);
}

int frmSettings::fps()
{
    return ui->cboFps->currentText().toInt();
}

void frmSettings::setFps(int fps)
{
    ui->cboFps->setCurrentText(QString::number(fps));
}

bool frmSettings::vsync()
{
    return ui->chkVSync->isChecked();
}

void frmSettings::setVsync(bool value)
{
    ui->chkVSync->setChecked(value);
}

bool frmSettings::msaa()
{
    return ui->radMSAA->isChecked();
}

void frmSettings::setMsaa(bool msaa)
{
    ui->radMSAA->setChecked(msaa);
}

bool frmSettings::autoCompletion()
{
    return ui->chkAutocompletion->isChecked();
}

void frmSettings::setAutoCompletion(bool autoCompletion)
{
    ui->chkAutocompletion->setChecked(autoCompletion);
}

int frmSettings::units()
{
    return ui->cboUnits->currentIndex();
}

void frmSettings::setUnits(int units)
{
    ui->cboUnits->setCurrentIndex(units);
}

QString frmSettings::touchCommand()
{
    return ui->txtTouchCommand->text();
}

void frmSettings::setTouchCommand(QString touchCommand)
{
    ui->txtTouchCommand->setText(touchCommand);
}

bool frmSettings::simplify()
{
    return ui->chkSimplify->isChecked();
}

void frmSettings::setSimplify(bool simplify)
{
    ui->chkSimplify->setChecked(simplify);
}

double frmSettings::simplifyPrecision()
{
    return ui->txtSimplifyPrecision->value();
}

void frmSettings::setSimplifyPrecision(double simplifyPrecision)
{
    ui->txtSimplifyPrecision->setValue(simplifyPrecision);
}

bool frmSettings::panelUserCommands()
{
    return ui->chkPanelUserCommands->isChecked();
}

void frmSettings::setPanelUserCommands(bool value)
{
    ui->chkPanelUserCommands->setChecked(value);
}

bool frmSettings::panelHeightmap()
{
    return ui->chkPanelHeightmap->isChecked();
}

void frmSettings::setPanelHeightmap(bool panelHeightmap)
{
    ui->chkPanelHeightmap->setChecked(panelHeightmap);
}

bool frmSettings::panelSpindle()
{
    return ui->chkPanelSpindle->isChecked();
}

void frmSettings::setPanelSpindle(bool panelSpindle)
{
    ui->chkPanelSpindle->setChecked(panelSpindle);
}

bool frmSettings::panelOverriding()
{
    return ui->chkPanelOverriding->isChecked();
}

void frmSettings::setPanelOverriding(bool panelFeed)
{
    ui->chkPanelOverriding->setChecked(panelFeed);
}

bool frmSettings::panelJog()
{
    return ui->chkPanelJog->isChecked();
}

void frmSettings::setPanelJog(bool panelJog)
{
    ui->chkPanelJog->setChecked(panelJog);
}

bool frmSettings::layoutCompact()
{
    return ui->chkLayoutCompact->isChecked();
}

void frmSettings::setLayoutCompact(bool compact)
{
    ui->chkLayoutCompact->setChecked(compact);
}

QList<ColorPicker *> frmSettings::colors()
{
    return this->findChildren<ColorPicker*>();
}

QColor frmSettings::colors(QString name)
{
    ColorPicker *pick = this->findChildren<ColorPicker*>("clp" + name).at(0);
    if (pick) return pick->color(); else return QColor();
}

int frmSettings::fontSize()
{
    return ui->cboFontSize->currentText().toInt();
}

void frmSettings::setFontSize(int fontSize)
{
    ui->cboFontSize->setCurrentText(QString::number(fontSize));
}

bool frmSettings::grayscaleSegments()
{
    return ui->chkGrayscale->isChecked();
}

void frmSettings::setGrayscaleSegments(bool value)
{
    ui->chkGrayscale->setChecked(value);
}

bool frmSettings::grayscaleSCode()
{
    return ui->radGrayscaleS->isChecked();
}

void frmSettings::setGrayscaleSCode(bool value)
{
    ui->radGrayscaleS->setChecked(value);
    ui->radGrayscaleZ->setChecked(!value);
}

bool frmSettings::drawModeVectors()
{
    return ui->radDrawModeVectors->isChecked();
}

void frmSettings::setDrawModeVectors(bool value)
{
    ui->radDrawModeVectors->setChecked(value);
    ui->radDrawModeRaster->setChecked(!value);
}

QString frmSettings::userCommands(int index)
{
    return this->findChild<QLineEdit*>(QString("txtUserCommand%1").arg(index))->text();
}

void frmSettings::setUserCommands(int index, QString commands)
{
    this->findChild<QLineEdit*>(QString("txtUserCommand%1").arg(index))->setText(commands);
}

bool frmSettings::ignoreErrors()
{
    return ui->chkIgnoreErrors->isChecked();
}

void frmSettings::setIgnoreErrors(bool value)
{
    ui->chkIgnoreErrors->setChecked(value);
}

bool frmSettings::autoLine()
{
    return ui->chkAutoLine->isChecked();
}

void frmSettings::setAutoLine(bool value)
{
    ui->chkAutoLine->setChecked(value);
}

bool frmSettings::UseM6()
{
    return ui->chkUseM6->isChecked();
}

void frmSettings::setUseM6(bool value)
{
    ui->chkUseM6->setChecked(value);
}

bool frmSettings::UseRotaryAxis()
{
    return  ui->chkEnableRotaryAxis->isChecked();
}

void frmSettings::setUseRotaryAxis(bool value)
{
    ui->chkEnableRotaryAxis->setChecked(value);
}

bool frmSettings::ResetAfterConnect()
{
    return  ui->chkResetConnect->isChecked();
}

void frmSettings::setResetAfterConnect(bool value)
{
    ui->chkResetConnect->setChecked(value);
}

QColor frmSettings::tool()
{
    return ui->clpTool->color();
}

void frmSettings::setTool(const QColor& color)
{
    ui->clpTool->setColor(color);
}

QColor frmSettings::visualizerBackground()
{
    return ui->clpVisualizerBackground->color();
}

void frmSettings::setVisualizerBackground(const QColor& color)
{
    ui->clpVisualizerBackground->setColor(color);
}

QColor frmSettings::visualizerText()
{
    return ui->clpVisualizerText->color();
}

void frmSettings::setVisualizerText(const QColor& color)
{
    ui->clpVisualizerText->setColor(color);
}

QColor frmSettings::toolpathNormal()
{
    return ui->clpToolpathNormal->color();
}

void frmSettings::setToolpathNormal(const QColor& color)
{
    ui->clpToolpathNormal->setColor(color);
}

QColor frmSettings::toolpathDrawn()
{
    return ui->clpToolpathDrawn->color();
}

void frmSettings::setToolpathDrawn(const QColor& color)
{
    ui->clpToolpathDrawn->setColor(color);
}

QColor frmSettings::toolpathHighlight()
{
    return ui->clpToolpathHighlight->color();
}

void frmSettings::setToolpathHighlight(const QColor& color)
{
    ui->clpToolpathHighlight->setColor(color);
}

QColor frmSettings::toolpathZMovement()
{
    return ui->clpToolpathZMovement->color();
}

void frmSettings::setToolpathZMovement(const QColor& color)
{
    ui->clpToolpathZMovement->setColor(color);
}

QColor frmSettings::toolpathStart()
{
    return ui->clpToolpathStart->color();
}

void frmSettings::setToolpathStart(const QColor& color)
{
    ui->clpToolpathStart->setColor(color);
}

QColor frmSettings::toolpathEnd()
{
    return ui->clpToolpathEnd->color();
}

void frmSettings::setToolpathEnd(const QColor& color)
{
    ui->clpToolpathEnd->setColor(color);
}

void frmSettings::setHotkeys(const QStringList& hotkeys)
{
    for (QString hotkey : hotkeys)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem(ui->hotkeysTree);
        QStringList parts = hotkey.split(':');
        if (parts.size() == 2)
        {
            item->setText(0, parts[0]);
            item->setText(1, parts[1]);
        }
        ui->hotkeysTree->addTopLevelItem(item);
    }
}

void frmSettings::showEvent(QShowEvent *se)
{
    Q_UNUSED(se)

    ui->scrollSettings->updateMinimumWidth();
}


void frmSettings::on_cmdRefresh_clicked()
{
}


void frmSettings::on_cmdOK_clicked()
{
    this->accept();
}


void frmSettings::on_cmdCancel_clicked()
{
    this->reject();
}


void frmSettings::on_cboToolType_currentIndexChanged(int index)
{
    ui->lblToolAngle->setEnabled(index == 1);
    ui->txtToolAngle->setEnabled(index == 1);
}


void frmSettings::on_cmdDefaults_clicked()
{
    if (QMessageBox::warning(this, qApp->applicationDisplayName(), tr("Reset settings to default values?"),
                             QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel) != QMessageBox::Yes) return;

    setIgnoreErrors(false);

    setIPAddress("192.168.1.20");

    setQueryStateTime(40);
    setRapidSpeed(2000);
    setAcceleration(100);
    setSpindleSpeedMin(0);
    setSpindleSpeedMax(10000);
    setLaserPowerMin(0);
    setLaserPowerMax(100);
    setTouchCommand("G21G91G38.2Z-30F100; G0Z1; G38.2Z-2F10");
    setSafePositionCommand("G21G90; G53G0Z0");
    setMoveOnRestore(false);
    setRestoreMode(0);
    setHeightmapProbingFeed(10);
    setUnits(0);

    setArcLength(0.0);
    setArcDegreeMode(true);
    setArcDegree(5.0);

    setLineWidth(1.5);
    setAntialiasing(true);
    setMsaa(true);
    setSimplify(true);
    setSimplifyPrecision(0.0);
    setFps(60);
    setZBuffer(false);
    setGrayscaleSegments(false);
    setGrayscaleSCode(true);
    setDrawModeVectors(true);

    setToolType(1);
    setToolAngle(15.0);
    setToolDiameter(3.0);
    setToolLength(30.0);

    setShowProgramCommands(false);
    setAutoCompletion(true);

    setPanelOverriding(true);
    setPanelHeightmap(true);
    setPanelJog(true);
    setPanelSpindle(true);   

    ui->clpTool->setColor(QColor(255, 153, 0));

    ui->clpVisualizerBackground->setColor(QColor(255, 255, 255));
    ui->clpVisualizerText->setColor(QColor(0, 0, 0));

    ui->clpToolpathNormal->setColor(QColor(0, 0, 0));
    ui->clpToolpathDrawn->setColor(QColor(217, 217, 217));
    ui->clpToolpathHighlight->setColor(QColor(145, 130, 230));
    ui->clpToolpathZMovement->setColor(QColor(255, 0, 0));
    ui->clpToolpathStart->setColor(QColor(255, 0, 0));
    ui->clpToolpathEnd->setColor(QColor(0, 255, 0));

    setFontSize(9);
}

void frmSettings::on_cboFontSize_currentTextChanged(const QString &arg1)
{
    qApp->setStyleSheet(QString(qApp->styleSheet()).replace(QRegExp("font-size:\\s*\\d+"), "font-size: " + arg1));
}

void frmSettings::on_radDrawModeVectors_toggled(bool checked)
{
    ui->chkSimplify->setEnabled(checked);
    ui->lblSimpilyPrecision->setEnabled(checked && ui->chkSimplify->isChecked());
    ui->txtSimplifyPrecision->setEnabled(checked && ui->chkSimplify->isChecked());

    ui->radDrawModeRaster->setChecked(!checked);
}

void frmSettings::on_radDrawModeRaster_toggled(bool checked)
{
    ui->radDrawModeVectors->setChecked(!checked);
}

void frmSettings::on_radGrayscaleS_toggled(bool checked)
{
    ui->radGrayscaleZ->setChecked(!checked);
}

void frmSettings::on_radGrayscaleZ_toggled(bool checked)
{
    ui->radGrayscaleS->setChecked(!checked);
}

void frmSettings::on_btnTestNetwork_clicked()
{
    QHostAddress ip;

    if(ip.setAddress(ui->txtIP->text()))
    {
        //qDebug() << "IP: " << ip.toString() << " is valid";

        QStringList args;
#if defined(Q_OS_LINUX)
        args << "-c 1 -W 1";
#elif defined(Q_OS_WIN)
        args << "-n 1 -w 1500";
#endif
        args << ip.toString();

        int exit = QProcess::execute("ping", args);
        if(exit == 0)
        {
            QMessageBox::information(this, qApp->applicationDisplayName(), tr("Connection OK!"));
        }
        else
        {
            QMessageBox::information(this, qApp->applicationDisplayName(), tr("Target not found!"));
        }
    }
    else
    {
        qDebug() << "IP is not valid";
        QMessageBox::information(this, qApp->applicationDisplayName(), tr("IP invalid!"));
    }
}
