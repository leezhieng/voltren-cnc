#include "axisdialog.h"
#include "ui_axisdialog.h"

AxisDialog::AxisDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::AxisDialog)
{
	ui->setupUi(this);

	this->setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
	this->setFixedSize(this->size());
}

AxisDialog::~AxisDialog()
{
	delete ui;
}

void AxisDialog::init(MainWindow* window, int size)
{
	mainWindow = window;
	ui->sizeInput->setValue(size);
}

void AxisDialog::on_updateButton_clicked()
{
	mainWindow->simView->updateAxis(ui->sizeInput->value());
}
