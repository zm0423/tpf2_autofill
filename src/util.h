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
    std::unordered_multimap<std::string, int> station{};
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

    bool d_station_add{};
    bool d_line_add{};
    bool d_clear2_warning{true};
    bool d_tpf3_notice{};   // 三代注意事项：已勾选“下次不再提示”
    int tpf2_version{};     // 上次的二代子版本（0/1），从三代切回时恢复
    int d_version{};      // 0=字符串键(TPF2) 1=数字键(TPF2) 2=狂热运输3
    int cycle_index{};    // TPF3 周期档位 0..5 → 3600/7200/10800/21600/43200/86400

    std::string steam_user;   // TPF3 当前选择的 Steam 账号（userdata 下的账号文件夹名，写入 .dat）

    // TPF3 实时探测结果（“刷新”按钮获取，运行时数据，不写入 .dat）
    bool probe_ok{};
    std::string probe_save_id;
    int probe_cycle_sec{};

    bool tpf3() const { return d_version == 2; }
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

        MULTI_STATION,
        MULTI_LINE,

        TIME_OVER_CYCLE,
        EXPORT_MISMATCH,

    };

    explicit errortype(int type, QString q = "");
};

bool get_first_cut(std::string& linename, const char token);



void readXlsx(const std::filesystem::path& filename, std::unordered_multimap<std::string, int> &result);

void readXlsx(const std::filesystem::path& filename, std::vector<std::pair<std::string, int>> &result);





bool writeVectorToXlsx(const std::vector<std::pair<std::string, int>>& data,
                       const std::filesystem::path& filename, const std::vector<std::pair<int, int>>& sort = {});

enum class EndingType : int{
    STRANGE_STATION = 2,
    NO_MATCH = 1,
    ROAD = 3,
};

EndingType checkEnding(const std::string& input);

bool write_to_lua(const std::filesystem::path& filename,
                  const std::string& data,
                  const std::vector<std::pair<int, std::vector<std::pair<QString, QString>>>>& id,
                  const std::vector<std::pair<std::string, int>> & line,
                  int clear_if,
                  bool quotedKeys);

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

std::pair<int, int> read_xlsx_time(QVariant value, bool total_minutes = false);

bool printq(const QString& prefix, const QString& content, const QString& suffix);

enum class IDtype : int{
    STATION,
    LINE,
};

void read_id_data(const std::filesystem::path& filePath,
                  std::vector<std::pair<std::string, int>>& data,
                  IDtype type);



// ========== 狂热运输3（TPF3 桥接）相关 ==========

// export.lua 解析结果（游戏 → 工具）
struct export_info
{
    std::vector<std::pair<std::string, int>> stations;    // 站点名, id
    std::vector<std::pair<std::string, int>> lines;       // 线路名, id
    std::unordered_map<int, std::vector<int>> line_stops; // 线路id → 站序(id序列)
    int cycle_sec{};                                      // 原存档周期（秒），0=未读取
    std::string save_id;                                  // 存档名（引擎返回，可为空）
};

bool read_export_file(const std::filesystem::path& file, export_info& out);

int cycle_sec_from_index(int index);
QString cycle_label_from_sec(int sec);

// 生成 data.lua（工具 → 游戏），内部完成站序校验与周期越界校验
bool write_data_lua(const my_data& sdata,
                    const std::vector<std::pair<int, std::vector<stationinfo>>>& data);

// TPF3 实时探测（“刷新”按钮）：工具写 probe.lua，游戏插件回写 probe_result.lua
bool write_probe_file(const std::filesystem::path& dir, const std::string& token);
bool read_probe_result(const std::filesystem::path& file, std::string& token,
                       std::string& save_id, int& cycle_sec);



#endif // UTIL_H_
