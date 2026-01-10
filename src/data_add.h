#ifndef DATA_ADD_H
#define DATA_ADD_H

#include <QWidget>
#include "util.h"
#include <QButtonGroup>

namespace Ui {
class data_add;
}

class data_add : public QWidget
{
    Q_OBJECT

public:
    explicit data_add(my_data &input_data, QWidget *parent = nullptr);
    ~data_add();

private slots:

    void on_station_input_clicked();

    void on_line_input_clicked();

    void on_instruction_clicked();

    void on_instruction_2_clicked();

private:

    enum {
      ADD = 1,
      COVER = 0,
    };

    QButtonGroup* station_group;
    QButtonGroup* line_group;

    Ui::data_add *ui;
    my_data &sdata;
};

#endif // DATA_ADD_H
