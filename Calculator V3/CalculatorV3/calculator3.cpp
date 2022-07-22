#include "calculator3.h"
#include "ui_calculator3.h"
#include <QString>
#include <iostream>
#include <Qvector>

using namespace std;

Calculator::Calculator(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Calculator)
{
    ui->setupUi(this);
}

Calculator::~Calculator()
{
    delete ui;
}

void Calculator::EnterNumber(QString a, QString num)
{
    // Функция принимает на вход выражение и число num

    int len = a.size();

    //Если в строке ноль, то при нажатии выведется число num

    if (a == "0")
    {
        if (num == ".")
        {
            ui->lineEdit->setText(a + ".");
        }
        else
        {
            ui->lineEdit->setText(num);
        }
    }
    else
    {
        //если вводить любое число после "+0" "/0" и т.п., ноль будет заменяться на число

        if ((a[len-1] == '0') && (a[len-2] == '+' || a[len-2] == '-' ||
                  a[len-2] == '*' || a[len-2] == '/'))
        {
            if (num != ".")
            {
                a.remove(len-1, 1);
                ui->lineEdit->setText(a + num);
            }
        }
        ui->lineEdit->setText(a + num);
    }
}

void Calculator::EnterSign(QString a, QString sign)
{
    // Функция принимает на вход выражение и знак выражения num

    int len = a.size();

    //условие позволяет отображать лишь один знак операции и сменять ее тип

    if (a[len-1] == '-' || a[len-1] == '*' ||
            a[len-1] == '/' || a[len-1] == '+')
    {
        a.remove(len-1, 1);
    }
    ui->lineEdit->setText(a + sign);
}

void Calculator::on_Button_1_clicked()
{
    EnterNumber(ui->lineEdit->displayText(), "1");
}

void Calculator::on_Button_2_clicked()
{
    EnterNumber(ui->lineEdit->displayText(), "2");
}

void Calculator::on_Button_3_clicked()
{
    EnterNumber(ui->lineEdit->displayText(), "3");
}

void Calculator::on_Button_4_clicked()
{
    EnterNumber(ui->lineEdit->displayText(), "4");
}

void Calculator::on_Button_5_clicked()
{
    EnterNumber(ui->lineEdit->displayText(), "5");
}

void Calculator::on_Button_6_clicked()
{
    EnterNumber(ui->lineEdit->displayText(), "6");
}

void Calculator::on_Button_7_clicked()
{
    EnterNumber(ui->lineEdit->displayText(), "7");
}

void Calculator::on_Button_8_clicked()
{
    EnterNumber(ui->lineEdit->displayText(), "8");
}

void Calculator::on_Button_9_clicked()
{
    EnterNumber(ui->lineEdit->displayText(), "9");
}

void Calculator::on_Button_0_clicked()
{
    EnterNumber(ui->lineEdit->displayText(), "0");
}

void Calculator::on_Button_Point_clicked()
{
    QString a = ui->lineEdit->displayText();

    int sign = max(max(a.lastIndexOf("+"),
                    a.lastIndexOf("-")),
                    max(a.lastIndexOf("*"),
                        a.lastIndexOf("/")));   //последняя позиция знака

    int point = a.lastIndexOf(".");             //последняя позиция точки

    //условие для невозможности поставить несколько точек

    if ((point == -1 and sign == -1) or (point < sign ))
    {
        if (sign == a.length()-1)
        {
            EnterNumber(a, "0.");
        }
        else
        {
            EnterNumber(a, ".");
        }
    }
}

void Calculator::on_Button_Clear_clicked()
{
    ui->lineEdit->del();
    ui->lineEdit->setText("0");
}

void Calculator::on_Button_Delete_clicked()
{
    QString a;
    a = ui->lineEdit->displayText();
    int len = a.length();

    if (len > 1)
    {
        a.remove(len-1, 1);
        ui->lineEdit->setText(a);
    }
    else if (a == "+" or a == "-" or a == "/" or a == "*")
    {
        ui->lineEdit->setText("0");
    }
}

void Calculator::on_Button_Plus_clicked()
{
    EnterSign(ui->lineEdit->displayText(), "+");
}

void Calculator::on_Button_Minus_clicked()
{
    EnterSign(ui->lineEdit->displayText(), "-");
}

void Calculator::on_Button_Multi_clicked()
{
    EnterSign(ui->lineEdit->displayText(), "*");
}

void Calculator::on_Button_Div_clicked()
{
    EnterSign(ui->lineEdit->displayText(), "/");
}


float summa(float a, float b)         //функция суммы
{
    float summ = a + b;
    return summ;
}

float difference(float a, float b)    //функция разности
{
    float diff = a - b;
    return diff;
}

float multiplexion(float a, float b)  //функция умножения
{
    float mult = a * b;
    return mult;
}

float division(float a, float b)      //функция деления
{
    float div = a / b;
    return div;
}


// кнопка расчета результата "="

void Calculator::on_Button_Result_clicked()
{
    QString a;

    // считывается вся строка
    a = ui->lineEdit->displayText();
    int len = a.size();

    //создаются строки для отдельной записи чисел и знаков
    int i;
    QString numbers = "";
    QString signs = "";
    QVector <float> vector_numbers;

    for (i = 0; i < len;)
    {
        //условие для разделения чисел при обнаружении знака операции
        if (a[i] == '+' || a[i] == '-' || a[i] == '*' || a[i] == '/')
        {
            //знак операции записывается в сроку операций
            signs += a[i];

            //перевод числа типа string в integer и запись чисел в вектор
            if ((i == 0) && (a[i] == '-'))
            {
                numbers.append("0");
            }
            float res = numbers.toFloat();
            vector_numbers.push_back(res);
            numbers = "";
        }
        else
        {
          numbers += a[i];
        }
        i = i + 1;
    }

    int vector_length = vector_numbers.size();

    /*  условие для удаления знака операции из строки в случае, если он
    был последний в числом выражении при нажатии на равно  */

    if (numbers != "")
    {
        float res = numbers.toFloat();
        vector_numbers.push_back(res);
        vector_length = vector_numbers.size();
    }
    else
    {
        signs.remove(signs.size()-1, 1);
    }

    // Блок перемножения/деления и сложения/вычитания чисел

    int signs_length = signs.length();

    for (i = 0; i < signs_length; i += 1)
    {
        bool mult = true; bool div = true;   //наличие * и /
        int posm = 0; int posd = 0;          //позиции знаков операций в строке

        //проверка на наличие операции умножения
        if (signs.indexOf("*") == -1)
        {
            mult = false;
        }
        else
        {
            mult = true;
            posm = signs.indexOf("*");
        }

        //проверка на наличие операции деления
        if (signs.indexOf("/") == -1)
        {
            div = false;
        }
        else
        {
            div = true;
            posd = signs.indexOf("/");
        }

        // условие для выполнения умножения
        if (((mult == true) and (div == false)) or
                ((posm < posd) and (mult == true) and (div == true)))
        {
            int pos = posm;

            //результат умножения записывается в chislo
            float chislo = multiplexion(vector_numbers[pos],vector_numbers[pos+1]);

            //из вектора чисел удаляются множители
            vector_numbers.remove(pos, 2);

            //и вносится результат произведения
            vector_numbers.insert(pos, 1, chislo);

            //знак умножения удаляется из строки с операциями
            signs.replace(pos, 1, "");

        }
        // условие для выполнения деления
        else if (((div == true) and (mult == false)) or
                    ((posd < posm) and (div == true) and (mult == true)))
        {
            int pos = posd;

            //результат деления записывается в chislo
            float chislo = division(vector_numbers[pos],vector_numbers[pos+1]);

            //из вектора чисел удаляются делимое и частное
            vector_numbers.remove(pos, 2);

            //и вносится результат деления
            vector_numbers.insert(pos, 1, chislo);

            //знак деления удаляется из строки с операциями
            signs.replace(pos, 1, "");
        }
    }

    signs_length = signs.length();
    for (i = 0; i < signs_length; i += 1)
    {
        bool summ = true; bool diff = true;        //наличие + и -
        int possumm = 0; int posdiff = 0;          //позиции знаков операций в строке

        //проверка на наличие операции сложения
        if (signs.indexOf("+") == -1)
        {
            summ = false;
        }
        else
        {
            summ = true;
            possumm = signs.indexOf("+");
        }

        //проверка на наличие операции вычитания
        if (signs.indexOf("-") == -1)
        {
            diff = false;
        }
        else
        {
            diff = true;
            posdiff = signs.indexOf("-");
        }

        // условие для выполнения сложения
        if (((summ == true) and (diff == false)) or
                ((possumm < posdiff) and (summ == true) and (diff == true)))
        {
            int pos = possumm;

            //результат сложения записывается в chislo
            float chislo = summa(vector_numbers[pos],vector_numbers[pos+1]);

            //из вектора чисел удаляются слагаемые
            vector_numbers.remove(pos, 2);

            //и вносится результат суммы
            vector_numbers.insert(pos, 1, chislo);

            //знак суммы удаляется из строки с операциями
            signs.replace(pos, 1, "");
        }

        // условие для выполнения вычитания
        else if (((diff == true) and (summ == false)) or
                    ((posdiff < possumm) and (diff == true) and (summ == true)))
        {
            int pos = posdiff;

            //результат разности записывается в chislo
            float chislo = difference(vector_numbers[pos],vector_numbers[pos+1]);

            //из вектора чисел удаляются уменьшаемое и вычитаемое
            vector_numbers.remove(pos, 2);

            //и вносится результат разности
            vector_numbers.insert(pos, 1, chislo);

            //знак деления удаляется из строки с операциями
            signs.replace(pos, 1, "");
        }
    }

    QString result = QString::number(vector_numbers.value(0));
    ui->lineEdit->setText(result);
}
