#include "objectdialog.h"
#include "ui_objectdialog.h"

ObjectDialog::ObjectDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::ObjectDialog)
{
	ui->setupUi(this);

	this->setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
	this->setFixedSize(this->size());
}

ObjectDialog::~ObjectDialog()
{
	delete ui;
}

void ObjectDialog::init(MainWindow* window, int sizeX, int sizeY, int sizeZ)
{
	mainWindow = window;
	ui->sizeXInput->setValue(sizeX);
	ui->sizeYInput->setValue(sizeY);
	ui->sizeZInput->setValue(sizeZ);
}

void ObjectDialog::on_updateButton_clicked()
{
	mainWindow->simView->updateObject(ui->sizeXInput->value(), ui->sizeYInput->value(), ui->sizeZInput->value());
}
