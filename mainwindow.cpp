#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtCore/QCoreApplication>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent) {
       QFont font;
       font.setPointSize(40);  // setting the desired font size

       label = new QLabel("0", this);
       label -> setFont(font);
       label -> setGeometry(QRect(QPoint(400, 75), QSize(450, 190)));

       bin_first_32 = new QLabel("0000 0000 0000 0000 0000 0000 0000 0000 0000", this);
       bin_second_32 = new QLabel("0000 0000 0000 0000 0000 0000 0000 0000 0000", this);

       font.setPointSize(11);
       bin_first_32 -> setGeometry(QRect(QPoint(25, 125), QSize(450, 190)));
       bin_first_32 -> setFont(font);

       bin_second_32 -> setGeometry(QRect(QPoint(25, 150), QSize(450, 190)));
       bin_second_32 -> setFont(font);

       font.setPointSize(25);

       for (int i = 0; i < 14; ++i) {
           QString operations[] = {"C", "=", "+", "-", "*", "/", "AND", "OR", "NOR", "XOR", "<<", ">>", "RoL", "RoR"};
           op_buttons[i] = new QPushButton(operations[i], this);
           if (i == 1) {
               connect(op_buttons[i], SIGNAL(released()), this, SLOT(equal()));
           } else {
                connect(op_buttons[i], SIGNAL(released()), this, SLOT(operation_button_pushed()));
           }
       }

       for (int i = 0; i < 10; ++i) {
           QString digit = QString::number(i);
           digit_buttons[i] = new QPushButton(digit, this);
           connect(digit_buttons[i], SIGNAL(released()), this, SLOT(digit_button_pushed()));
       }

       set_buttons();
}

// function to get first 32 buts of binary representation of number
QString MainWindow::first32bits(quint64 value) {
    QString bin;
    for (int i = 63; i >= 32; --i) {
        if ((i + 1) % 4 == 0) {
            bin.append(" ");
        }
        bin.append((value & (1ULL << i)) ? '1' : '0');
    }
    return bin;
}

// function to get second 32 buts of binary representation of number
QString MainWindow::second32bits(quint64 value) {
    QString bin;
    for (int i = 31; i >= 0; --i) {
        if ((i + 1) % 4 == 0) {
            bin.append(" ");
        }
        bin.append((value & (1ULL << i)) ? '1' : '0');
    }

    return bin;
}

// handling operation button pressing
void MainWindow::operation_button_pushed() {
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    vec.append(val);
    vec.append(button -> text());

    if (button -> text() == "C") {
        val = "0";
        vec.clear();
        label -> setText(val);
        print_binary(val);
    } else if (button -> text() == "=") {
        equal();
    }

    val = "";
    label -> setText(val);
}

// handling result after pushing equal button
void MainWindow::equal() {
    vec.append(val);

    // * and / has higer priorities in multiple operations than + and -
    for (int i = 0; i < vec.size(); ++i) {
        if (vec[i] == "*" || vec[i] == "/") {
            if (i - 1 >= 0 && i + 1 < vec.size()) {
                double operand1 = vec[i - 1].toDouble();
                double operand2 = vec[i + 1].toDouble();
                if (vec[i] == "*") {
                    vec[i - 1] = QString::number(operand1 * operand2);
                } else if (vec[i] == "/") {
                    if (operand2 != 0) {
                        vec[i - 1] = QString::number(operand1 / operand2);
                    }
                }
                // removing the operator and the second operand from the vector
                vec.remove(i, 2);
                i--;
            }
        }
    }

    for (int i = 0; i < vec.size(); ++i) {
        if (vec[i] == "+" || vec[i] == "-") {
            if (i - 1 >= 0 && i + 1 < vec.size()) {
                double operand1 = vec[i - 1].toDouble();
                double operand2 = vec[i + 1].toDouble();
                // performing the operation and replacing the result in the vector
                if (vec[i] == "+") {
                    vec[i - 1] = QString::number(operand1 + operand2);
                } else if (vec[i] == "-") {
                    vec[i - 1] = QString::number(operand1 - operand2);
                }
                // removeing the operator and the second operand from the vector
                vec.remove(i, 2);
                i--; // adjust the index after removing elements
            }
        }
    }

    for (int i = 0; i < vec.size(); ++i) {
        if (vec[i] == "AND" || vec[i] == "NOR") {
            int operand1 = vec[i - 1].toInt();
            int operand2 = vec[i + 1].toInt();
            if (vec[i] == "AND") {
                vec[i - 1] = QString::number(operand1 & operand2);  // bitwise AND
            } else if (vec[i] == "NOR") {
                vec[i - 1] = QString::number(~operand1);  // bitwise NOR
            }
            vec.remove(i, 2);
            i--;
        }
    }

    for (int i = 0; i < vec.size(); ++i) {
        if (vec[i] == "OR" || vec[i] == "XOR") {
            int operand1 = vec[i - 1].toInt();
            int operand2 = vec[i + 1].toInt();
            if (vec[i] == "XOR") {
                vec[i - 1] = QString::number(operand1 ^ operand2);  // bitwise XOR
            } if (vec[i] == "OR") {
                vec[i - 1] = QString::number(operand1 | operand2);  // OR
            }
            vec.remove(i, 2);
            i--;
        }
    }

    for (int i = 0; i < vec.size(); ++i) {
        if (vec[i] == "<<" || vec[i] == ">>") {
           int operand1 = vec[i - 1].toInt();
           int operand2 = vec[i + 1].toInt();
           if (vec[i] == "<<") {
               vec[i - 1] = QString::number(operand1 << operand2);
           } if (vec[i] == ">>") {
               vec[i - 1] = QString::number(operand1 >> operand2);
           }
           vec.remove(i, 2);
           i--;
       }
    }

    for (int i = 0; i < vec.size(); ++i) {
        if (vec[i] == "RoL" || vec[i] == "RoR") {
            unsigned int operand1 = vec[i - 1].toInt();
            if (vec[i] == "RoL") {
                vec[i - 1] = QString::number((operand1 << 1) | (operand1 >> (sizeof(operand1) * 8 - 1)));
            } else if (vec[i] == "RoR") {
                vec[i - 1] = QString::number((operand1 >> 1) | (operand1 >> (sizeof(operand1) * 8 - 1)));
            }
            vec.remove(i, 2);
            i--;
        }
    }

    res = QString::number(vec[0].toDouble());
    // handling the binary represent of negative number
    if (res.toDouble() < 0) {
        qint64 tmp = convert_QString_toQInt64(res);
        quint64 abs_value = ~tmp;
        quint64 invert = ~abs_value;
        label -> setText(QString::number(tmp));
        resize_digits(QString::number(tmp));
        print_binary(QString::number(invert));
    } else {
        label -> setText(res);
        resize_digits(res);
        print_binary(res);
    }
    vec.clear();
    val = res;
}

// handling digit button pressing
void MainWindow::digit_button_pushed() {
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    emit numberEmitted((button -> text()[0].digitValue())); // The signal numberEmitted is emitted using the emit keyword. It sends the signal with the digit value obtained from the first character of the button's text
    val += QString::number((button -> text()[0].digitValue()));
    resize_digits(val);
}

// resizing digits when the number is so long that there is no place in screen
void MainWindow::resize_digits(const QString& str) {
    if (str.length() >= 17) {
        return;
    }

    for (int i = 0; i < 12; ++i) {
        if (str.length() == i) {
            QFont font1;
            font1.setPointSize(40);
            label -> setFont(font1);
            label -> setGeometry(QRect(QPoint(450 - (i * 39), 75), QSize(450, 190)));
            break;
        }
    }

    for (int i = 11; i < 17; ++i) {
        if (str.length() > i) {
            QFont font;
            font.setPointSize(50 - i - 1 * (i - 10));
            label -> setFont(font);
        }
    }

    label -> setText(str);
    print_binary(str);
}

// printing binary represenation
void MainWindow::print_binary(QString val) {
    QFont font;
    font.setPointSize(12);
    bin_first_32 -> setText(first32bits(string_to_int64(val)));
    bin_second_32 -> setText(second32bits(string_to_int64(val)));
    bin_first_32 -> setFont(font);
    bin_second_32 -> setFont(font);
}

// setting buttons
void MainWindow::set_buttons() {
    QFont font;
    font.setPointSize(20);

    op_buttons[0] -> setGeometry(QRect(QPoint(10, 270), QSize(80, 80)));
    op_buttons[0] -> setFont(font);

    op_buttons[1] -> setGeometry(QRect(QPoint(280, 630), QSize(80, 80)));
    op_buttons[1] -> setFont(font);

    for (int i = 2; i < 6; ++i) {
        op_buttons[i] -> setGeometry(QRect(QPoint(370, 270 + ((i - 1) * 90)), QSize(80, 80)));
        op_buttons[i] -> setFont(font);
    }

    for (int i = 6; i < 10; ++i) {
        font.setPointSize(20);
        op_buttons[i] -> setGeometry(QRect(QPoint(10, 360 + ((i - 6) * 90)), QSize(80, 80)));
        op_buttons[i] -> setFont(font);
    }


    for (int i = 10; i < 14; ++i) {
        op_buttons[i] -> setGeometry(QRect(QPoint(10 + (i - 9) * 90, 270), QSize(80, 80)));
        op_buttons[i] -> setFont(font);
    }

    digit_buttons[0] -> setFont(font);
    digit_buttons[0] -> setGeometry(QRect(QPoint(100, 630), QSize(170, 80)));

    for (int i = 1; i < 4; ++i) {
        digit_buttons[i] -> setFont(font);
        digit_buttons[i] -> setGeometry(QRect(QPoint(10 + 90 * i, 540), QSize(80, 80)));
    }

    for (int i = 4; i < 7; ++i) {
        digit_buttons[i] -> setFont(font);
        digit_buttons[i] -> setGeometry(QRect(QPoint(10 + 90 * (i - 3), 450), QSize(80, 80)));
    }

    for (int i = 7; i < 10; ++i) {
        digit_buttons[i] -> setFont(font);
        digit_buttons[i] -> setGeometry(QRect(QPoint(10 + 90 * (i - 6), 360), QSize(80, 80)));
    }
}

qint64 MainWindow::convert_QString_toQInt64(const QString& str) {
    qint64 result = str.toLongLong();
    return result;
}

quint64 MainWindow::string_to_int64(const QString& dec) {
    return dec.toULongLong();
}

MainWindow::~MainWindow() {
    delete label;
    delete bin_first_32;
    delete bin_second_32;
}




