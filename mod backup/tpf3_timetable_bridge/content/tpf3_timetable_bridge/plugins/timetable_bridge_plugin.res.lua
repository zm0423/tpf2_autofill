-- TPF3 Timetable Bridge — UI plugin descriptor.
-- The plugin does the file IO (the game-script context has no `app` API)
-- and broadcasts the data to the bridge game script.
function data()
    return {
        type = "react-plugin ::GameBarInfoDisplayExtension",
        data = {
            filePath = "tpf3_timetable_bridge::/tpf3_timetable_bridge/plugins/timetable_bridge_plugin.script@AutoFillBridge",
            order = 99,
            priority = 99,
        }
    }
end
