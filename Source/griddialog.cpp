#include "griddialog.h"
#include "ui_griddialog.h"

GridDialog::GridDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::GridDialog)
{
	ui->setupUi(this);

	this->setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
	this->setFixedSize(this->size());
}

GridDialog::~GridDialog()
{
	delete ui;
}

void GridDialog::init(MainWindow* window, int squareSize, int halfGridCount)
{
	mainWindow = window;

	ui->squareSizeInput->setValue(squareSize);
	ui->halfGridCountInput->setValue(halfGridCount);
}

void GridDialog::on_updateButton_clicked()
{
	mainWindow->simView->updateGrid(ui->squareSizeInput->value(), ui->halfGridCountInput->value());
}
