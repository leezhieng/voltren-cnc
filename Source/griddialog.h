#ifndef GRIDDIALOG_H
#define GRIDDIALOG_H

#include "mainwindow.h"

#include <QDialog>

class MainWindow;

namespace Ui {
class GridDialog;
}

class GridDialog : public QDialog
{
	Q_OBJECT

public:
	explicit GridDialog(QWidget *parent = 0);
	~GridDialog();

	void init(MainWindow* window, int squareSize, int halfGridCount);

private slots:
	void on_updateButton_clicked();

private:
	Ui::GridDialog *ui;

	MainWindow* mainWindow;
};

#endif // GRIDDIALOG_H
