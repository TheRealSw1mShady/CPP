#include "calculator.h"
#include "ui_calculator.h"

Calculator::Calculator(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Calculator)
{
    ui->setupUi(this);
    ui->lineEdit->setText("0");
}

Calculator::~Calculator()
{
    delete ui;
}


void Calculator::on_button_1_clicked()
{
    ui->lineEdit->setText(ui->lineEdit->text() + "1");
}


void Calculator::on_lineEdit_textChanged()
{
    if (ui->lineEdit->text().length() > 1)
    {
        QString byteString = ui->lineEdit->text().toLatin1();

        if (byteString.at(0) == '0' and byteString.at(0) != '.')
            ui->lineEdit->setText(byteString.remove(0, 1));
    }
}


void Calculator::on_button_2_clicked()
{
    ui->lineEdit->setText(ui->lineEdit->text() + "2");
}

void Calculator::on_button_3_clicked()
{
    ui->lineEdit->setText(ui->lineEdit->text() + "3");
}

void Calculator::on_button_4_clicked()
{
    ui->lineEdit->setText(ui->lineEdit->text() + "4");
}

void Calculator::on_button_5_clicked()
{
    ui->lineEdit->setText(ui->lineEdit->text() + "5");
}

void Calculator::on_button_6_clicked()
{
    ui->lineEdit->setText(ui->lineEdit->text() + "6");
}

void Calculator::on_button_7_clicked()
{
    ui->lineEdit->setText(ui->lineEdit->text() + "7");
}

void Calculator::on_button_8_clicked()
{
    ui->lineEdit->setText(ui->lineEdit->text() + "8");
}

void Calculator::on_button_9_clicked()
{
    ui->lineEdit->setText(ui->lineEdit->text() + "9");
}

void Calculator::on_button_0_clicked()
{
    ui->lineEdit->setText(ui->lineEdit->text() + "0");
}

void Calculator::on_button_point_clicked()
{
    if (ui->lineEdit->text().toLatin1().indexOf('.') == -1)
        ui->lineEdit->setText(ui->lineEdit->text() + ".");
}

void Calculator::on_button_clear_clicked()
{
    ui->lineEdit->setText("0");
}

