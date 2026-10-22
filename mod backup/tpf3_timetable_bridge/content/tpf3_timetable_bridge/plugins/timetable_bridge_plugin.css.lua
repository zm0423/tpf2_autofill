-- TPF3 Timetable Bridge — stylesheet.
-- Centers the AutoFill window, gives it a fixed size, and draws a framed
-- log area. Loaded automatically by the engine (same convention as the
-- other *.css.lua stylesheets shipped with mods).
local ssu = require "::/gui/main/stylesheetutil.lua"

function data()
    local result = {}
    local a = ssu.makeAdder(result)

    -- Centered, fixed-size main window.
    a("Window#tpf3.autofill.window", {
        anchorPoint = { 0.5, 0.5 },
        gravity = { 0.5, 0.5 },
        size = { 780, 540 },
    })

    -- Inner padding for the content container.
    a("R::AutoFillWindowContent", {
        padding = { 12, 12, 12, 12 },
    })

    -- Framed log area.
    a("!afb-logbox", {
        backgroundColor = { 0, 0, 0, 0.35 },
        borderWidth = { 1, 1, 1, 1 },
        borderColor = { 1, 1, 1, 0.15 },
        padding = { 8, 8, 8, 8 },
    })

    -- Log scroll area: fills the leftover height (a bounded parent so the
    -- vertical scrollbar engages) — long reports scroll instead of clipping.
    a("!afb-logscroll", {
        gravity = { -1, -1 },
        minSize = { 0, 0 },
    })

    -- Game-bar “AutoFill” button: renders as a flat text item like the
    -- neighbouring Earnings / clock displays (same pattern as celmi's
    -- gamebar clock), so it fits the bar in every language.
    a("R::AutoFillBridge Button!afb-gamebar-btn", {
        padding = { 2, 6, 2, 6 },
        margin = { 0, 4, 0, 4 },
        backgroundColor1 = { 0, 0, 0, 0 },
        borderColor = { 0, 0, 0, 0 },
    })

    return result
end
