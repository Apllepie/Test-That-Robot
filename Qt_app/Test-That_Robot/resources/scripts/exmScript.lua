-- Простая реализация алгоритма A*

-- Функция для вычисления эвристики (Манхэттенское расстояние)
local function heuristic(a, b)
    return math.abs(a.x - b.x) + math.abs(a.y - b.y)
end

-- Основная функция поиска пути
function findPath(grid, width, height, startNode, goalNode)
    local openSet = {}
    local closedSet = {}
    local cameFrom = {}

    local gScore = {}
    local fScore = {}

    local startKey = startNode.x .. "," .. startNode.y
    gScore[startKey] = 0
    fScore[startKey] = heuristic(startNode, goalNode)

    table.insert(openSet, startNode)

    while #openSet > 0 do
        local current = nil
        local lowestFScore = math.huge
        local currentIndex = -1

        for i, node in ipairs(openSet) do
            local key = node.x .. "," .. node.y
            if fScore[key] < lowestFScore then
                lowestFScore = fScore[key]
                current = node
                currentIndex = i
            end
        end

        if current.x == goalNode.x and current.y == goalNode.y then
            -- Восстановление пути
            local path = {}
            local temp = current
            while temp do
                table.insert(path, 1, temp)
                local key = temp.x .. "," .. temp.y
                temp = cameFrom[key]
            end
            return path
        end

        table.remove(openSet, currentIndex)
        local currentKey = current.x .. "," .. current.y
        closedSet[currentKey] = true

        local neighbors = {
            {x = current.x, y = current.y - 1},
            {x = current.x, y = current.y + 1},
            {x = current.x - 1, y = current.y},
            {x = current.x + 1, y = current.y}
        }

        for _, neighbor in ipairs(neighbors) do
            if neighbor.x >= 0 and neighbor.x < width and neighbor.y >= 0 and neighbor.y < height and not grid[neighbor.y + 1][neighbor.x + 1] then
                local neighborKey = neighbor.x .. "," .. neighbor.y
                if not closedSet[neighborKey] then
                    local tentativeGScore = gScore[currentKey] + 1

                    if not gScore[neighborKey] or tentativeGScore < gScore[neighborKey] then
                        cameFrom[neighborKey] = current
                        gScore[neighborKey] = tentativeGScore
                        fScore[neighborKey] = gScore[neighborKey] + heuristic(neighbor, goalNode)

                        local inOpenSet = false
                        for _, node in ipairs(openSet) do
                            if node.x == neighbor.x and node.y == neighbor.y then
                                inOpenSet = true
                                break
                            end
                        end

                        if not inOpenSet then
                            table.insert(openSet, neighbor)
                        end
                    end
                end
            end
        end
    end

    return nil -- Путь не найден
end