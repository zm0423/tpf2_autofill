#ifndef UTIL_H_
#define UTIL_H_


#include <filesystem>
#include <vector>
#include <string>
#include <unordered_map>
#include <QString>
#include <QVariant>
#include <memory>

#include "xlsxdocument.h"

inline QString stq(const std::string &s){return QString::fromStdString(s);}




struct my_data
{
    std::unordered_map<std::string, int> station{};
    std::vector<std::pair<std::string, int>> line{};

    std::string folder_name{};
    std::string sg_name{};

    std::filesystem::path sys_save_dir{};

    std::filesystem::path folder_dir{};
    std::filesystem::path sg_dir{};

    std::string buffer{};

    char trunc{'/'};
    bool trunc_if{};

    bool easy_if{true};
    bool xls_if{true};
    bool invalid_if{};
    int clear_if{1};
    bool pile_if{};
};

const std::filesystem::path sys_file_name{"tpf2_autofill.dat"};

void refresh_file(const my_data &sdata);


inline QString get_linename(const std::vector<std::pair<std::string, int>>& line, int lineid)
{
    auto it = find_if(line.begin(),line.end(), [&](const auto&p){return p.second == lineid;});
    return it == line.end() ? "" : stq(it->first);
}

struct arrdeptime
{
    int arrmin{};
    int arrsec{};
    int depmin{};
    int depsec{};

    bool operator()(){
        return !(arrmin == 0 && depmin == 0 && arrsec <= 5 && depsec <= 5);
    }
};

arrdeptime operator-(arrdeptime a, arrdeptime b);




struct stationinfo
{
    int stationid;
    std::vector<arrdeptime> arrdep;
};


struct fileinfo
{
    std::unique_ptr<QXlsx::Document> doc{};
    std::unordered_map<QString, QString> sheets{};
};







void display_info(const QString& head, const QString& info);


class errortype
{
public:
    enum
    {
        LINE_EMPTY,
        STATION_EMPTY,

        LISTMODE_NOLIST,

        LIST_LINE_DONT_EXIST,

        LIST_FILE_DONT_EXIST,

        LIST_SHEET_DONT_EXIST,

        NO_TIMETABLE,


        STATION_DONT_EXIST,
        NO_LINE_STAT,
        STATION_MISMATCH,

        TIME_INVALID,

        NO_SHEET_IN_FILE,

        SAVE_FILE_UNOPEN,
        SAVE_FILE_UNSAVE,

        NO_TIMETABLE_MOD,

    };

    explicit errortype(int type, QString q = "");
};

bool get_first_cut(std::string& linename, const char token);



void readXlsx(const std::filesystem::path& filename, std::unordered_map<std::string, int> &result);

void readXlsx(const std::filesystem::path& filename, std::vector<std::pair<std::string, int>> &result);





bool writeVectorToXlsx(const std::vector<std::pair<std::string, int>>& data,
                       const std::filesystem::path& filename, const std::vector<std::pair<int, int>>& sort = {});

enum class EndingType : int{
    STRANGE_STATION = 2,
    NO_MATCH = 1,
    ROAD = 3,
};

EndingType checkEnding(const std::string& input);

bool text_to_vector(const std::string& input, std::vector<std::pair<std::string, int>> &output);




bool write_to_lua(const std::filesystem::path& filename,
                  const std::string& data,
                  const std::vector<std::pair<int, std::vector<std::pair<QString, QString>>>>& id,
                  const std::vector<std::pair<std::string, int>> & line,
                  int clear_if);

struct CSVData {
    std::string col2;
    std::string col3;
    std::string col4;
};


struct TimeComponents {
    int hours = 0;
    int minutes = 0;
    int seconds = 0;
    bool operator()(){return hours != -1 && minutes != -1 && seconds != -1;}
};

TimeComponents parseCSVTime(const std::string& timeStr);

std::vector<CSVData> readCSV(const std::filesystem::path& filePath);

std::pair<int, int> read_xlsx_time(QVariant value);

bool printq(const QString& q,const QString& p);




#endif // UTIL_H_
