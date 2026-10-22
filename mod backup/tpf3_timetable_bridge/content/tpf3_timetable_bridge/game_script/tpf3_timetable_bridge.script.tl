-- TPF3 Timetable Bridge — game script.
--
-- All file IO happens in the UI plugin part of this mod. The plugin
-- broadcasts the timetable data as the "autofill_data" event
-- (id="AutoFillBridge"); this script subscribes to it and applies the
-- data through celmi_timetables' public editor channel (broadcast events
-- with id="TimetablesEdit").
--
-- After each apply it stores a small report in its state
-- (`state.lastApply`), which the UI plugin reads for the import report.
--
-- This script also logs the line roster to the game log once per session
-- (prefix "[AFB]").

local record BridgeSlot
    arrMin : integer
    arrSec : integer
    depMin : integer
    depSec : integer
end

local record BridgeStop
    index : integer
    slots : {BridgeSlot}
end

local record BridgeLine
    name : string
    stops : {BridgeStop}
end

local record BridgeData
    revision : integer | nil
    force : boolean | nil
    cycleSec : integer | nil
    clearAll : boolean | nil
    clearLines : {string} | nil
    lines : {BridgeLine} | nil
end

local record ReportLine
    name : string
    stops : integer
    slots : integer
    found : boolean
end

local record Report
    revision : integer | nil
    force : boolean | nil
    applied : integer
    totalLines : integer
    totalStops : integer
    totalSlots : integer
    cleared : integer
    lines : {ReportLine}
end

local record BridgeState
    rosterLogged : boolean | nil
    appliedRevision : integer | nil
    lastApply : Report | nil
end

local subscribedThisSession : boolean = false

local function nameOf(e : Engine.Entity) : string
    if e == nil then return "" end
    if (e as integer) <= 0 then return "" end
    local n = api.engine.getComponent(e, api.type.ComponentType.NAME) as Engine.Component.Name
    if n ~= nil and n.name ~= nil then return n.name end
    return ""
end

local function logRoster()
    local lines = api.engine.system.lineSystem.getLines()
    debugPrint("[AFB] roster: " .. tostring(#lines) .. " line(s)")
    for _, lineEnt in ipairs(lines) do
        local lineComp = api.engine.getComponent(lineEnt, api.type.ComponentType.LINE) as Engine.Component.Line
        local s : string = "[AFB]   line id=" .. tostring(lineEnt) ..
            " name=\"" .. nameOf(lineEnt) .. "\""
        if lineComp ~= nil and lineComp.stops ~= nil then
            for i, stop in ipairs(lineComp.stops) do
                s = s .. " | stop " .. tostring(i) .. "=\"" .. nameOf(stop.stationGroup) .. "\""
            end
        end
        debugPrint(s)
    end
end

local function sendEdit(name : string, payload : any)
    api.cmd.sendCommand(
        api.cmd.makeScriptingSendEventCmd("", "TimetablesEdit", name, payload)
    )
end

-- 方案甲：清空 = 该线路时刻表开关关闭 + 每站条件设为「无时刻表(None)」
local function clearLine(lineEnt : Engine.Entity)
    sendEdit("setHasTimetable", { line = lineEnt, enabled = false })
    local lineComp = api.engine.getComponent(lineEnt, api.type.ComponentType.LINE) as Engine.Component.Line
    if lineComp ~= nil and lineComp.stops ~= nil then
        for i, _stop in ipairs(lineComp.stops) do
            sendEdit("setConditionType", { line = lineEnt, stopIdx = i, type = "None" })
        end
    end
end

local function applyData(data : BridgeData) : Report
    local report : Report = {
        revision = data.revision,
        force = data.force or false,
        applied = 0,
        totalLines = 0,
        totalStops = 0,
        totalSlots = 0,
        cleared = 0,
        lines = {},
    }

    if data.cycleSec ~= nil then
        sendEdit("setCycleSec", { value = data.cycleSec })
    end

    local lines = api.engine.system.lineSystem.getLines()
    local byName : {string : Engine.Entity} = {}
    for _, lineEnt in ipairs(lines) do
        local nm = nameOf(lineEnt)
        if nm ~= "" then byName[nm] = lineEnt end
    end

    -- 清空阶段（先清后写；方案甲）
    local doClearAll : boolean = (data.clearAll == true)
    if doClearAll or data.clearLines ~= nil then
        local toClear : {string : boolean} = {}
        if doClearAll then
            for nm, _ in pairs(byName) do
                toClear[nm] = true
            end
        else
            local list : {string} = data.clearLines as {string}
            for _, nm in ipairs(list) do
                toClear[nm] = true
            end
        end
        local cleared : integer = 0
        for nm, _ in pairs(toClear) do
            local target : Engine.Entity | nil = byName[nm]
            if target ~= nil then
                clearLine(target)
                cleared = cleared + 1
            end
        end
        report.cleared = cleared
        debugPrint("[AFB] cleared " .. tostring(cleared) .. " line(s)")
    end

    local lineSpecs : {BridgeLine} | nil = data.lines
    if lineSpecs ~= nil then
        local arr : {BridgeLine} = lineSpecs as {BridgeLine}
        report.totalLines = #arr
        for _, lineSpec in ipairs(arr) do
            local rl : ReportLine = {
                name = lineSpec.name,
                stops = 0,
                slots = 0,
                found = false,
            }
            local target : Engine.Entity | nil = byName[lineSpec.name]
            if target == nil then
                debugPrint("[AFB] line not found: \"" .. tostring(lineSpec.name) .. "\"")
            else
                local stopCount : integer = #lineSpec.stops
                local slotCount : integer = 0
                for _, stop in ipairs(lineSpec.stops) do
                    slotCount = slotCount + #stop.slots
                end
                debugPrint("[AFB] applying line \"" .. lineSpec.name ..
                    "\" (" .. tostring(stopCount) .. " stop(s), " ..
                    tostring(slotCount) .. " slot(s))")
                sendEdit("setHasTimetable", { line = target, enabled = true })
                for _, stop in ipairs(lineSpec.stops) do
                    sendEdit("setArrDep", { line = target, stopIdx = stop.index, slots = stop.slots })
                end
                rl.found = true
                rl.stops = stopCount
                rl.slots = slotCount
                report.applied = report.applied + 1
                report.totalStops = report.totalStops + stopCount
                report.totalSlots = report.totalSlots + slotCount
            end
            table.insert(report.lines, rl)
        end
    end

    debugPrint("[AFB] applied " .. tostring(report.applied) .. " line(s)")
    return report
end

local ret : any = {
    update = function(_userParams : any, state : GameScriptState<BridgeState>, _dt : number) : any
        if not subscribedThisSession then
            state:subscribeToEvent("autofill_data")
            subscribedThisSession = true
            debugPrint("[AFB] subscribed to autofill_data")
            -- New session: hide the previous session's import report from
            -- the AutoFill window (the report must only show this session's
            -- imports; re-entering a saved game counts as a new session).
            local raw0 : BridgeState | nil = state:get()
            local s0 : BridgeState
            if raw0 == nil then
                s0 = { rosterLogged = false }
            else
                s0 = raw0
            end
            if s0.lastApply ~= nil then
                s0.lastApply = nil
                state:set(s0)
                debugPrint("[AFB] cleared previous session's apply report")
            end
        end

        local raw : BridgeState | nil = state:get()
        local s : BridgeState
        if raw == nil then
            s = { rosterLogged = false }
        else
            s = raw
        end
        if not (s.rosterLogged or false) then
            local lines = api.engine.system.lineSystem.getLines()
            if #lines > 0 then
                logRoster()
                s.rosterLogged = true
                state:set(s)
            end
        end
    end,

    handleEvent = function(_userParams : any, state : GameScriptState<BridgeState>, _src : string, id : string, name : string, param : any) : any
        if id ~= "AutoFillBridge" or name ~= "autofill_data" or param == nil then return end

        local data = param as BridgeData
        local raw : BridgeState | nil = state:get()
        local s : BridgeState
        if raw == nil then
            s = { rosterLogged = false }
        else
            s = raw
        end

        if not (data.force or false) and data.revision ~= nil and s.appliedRevision == data.revision then
            debugPrint("[AFB] revision " .. tostring(data.revision) .. " already applied; skip")
            return
        end

        local report = applyData(data)

        s.lastApply = report
        if data.revision ~= nil then
            s.appliedRevision = data.revision
        end
        state:set(s)
    end,
}

return ret
