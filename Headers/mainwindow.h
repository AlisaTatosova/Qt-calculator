#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QString>
#include <QLabel>
#include <QPushButton>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
   ~MainWindow();

signals:
    void numberEmitted(int number);

private slots:
    void digit_button_pushed();
    void operation_button_pushed();
    void set_buttons();
    void print_binary(QString val);
    void equal();
    void resize_digits(const QString& str);
    QString first32bits(quint64 value);
    QString second32bits(quint64 value);
    quint64 string_to_int64(const QString& str);
    qint64 convert_QString_toQInt64(const QString& str);

private:
    QPushButton* op_buttons[14];
    QPushButton* digit_buttons[10];

private:
    QLabel* label;
    QLabel* bin_first_32;
    QLabel* bin_second_32;
    QString val = "";
    QString res = "";
    QVector<QString> vec;
};
#endif // MAINWINDOW_H
