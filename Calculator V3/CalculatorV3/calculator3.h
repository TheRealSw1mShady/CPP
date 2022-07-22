#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class Calculator; }
QT_END_NAMESPACE

class Calculator : public QWidget
{
    Q_OBJECT

public:
    Calculator(QWidget *parent = 0);
    ~Calculator();

private slots:
    void on_Button_1_clicked();

    void on_Button_2_clicked();

    void on_Button_Plus_clicked();

    void on_Button_Clear_clicked();

    void on_Button_3_clicked();

    void on_Button_4_clicked();

    void on_Button_5_clicked();

    void on_Button_6_clicked();

    void on_Button_7_clicked();

    void on_Button_8_clicked();

    void on_Button_9_clicked();

    void on_Button_0_clicked();

    void on_Button_Minus_clicked();

    void on_Button_Multi_clicked();

    void on_Button_Div_clicked();

    void on_Button_Result_clicked();

    void EnterNumber(QString a, QString num);

    void EnterSign(QString a, QString sign);

    void on_Button_Delete_clicked();

    void on_Button_Point_clicked();

private:
    Ui::Calculator *ui;
};
#endif // CALCULATOR_H
