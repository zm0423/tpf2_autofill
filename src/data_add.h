#ifndef DATA_ADD_H
#define DATA_ADD_H

#include <QWidget>
#include "util.h"

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
    void on_station_copy_clicked();

    void on_line_copy_clicked();

    void on_station_input_clicked();

    void on_line_input_clicked();

    void on_instruction_clicked();

    void on_instruction_2_clicked();

private:




    const std::string stationcpy = "api.engine.forEachEntityWithComponent(function(entity) print(api.engine.getComponent(entity, api.type.ComponentType.NAME).name)print(entity) end,api.type.ComponentType.STATION_GROUP)";
    const std::string linecpy = "api.engine.forEachEntityWithComponent(function(entity) print(api.engine.getComponent(entity, api.type.ComponentType.NAME).name)print(entity) end,api.type.ComponentType.LINE)";

    Ui::data_add *ui;
    my_data &sdata;
};

#endif // DATA_ADD_H
