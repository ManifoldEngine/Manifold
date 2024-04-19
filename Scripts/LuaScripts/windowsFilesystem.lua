
local m = {}

m.copyFile = function (oldPath, newPath)
    local oldFile = io.open(oldPath, "rb")
    local newFile = io.open(newPath, "wb")
    local oldFileSize, newFileSize = 0, 0
    if not oldFile or not newFile then
        return false
    end
    while true do
        local block = oldFile:read(2^13)
        if not block then 
            oldFileSize = oldFile:seek( "end" )
            break
        end
        newFile:write(block)
    end
    oldFile:close()
    newFileSize = newFile:seek( "end" )
    newFile:close()
    return newFileSize == oldFileSize
end

m.replaceTokenInFile = function (path, token, replacement)
    -- Open the file for reading
    local file = io.open(path, "r")
    if not file then
        return nil, "Unable to open file for reading: " .. path
    end
    
    -- Read the entire content of the file
    local content = file:read("*a")
    file:close()
    
    -- Replace the token with the replacement string
    content = content:gsub(token, replacement)
    
    -- Open the file for writing
    file = io.open(path, "w")
    if not file then
        return nil, "Unable to open file for writing: " .. path
    end
    
    -- Write the modified content back to the file
    file:write(content)
    file:close()
    
    return true
end

-- Function to recursively copy a directory and its subdirectories
m.copyDirectory = function (source, destination)
    -- Create destination directory if it doesn't exist
    os.execute("mkdir \"" .. destination .. "\"")

    -- Iterate over files and subdirectories in the source directory
    for file in io.popen('dir "' .. source .. '" /b /a-d'):lines() do
        -- Construct full paths
        local sourcePath = source .. "\\" .. file
        local destinationPath = destination .. "\\" .. file

        -- Copy file
        os.execute("copy \"" .. sourcePath .. "\" \"" .. destinationPath .. "\"")
    end

    -- Recursively copy subdirectories
    for directory in io.popen('dir "' .. source .. '" /b /ad'):lines() do
        if directory ~= "." and directory ~= ".." then
            copyDirectory(source .. "\\" .. directory, destination .. "\\" .. directory)
        end
    end
end

m.copyDirectoryHierarchy = function(source, destination, foreachFile)
    -- Ensure source and destination paths end with a directory separator
    source = source:gsub("[/\\]$", "") -- Remove trailing slashes
    destination = destination:gsub("[/\\]$", "") -- Remove trailing slashes

    -- Get the list of directories and files in the source directory
    local directories = {}
    local files = {}
    for item in m.ls(source) do
        local itemPath = source .. "/" .. item
        if m.isDirectory(itemPath) then
            table.insert(directories, item)
        else
            table.insert(files, item)
        end
    end

    -- Create the destination directory if it doesn't exist
    m.createDirectory(destination)

    -- Copy files from source to destination
    for _, file in ipairs(files) do
        local sourceFilePath = source .. "/" .. file
        local destinationFilePath = destination .. "/" .. file
        m.copyFile(sourceFilePath, destinationFilePath)
        foreachFile(destinationFilePath)
    end

    -- Recursively copy subdirectories
    for _, directory in ipairs(directories) do
        local sourceDirPath = source .. "/" .. directory
        local destinationDirPath = destination .. "/" .. directory
        m.copyDirectoryHierarchy(sourceDirPath, destinationDirPath)
    end
end

m.isDirectory = function(path)
    -- Attempt to open the 
    local file = io.open(path, "r")
    if file then
        -- If successful, assume it's not a directory
        file:close()
        return false
    else
        -- Otherwise, assume it's a directory
        return true
    end
end

m.getCurrentDirectoryName = function ()
    local handle = io.popen("echo %CD%")
    local currentDirectory = handle:read("*a")
    handle:close()

    -- Replace backslashes with forward slashes to ensure consistency
    currentDirectory = currentDirectory:gsub("\\", "/")

    -- Replace end of line characters
    currentDirectory = currentDirectory:gsub("\n", "")

    -- Extract the folder name from the path
    local folderName = currentDirectory:match("([^/]+)[/\\]?$") -- Get the last component of the path

    return folderName
end

m.createDirectory = function(path)
    local success, error_message = os.execute("mkdir \"" .. path .. "\"")
    if success then
        return true
    else
        return false, error_message
    end
end


m.ls = function (path) 
    return io.popen("dir " .. path .. " /b"):lines()
end

return m