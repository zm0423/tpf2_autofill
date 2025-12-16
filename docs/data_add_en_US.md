**Instructions**

The game's underlying code only recognizes numeric IDs, so you must obtain them from within the game.

1. **Enable Debug Mode:**
   - Open the game, go to **Settings → Advanced**, enable **Debug Mode**, and restart the game.

2. **Open Console in Your Save:**
   - Load your save, press **`** (backtick) to open the console (key can be changed in settings).

3. **Run the Command:**
   - Copy the provided command.
   - Paste it into the console and press **Enter**. A large amount of data will appear.

4. **Copy the Output:**
   - Select **all data from the console**, starting from just after your command up to the last line of output (or the next command prompt).
   - Format should generally be: **name line → blank line → number line → blank line**, repeating.
   - *Garbled names are normal if your language is not ascii.*

5. **Import Data into the Software:**
   - Paste the copied text into the left panel of this software.
   - Select either **Station Import** or **Route Import**.
   - *For routes: truncation is supported. See the truncation option for details.*

**Important:**
- All data used for timetable synchronization must **match** the IDs you import here (Spaces before or behind is allowed).
- If anything doesn’t match, you must adjust your data accordingly—whether in the timetable files or in the `_station`/`_line` files—so the software can correctly identify the corresponding IDs.