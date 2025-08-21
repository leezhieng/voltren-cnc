#ifndef AXISDIALOG_H
#define AXISDIALOG_H

#include "mainwindow.h"

#include <QDialog>

class MainWindow;

namespace Ui {
class AxisDialog;
}

class AxisDialog : public QDialog
{
	Q_OBJECT

public:
	explicit AxisDialog(QWidget *parent = 0);
	~AxisDialog();

	void init(MainWindow* window, int size);

private slots:
	void on_updateButton_clicked();

private:
	Ui::AxisDialog *ui;

	MainWindow* mainWindow;
};

#endif // AXISDIALOG_H
