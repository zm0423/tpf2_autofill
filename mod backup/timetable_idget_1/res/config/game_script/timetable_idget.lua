local MyMod = {
    data = {
        -- 格式: [实体ID] = 名称
        stations = {},  -- STATION_GROUP: 实体ID -> 车站名称
        lines = {}      -- LINE: 实体ID -> 线路名称
    }
}

-- 核心收集函数
function MyMod:collectData()
    -- 收集车站
    api.engine.forEachEntityWithComponent(
        function(entityId)
            local nameComp = api.engine.getComponent(entityId, api.type.ComponentType.NAME)
            if nameComp and nameComp.name then
                MyMod.data.stations[entityId] = nameComp.name
            end
        end,
        api.type.ComponentType.STATION_GROUP
    )
        
    -- 收集线路
    api.engine.forEachEntityWithComponent(
        function(entityId)
            local nameComp = api.engine.getComponent(entityId, api.type.ComponentType.NAME)
            if nameComp and nameComp.name then
                MyMod.data.lines[entityId] = nameComp.name
            end
        end,
        api.type.ComponentType.LINE
    )
end



function data()   
    return {        
        -- 保存函数
        save = function()
            MyMod:collectData()
            return MyMod.data
        end
    }
end




