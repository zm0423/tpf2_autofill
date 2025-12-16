# Timetable Data Import Guide

### Basic Steps

1.  **Select Root Directory Folder**
    *   This folder is used to store all timetable data and system data.

2.  **Select Game Save File**
    *   The default save file path is typically:
        `C:\Program Files (x86)\Steam\userdata\XXXX\1066780\local\save\xxx.lua`
        (Depends on your Steam installation location).
    >   **Tip**: You can also locate it in-game via **Settings → Advanced → Open User Data Folder**.

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
| **Timetable Stacking** | **Example**: If a train runs 3 identical schedules within 60 minutes, enabling this option will record 3 timetables; disabling it will record only 1. |

## Important Notes and Format Requirements

1.  **Timetable Format**:
    *   The **second column** of the Excel file must contain station names, and the **third and fourth columns** must contain arrival and departure times, respectively.
    *   The program only reads data from these three columns; other columns do not affect the import.

2.  **Name Matching**:
    *   Ensure that **station names** and **route names** in the timetable files **exactly match** the data from the game save (only leading/trailing spaces are allowed).

3.  **Duplicate Data Handling**:
    *   If completely duplicate timetable data is detected, or if the arrival/departure times between two sets of data differ by less than 5 seconds, the program will **automatically merge** these duplicates and prompt you before import.

4.  **Backup and Safety**:
    *   Each import automatically creates a backup of the previous data, stored within the save folder.
    *   **How to Restore**: Locate the backup file, rename it (remove the backup marker), then copy it back to the save folder to overwrite the current file. (Ensure "File name extensions" are visible in your system's folder options before proceeding).

5.  **Critical Prerequisite**:
    *   **Ensure the game is closed or the save file is not loaded when importing timetables.**

---

If you encounter any issues, please contact me via the **"About" section** within the software.  
Bilibili Private Message, GitHub Issues, or email are all acceptable.

Thank you for using!