-- TPF3 Timetable Bridge — game-script descriptor (same form as the base
-- game's own *.gs.lua descriptors). The referenced module lives next to
-- this file.
function data()
    return {
        updateScript = {
            fileName = "tpf3_timetable_bridge.script@update",
        },
        handleEventScript = {
            fileName = "tpf3_timetable_bridge.script@handleEvent",
        },
    }
end
