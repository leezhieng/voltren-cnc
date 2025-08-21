#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "aboutdialog.h"
#include "objectdialog.h"
#include "griddialog.h"
#include "axisdialog.h"

#include "view3d.h"

#include <QDebug>
#include <QMainWindow>
#include <QMessageBox>
#include <QScrollBar>

#include <QSerialPort>
#include <QSerialPortInfo>

#include <QFileDialog>
#include <QFileInfo>
#include <QDir>
#include <QFile>

enum programStateEnum { NONE, CONNECTION, COMMAND, GCODEFILE };

class ObjectDialog;
class GridDialog;
class AxisDialog;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

	void refreshPortList();
	void setConnectionUi();
	void closeSerialConnection();
	void sendCommand(QString command);
	void printConsole(QString output);

	void setup3DView();

	bool showGrid = true;

	view3D* simView;

private slots:
	void bytesWritten(qint64 bytes);
	void readyRead();
	void errorOccurred(QSerialPort::SerialPortError error);

	void on_refreshButton_clicked();
	void on_connectButton_clicked();

	void on_xNegButton_clicked();
	void on_yPosButton_clicked();
	void on_yNegButton_clicked();
	void on_xPosButton_clicked();
	void on_zPosButton_clicked();
	void on_zNegButton_clicked();
	void on_setOriginButton_clicked();

	void on_sendCommandButton_clicked();

	void on_gcodeOpenButton_clicked();
	void on_gcodeLoadButton_clicked();
	void on_gcodeSimulateButton_clicked();
	void on_gcodeSendButton_clicked();

	void on_actionAbout_triggered();

	void on_showGridInput_toggled(bool checked);
	void on_showObjectInput_toggled(bool checked);
	void on_showAxisInput_toggled(bool checked);

	void on_viewType_currentIndexChanged(int index);

	void on_editObjectButton_clicked();
	void on_editGridButton_clicked();
	void on_editAxisButton_clicked();

private:
    Ui::MainWindow *ui;

	QSerialPort *serialPort = nullptr;
	bool serialConnected = false;

	programStateEnum programState = programStateEnum::NONE;

	int gcodeCount = 0;
	int gcodeCurrent = 0;

	QString commandConsoleOutput;
};

#endif // MAINWINDOW_H
