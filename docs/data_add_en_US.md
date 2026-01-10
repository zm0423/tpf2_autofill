**Instructions**

The game's underlying code only recognizes numeric IDs, so you must obtain them from within the game.

1. **Download and activate assist mod**
   - Download "**Timetable autofill program assist tool**" mod in the workshop.
   - Activate it in your save game and save it.

2. **Import Data into the Software:**
   - Select either **Station Import** or **Route Import**.
   - Select either **Overwrite** or **Add only**. **Overwrite** is recommended becasue it is better in auto sorting.
   - *For routes: truncation is supported. See the truncation option for details.*

**Important:**
- All data used for timetable synchronization must **match** the IDs you import here (Spaces before or behind is allowed).
- If anything doesn’t match, you must adjust your data accordingly—whether in the timetable files or in the `_station`/`_line` files—so the software can correctly identify the corresponding IDs.
- **Make sure the data you actually use is unique**: No matter line or station, if you use this name in your timetable data, make sure it is unique.(If you do not use that station or line, duplicate is allowed)