**Instructions**

The game's underlying code only recognizes numeric IDs, so you must obtain them from within the game.

1. **Download and activate assist mod**
   - **TPF2**: Download the "**Timetable AutoFill Program Assist Tool**" mod in the workshop. Activate it in your save game and save it.
   - **TPF3**: Open the in-game **Mod Hub** and subscribe to and enable the "**Timetable AutoFill**" mod on mod.io; entering a save once is enough.

2. **Import Data into the Software:**
   - Select either **Station Import** or **Route Import**.
   - Select either **Overwrite** or **Add only**. **Overwrite** is recommended becasue it is better in auto sorting.
   - *For routes: truncation is supported. See the truncation option for details.*

## Transport Fever 3 (TPF3) Special Notes

*   **No need to save the game**: TPF3 has no save file concept; after entering a save, the program detects and reads the data in real time.
*   **Data source**: TPF3 data comes directly from the game (it is read automatically once when entering a save, but if anything has changed you still need to click synchronize in the external program); if stations or routes are added mid-game, first click the in-game **“Export Save Station/Line Data”** button, then re-import in the program.
*   **Working file names**: TPF3 generates `tpf3_timetable_station.xlsx` and `tpf3_timetable_line.xlsx` (list mode: `tpf3_timetable_list.xlsx`).

**Important:**
- All data used for timetable synchronization must **match** the IDs you import here (Spaces before or behind is allowed).
- If anything doesn’t match, you must adjust your data accordingly—whether in the timetable files or in the `_station`/`_line` files—so the software can correctly identify the corresponding IDs.
- **Make sure the data you actually use is unique**: No matter line or station, if you use this name in your timetable data, make sure it is unique.(If you do not use that station or line, duplicate is allowed)