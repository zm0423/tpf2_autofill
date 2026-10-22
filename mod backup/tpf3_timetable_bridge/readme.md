# TPF3 Timetable Bridge

外部 **TPF2 Autofill** 工具和 **Timetables (celmi_timetables)** mod 之间的桥接（自用）。

> 详细开发笔记：`F:\code\qt\tpf3_autofill_notes.md`；本文件只做速查。

## 组成

- **game script**：订阅 `autofill_data` 事件（id `AutoFillBridge`），把工具
  生成的时刻表数据通过 Timetables 的公开编辑通道（`TimetablesEdit`）写入；
- **UI 插件**：负责文件 IO（导出 / 推送 / 探测），并提供一个底栏
  "AutoFill" 按钮 + 状态窗口。

## 数据交换目录

`<Steam>/userdata/<id>/3493540/local/towns_industries/`

> 游戏 40420（2026-10）起，脚本文件 API 只允许 `towns_industries`、
> `biomes`、`heightmaps`、`mod_presets` 四个目录，因此桥接文件放在
> `towns_industries` 里，并统一加前缀以免和地图编辑器数据混淆。

| 文件 | 方向 | 内容 |
|---|---|---|
| `tpf3_timetable_bridge_export.lua` | 游戏 → 工具 | 站点 / 线路 / 站序、存档名、周期 |
| `tpf3_timetable_bridge_data.lua` | 工具 → 游戏 | 时刻表数据（在 AutoFill 窗口点「应用数据文件」后生效） |
| `tpf3_timetable_bridge_probe.lua` | 工具 → 游戏 | 工具「刷新」的探测请求（token） |
| `tpf3_timetable_bridge_probe_result.lua` | 游戏 → 工具 | 当前存档名 + 当前周期（回应探测） |

## 日志

游戏日志：`<Steam>/userdata/<id>/3493540/local/crash_dump/stdout.txt`，
搜索 `[AFB]`（脚本）/ `[AFB-UI]`（插件）。

## 说明

- 进入存档时会自动导出一次 `tpf3_timetable_bridge_export.lua`（桌面工具的数据来源）；时刻表
  数据**不会自动应用**，需在 AutoFill 窗口手动点「将导入数据应用至时刻表」。
- 界面文案中英文自动切换：`strings.json`（英文为源语言，中文见 `zh_CN`，
  跟随游戏语言设置；其他语言回退英文）。
- 仅供自用开发，依赖 Timetables mod 的编辑事件（`TimetablesEdit` + set*
  词汇表，rev.8 验证）。
