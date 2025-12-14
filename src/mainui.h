#ifndef MAINUI_H
#define MAINUI_H

#include <QWidget>
#include "util.h"
#include <QButtonGroup>

QT_BEGIN_NAMESPACE
namespace Ui { class mainui; }
QT_END_NAMESPACE

class mainui : public QWidget
{
    Q_OBJECT

public:
    explicit mainui(QWidget *parent = nullptr);



    ~mainui();

private slots:
    void on_change_dir_clicked(){get_folder();}

    void on_change_sg_clicked(){get_sg();}

    void on_input_data_clicked();

    void on_sync_all_data_clicked();


    void on_settinginfo_clicked();

    void on_settinginfo_2_clicked();

private:

    void init();

    bool get_folder();
    bool get_sg();
    void read_station_line();

    void refresh();



    bool get_list(std::vector<std::pair<int, std::vector<std::pair<QString, QString>>>>& list);

    bool get_data(std::vector<std::pair<int, std::vector<std::pair<QString, QString>>>>& list,
                    std::vector<std::pair<int, std::vector<stationinfo>>> &output);


    std::string generateTimeSlot(const arrdeptime& time, int indent);

    std::string generateStationLua(const stationinfo& station, int indent = 6);

    std::string get_filetext(int lineId, const std::vector<stationinfo>& stations);

    


    my_data sdata;



    QButtonGroup* m_xlsGroup;
    QButtonGroup* m_easyGroup;
    QButtonGroup* m_clearGroup;

    Ui::mainui *ui;
};
#endif // MAINUI_H
