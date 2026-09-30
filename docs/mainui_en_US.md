# Timetable Data Import Guide

### Basic Steps

1.  **Select Root Directory Folder**
    *   This folder is used to store all timetable data and system data.

2.  **Select Game Save File (no save file in TPF3 — see details below)**
    *   The default save file path is typically:
        `C:\Program Files (x86)\Steam\userdata\XXXX\1066780\local\save\xxx.lua`
        (Depends on your Steam installation location).
    >   **Tip**: You can also locate it in-game via **Settings → Advanced → Open User Data Folder**.
    >   **TPF3 Tip**: TPF3 mode does not require selecting a save file; the data folder is detected automatically. See “Transport Fever 3 (TPF3) Mode” below.

3.  **Enter the Game and Obtain Save Information**
    *   Follow the instructions in the **"Station and Route Data Entry"** section to obtain the station and route information for your save.
    *   After successful import, two files will be generated: `[SaveName]_station.xlsx` and `[SaveName]_line.xlsx`.
    *   Please verify and modify the data in these files as needed. Note that changes to the `_line` file affect the **Overwrite Options**.

### Data Import Mode Description

#### Simple Mode
*   **Logic**: The program will automatically search for corresponding timetable files based on the route data in the `_line.xlsx` file.
*   **File Naming Convention**: `[RouteName].xlsx`, or `[RouteName]_[Index].xlsx` (e.g., `Z1.xlsx`, `Z1_2.xlsx`).
    *   Different timetables for the same route should be named consecutively using this rule.
    *   **Note**: Do not add `_1` to the first file; numbering starts from `_2`.
*   **Sheet Handling**: Automatically uses **all sheets** within the file.

#### List Match Mode
*   The program will automatically generate a `[SaveName]_list.xlsx` file.
    *   **First Row**: Can be changed freely (usually for a title or description).
    *   **Second Row Onwards**: Each row represents an import configuration for one route.
    *   **First Column**: Route name.
    *   **Second Column Onwards**: Every two columns form a pair, representing **Filename** and **Sheet Name**.
*   **Multiple Sheets**: To specify multiple sheets, separate their names with a **space**.
*   **All Sheets**: To use all sheets within a file, leave the sheet name cell **empty**.
*   You can add configuration pairs indefinitely to the right, or add a new row for the same route.

### Overwrite Options Description

| Option | Description |
| :--- | :--- |
| **Overwrite Only** | Only overwrites existing timetables that are detected (e.g., if the data contains a timetable for only one train, only that train's timetable will be overwritten). |
| **Clear _line and Import** | Detects all routes listed in `_line.xlsx`, first deletes their existing timetables (if any), then imports new data. **Use case**: When a save contains timetables for non-train routes, you can configure `_line.xlsx` to include only train routes, thereby clearing and overwriting only train timetables. |
| **Clear All and Import** | Deletes **all** existing timetables in the save, then imports new data. |

### Other Options Description

| Option | Description |
| :--- | :--- |
| **xlsx, csv** | Select the format of the import files. **Note**: `csv` only supports UTF-8 encoding (choose this option in the 'Save As' dialog), and sheet names are ignored in List Match Mode. |
| **Ignore Last Data Row** | When checked, the program will ignore the last row of the data table. |
| **Timetable Stacking** | **Example**: If a train runs 3 identical round trips within 60 minutes, **enabling** this option stacks the 3 trips into 3 separate timetables; **disabling** it records all 3 trips as one complete line. |
| **Compatibility Version** | Select the data format matching your installed timetable mod: **"Timetable & Train Diagram, Timetable 1.2"** stores line IDs as string keys, while **"Timetable 1.3-1.5"** uses numeric keys. During import, all line entries in the save will be migrated to the selected key format automatically. **Make sure the choice matches your mod, otherwise the game will fail to load the timetables.** (In TPF3 mode this section becomes the “Cycle” hours selector — see below) |

### Transport Fever 3 (TPF3) Mode

Click the **“Switch to TPF3”** button in the top-right corner to switch (the choice is remembered). TPF3 works quite differently from TPF2, so please read the following carefully:

1.  **Install the assist mod**: Subscribe to and enable the **“Timetable AutoFill”** mod from mod.io in the in-game **Mod Hub**.
2.  **Real-time sync**: We recommend starting the game and loading a save first, then opening this program; the import is **real-time**, so there is no need to re-enter the game after importing like in TPF2.
3.  **No save file**: TPF3 has no save file (the save row in the app shows **“Save in current game (no save file in TPF3 — see below)”**); the program detects the current in-game save in real time. If you switch saves, click **“Refresh”** after the save has finished loading (Refresh also fetches the current save name and cycle). The TPF3 working files are named `tpf3_timetable_xxx` (station / line / list); **keep different saves in different folders**.
4.  **Full workflow**: Load the game save → open the program → import routes and stations (same as TPF2, no need to do this every time) → import the data → click **“Apply Imported Data to Timetables”** in the in-game mod window to finish.
5.  **New stations or routes added mid-game**: First click the in-game **“Export Save Station/Line Data”** button, then re-import the station and route data in the program.
6.  **No backup**: TPF3 has no save file, so there is no backup; please make sure the data is correct before saving your game!
7.  **Cycle (hours)**: TPF3 has a global cycle (hours) concept (1/2/3/6/12/24 hours), selected in the “Compatibility Version” section; the hours part of “H:MM:SS” counts into total minutes (offset within the cycle); an hour overflow will raise an error.
8.  **Multiple accounts**: If you play Transport Fever with multiple Steam accounts and use timetables, please select the account manually (in the **“Current Steam user”** row of the “Path” section); if there is only one account, or only one account plays TPF3 timetables, the account is locked automatically and no manual selection is needed.
9.  **Problems**: If you run into any problems, please contact the author immediately.

## Important Notes and Format Requirements

1.  **Timetable Format**:
    *   The **second column** of the Excel file must contain station names, and the **third and fourth columns** must contain arrival and departure times, respectively.
    *   The program only reads data from these three columns; other columns do not affect the import.

2.  **Name Matching**:
    *   Ensure that **station names** and **route names** in the timetable files **exactly match** the data from the game save (only leading/trailing spaces are allowed).
    *   **Make sure the data you actually use is unique**: No matter line or station, if you use this name in your timetable data, make sure it is unique.(If you do not use that station or line, duplicate is allowed)

3.  **Duplicate Data Handling**:
    *   If completely duplicate timetable data is detected, or if the arrival/departure times between two sets of data differ by less than 5 seconds, the program will **automatically merge** these duplicates and prompt you before import.

4.  **Backup and Safety (TPF2)**:
    *   Each import automatically creates a backup of the previous data, stored within the save folder.
    *   **How to Restore**: Locate the backup file, rename it (remove the backup marker), then copy it back to the save folder to overwrite the current file. (Ensure "File name extensions" are visible in your system's folder options before proceeding).
    *   **TPF3 note**: TPF3 has no save file, so there is no backup; please make sure the data is correct before saving.

5.  **Critical Prerequisite**:
    *   **TPF2**: Ensure the game is closed or the save file is not loaded when importing timetables.
    *   **TPF3**: Keep the game running with a save loaded (see “Transport Fever 3 (TPF3) Mode” above).

---

If you encounter any issues, please contact me via the **"About" section** within the software.  
Bilibili Message, GitHub Issues, or email are all acceptable.

Thank you for using!