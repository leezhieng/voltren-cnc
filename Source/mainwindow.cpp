#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

	setup3DView();

	ui->operationTabPanel->setCurrentIndex(0);

	setConnectionUi();
	refreshPortList();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::refreshPortList()
{
	ui->portSelection->clear();

	const auto portList = QSerialPortInfo::availablePorts();
	for (const QSerialPortInfo &info : portList)
	{
		ui->portSelection->addItem(info.portName());
	}
}

void MainWindow::setConnectionUi()
{
	if (serialConnected)
	{
		ui->connectionPanel->setEnabled(false);
		ui->machineControlPanel->setEnabled(true);

		ui->sendCommandButton->setEnabled(true);
		ui->gcodeSendButton->setEnabled(true);

		ui->connectionStatusLabel->setText("Connected");
		ui->connectButton->setText("Disconnect");
	}
	else
	{
		ui->connectionPanel->setEnabled(true);
		ui->machineControlPanel->setEnabled(false);

		ui->sendCommandButton->setEnabled(false);
		ui->gcodeSendButton->setEnabled(false);

		ui->connectionStatusLabel->setText("Disconnected");
		ui->connectButton->setText("Connect");
	}

	ui->connectButton->setEnabled(true);

	ui->progressBar->setValue(0);

	ui->gcodeTotalLabel->setText(QString::number(gcodeCount));
	ui->gcodeCurrentLabel->setText(QString::number(gcodeCurrent));
}

void MainWindow::closeSerialConnection()
{
	if (serialPort->isOpen())
		serialPort->close();

	serialConnected = false;
	setConnectionUi();
}

void MainWindow::sendCommand(QString command)
{
	if (!command.isEmpty())
	{
		command.append("\r");

		QByteArray data = command.toUtf8();
		serialPort->write(data);

		//qDebug() << data;

		printConsole(">> " + QString(data));

		if (serialPort->bytesToWrite() > 0)
			serialPort->flush();

		ui->commandInput->clear();
	}
}

void MainWindow::printConsole(QString output)
{
	commandConsoleOutput.append(output);
	ui->commandConsole->setPlainText(commandConsoleOutput);
	ui->commandConsole->verticalScrollBar()->setValue(ui->commandConsole->verticalScrollBar()->maximum());
}

void MainWindow::setup3DView()
{
	simView = new view3D(this);
	simView->setMouseTracking(true);

	if (ui->viewType->currentIndex() == 0)
		simView->enablePerspective(true);
	else
		simView->enablePerspective(false);

	simView->setDrawGrid(ui->showGridInput->isChecked());
	simView->setDrawObject(ui->showObjectInput->isChecked());
	simView->setDrawAxis(ui->showAxisInput->isChecked());

	ui->simulationLayout->layout()->addWidget(simView);
}

void MainWindow::bytesWritten(qint64 bytes)
{
	//qDebug() << bytes;
}

void MainWindow::readyRead()
{
	QByteArray data = serialPort->readAll();
	qDebug() << data;

	QString dataString = QString(data);

	if (programState == programStateEnum::CONNECTION)
	{
		if (!dataString.startsWith("\r\n"))
		{
			serialPort->close();
			serialConnected = false;
			setConnectionUi();

			QMessageBox::warning(this, "No Response", "Please try another baud rate.");
		}
		else
		{
			programState == programStateEnum::NONE;

			serialConnected = true;
			setConnectionUi();

			printConsole(dataString.remove("\r\n").append("\r\n"));

			QMessageBox::information(this, "Connected", "Connected to machine.");
		}
	}
	else if (programState == programStateEnum::COMMAND)
	{
		printConsole(data);
	}
}

void MainWindow::errorOccurred(QSerialPort::SerialPortError error)
{
	if (error == QSerialPort::NoError)
	{
		//qDebug() << "No error occured";
	}
	else if (error == QSerialPort::DeviceNotFoundError)
	{
		qDebug() << "An error occured while attempting to open an non-existing device.";

		QMessageBox::warning(this, "Not Found", "Serial port not found. Please click the 'Refresh' button.");
	}
	else if (error == QSerialPort::PermissionError)
	{
		qDebug() << "An error occured while attempting to open an already opened device by another process or a user not having enough permission and credential to open.";
	}
	else if (error == QSerialPort::OpenError)
	{
		qDebug() << "An error occured while attempting to open an already opened device in this object.";
	}
	else if (error == QSerialPort::NotOpenError)
	{
		qDebug() << "This error occured when an operation is executed that can only be successfully performed if the device is open.";
	}
	else if (error == QSerialPort::ParityError)
	{
		qDebug() << "Parity error detected by the hardware while reading data. This value is obsolete. We strongly advise against using it in new code.";
	}
	else if (error == QSerialPort::FramingError)
	{
		qDebug() << "Framing error detected by the hardware while reading data. This value is obsolete. We strongly advise against using it in new code.";
	}
	else if (error == QSerialPort::BreakConditionError)
	{
		qDebug() << "Break condition detected by the hardware on the input line. This value is obsolete. We strongly advise against using it in new code.";
	}
	else if (error == QSerialPort::WriteError)
	{
		qDebug() << "An I/O error occurred while writing the data.";
	}
	else if (error == QSerialPort::ReadError)
	{
		qDebug() << "An I/O error occurred while reading the data.";
	}
	else if (error == QSerialPort::ResourceError)
	{
		qDebug() << "An I/O error occurred when a resource becomes unavailable, e.g. when the device is unexpectedly removed from the system.";
		closeSerialConnection();
	}
	else if (error == QSerialPort::UnsupportedOperationError)
	{
		qDebug() << "The requested device operation is not supported or prohibited by the running operating system.";
	}
	else if (error == QSerialPort::TimeoutError)
	{
		qDebug() << "A timeout error occurred.";
	}
	else if (error == QSerialPort::UnknownError)
	{
		qDebug() << "An unidentified error occurred.";
	}

	serialConnected = false;
	setConnectionUi();
}

void MainWindow::on_refreshButton_clicked()
{
	refreshPortList();
}

void MainWindow::on_connectButton_clicked()
{
	programState = programStateEnum::CONNECTION;

	QString portName = ui->portSelection->currentText();

	if (!serialConnected)
	{
		if (!portName.isEmpty())
		{
			serialPort = new QSerialPort(this);
			serialPort->setPortName(portName);
			serialPort->setBaudRate(ui->baudSelection->currentText().toInt());
			serialPort->setParity(QSerialPort::NoParity);
			serialPort->setDataBits(QSerialPort::Data8);
			serialPort->setFlowControl(QSerialPort::NoFlowControl);
			serialPort->setStopBits(QSerialPort::OneStop);

			connect(serialPort, &QSerialPort::bytesWritten, this, &MainWindow::bytesWritten);
			connect(serialPort, &QSerialPort::readyRead, this, &MainWindow::readyRead);
			connect(serialPort, &QSerialPort::errorOccurred, this, &MainWindow::errorOccurred);

			bool success = serialPort->open(QIODevice::ReadWrite);

			if (success)
			{
				ui->connectionPanel->setEnabled(false);
				ui->connectButton->setEnabled(false);
				ui->connectButton->setText("Connecting...");
			}
		}
		else
		{
			QMessageBox::warning(this, "Not Found", "No active serial port is found.");
		}
	}
	else
	{
		closeSerialConnection();
	}
}

void MainWindow::on_xNegButton_clicked()
{
	programState = programStateEnum::COMMAND;

	QString speed = QString::number(ui->stepSizeInput->value());
	QString command = "G91 G0 X-" + speed;
	sendCommand(command);
}

void MainWindow::on_yPosButton_clicked()
{
	programState = programStateEnum::COMMAND;

	QString speed = QString::number(ui->stepSizeInput->value());
	QString command = "G91 G0 Y" + speed;
	sendCommand(command);
}

void MainWindow::on_yNegButton_clicked()
{
	programState = programStateEnum::COMMAND;

	QString speed = QString::number(ui->stepSizeInput->value());
	QString command = "G91 G0 Y-" + speed;
	sendCommand(command);
}

void MainWindow::on_xPosButton_clicked()
{
	programState = programStateEnum::COMMAND;

	QString speed = QString::number(ui->stepSizeInput->value());
	QString command = "G91 G0 X" + speed;
	sendCommand(command);
}

void MainWindow::on_zPosButton_clicked()
{
	programState = programStateEnum::COMMAND;

	QString speed = QString::number(ui->stepSizeInput->value());
	QString command = "G91 G0 Z" + speed;
	sendCommand(command);
}

void MainWindow::on_zNegButton_clicked()
{
	programState = programStateEnum::COMMAND;

	QString speed = QString::number(ui->stepSizeInput->value());
	QString command = "G91 G0 Z-" + speed;
	sendCommand(command);
}

void MainWindow::on_setOriginButton_clicked()
{
}

void MainWindow::on_sendCommandButton_clicked()
{
	programState = programStateEnum::COMMAND;

	QString command = ui->commandInput->text().simplified();
	sendCommand(command);
}

void MainWindow::on_gcodeOpenButton_clicked()
{
	QString fileName = QFileDialog::getOpenFileName(this, "Open File", qApp->applicationDirPath(), "GCode (*.gcode)");
	ui->gcodeFileInput->setText(fileName);
}

void MainWindow::on_gcodeLoadButton_clicked()
{
	QString fileName = ui->gcodeFileInput->text().simplified();
	if (!fileName.isEmpty() && QFileInfo(fileName).exists() && !QDir(fileName).exists())
	{
		ui->gcodeListDisplay->clear();
		gcodeCount = 0;
		gcodeCurrent = 0;

		QFile inputFile(fileName);
		if (inputFile.open(QIODevice::ReadOnly))
		{
			QTextStream in(&inputFile);
			while (!in.atEnd())
			{
				QString line = in.readLine();

				if (line.isEmpty() || line.startsWith(";"))
				{
					//qDebug() << "Ignore " + line;
				}
				else
				{
					//qDebug() << line;

					gcodeCount++;

					QTreeWidgetItem* item = new QTreeWidgetItem();
					item->setData(0, 0, QString::number(gcodeCount));
					item->setData(1, 0, line);
					item->setData(2, 0, "No");
					item->setData(3, 0, "");

					ui->gcodeListDisplay->addTopLevelItem(item);
				}
			}
			inputFile.close();

			ui->gcodeListDisplay->resizeColumnToContents(0);
			ui->gcodeListDisplay->resizeColumnToContents(1);
			ui->gcodeListDisplay->resizeColumnToContents(2);

			setConnectionUi();
		}
	}
	else
	{
		QMessageBox::warning(this, "Invalid File", "Please select a valid GCode file.");
	}
}

void MainWindow::on_gcodeSimulateButton_clicked()
{
	simView->clearAllGCode();

	if (gcodeCount > 0)
	{
		for (int i = 0; i < gcodeCount; i++)
		{
			QString gcode = ui->gcodeListDisplay->topLevelItem(i)->data(1, 0).toString();

			qDebug() << gcode;

			simView->addGCode(gcode);
		}
	}
}

void MainWindow::on_gcodeSendButton_clicked()
{
	programState = programStateEnum::GCODEFILE;

	if (gcodeCount > 0)
	{
		ui->sendCommandButton->setEnabled(false);
		ui->machineControlPanel->setEnabled(false);
	}
	else
	{
		QMessageBox::warning(this, "Empty Command", "Nothing to send. Please load a GCode file.");
	}
}

void MainWindow::on_actionAbout_triggered()
{
	AboutDialog* dialog = new AboutDialog(this);
	dialog->show();
}

void MainWindow::on_showGridInput_toggled(bool checked)
{
	simView->setDrawGrid(checked);
}

void MainWindow::on_showObjectInput_toggled(bool checked)
{
	simView->setDrawObject(checked);
}

void MainWindow::on_showAxisInput_toggled(bool checked)
{
	simView->setDrawAxis(checked);
}

void MainWindow::on_viewType_currentIndexChanged(int index)
{
	if (index == 0)
	{
		// Persp
		simView->enablePerspective(true);
		simView->update();

		//qDebug() << "persp";
	}
	else
	{
		// Ortho
		simView->enablePerspective(false);
		simView->update();

		//qDebug() << "ortho";
	}
}

void MainWindow::on_editObjectButton_clicked()
{
	ObjectDialog* dialog = new ObjectDialog(this);
	dialog->init(this, simView->objectSizeX, simView->objectSizeY, simView->objectSizeZ);
	dialog->show();
}

void MainWindow::on_editGridButton_clicked()
{
	GridDialog* dialog = new GridDialog(this);
	dialog->init(this, simView->gridSquareSize, simView->gridHalfGridCount);
	dialog->show();
}

void MainWindow::on_editAxisButton_clicked()
{
	AxisDialog* dialog = new AxisDialog(this);
	dialog->init(this, simView->axisSize);
	dialog->show();
}
