function CopyFile(old_path, new_path)
    local old_file = io.open(old_path, "rb")
    local new_file = io.open(new_path, "wb")
    local old_file_sz, new_file_sz = 0, 0
    if not old_file or not new_file then
        return false
    end
    while true do
        local block = old_file:read(2^13)
        if not block then 
        old_file_sz = old_file:seek( "end" )
        break
        end
        new_file:write(block)
    end
    old_file:close()
    new_file_sz = new_file:seek( "end" )
    new_file:close()
    return new_file_sz == old_file_sz
end