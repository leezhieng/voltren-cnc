#ifndef OBJECTDIALOG_H
#define OBJECTDIALOG_H

#include "mainwindow.h"

#include <QDialog>

class MainWindow;

namespace Ui {
class ObjectDialog;
}

class ObjectDialog : public QDialog
{
	Q_OBJECT

public:
	explicit ObjectDialog(QWidget *parent = 0);
	~ObjectDialog();

	void init(MainWindow* window, int sizeX, int sizeY, int sizeZ);

private slots:
	void on_updateButton_clicked();

private:
	Ui::ObjectDialog *ui;

	MainWindow* mainWindow;
};

#endif // OBJECTDIALOG_H
