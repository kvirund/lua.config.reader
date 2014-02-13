-- Helpers package
utils = {}
function utils.print_pair(key, value, indent)
	if "string" == type(value) then
		print(string.rep("\t", indent) .. tostring(key) .. " (" .. type(key) .. "): '" .. tostring(value) .. "' (" .. type(value) .. ")")
	else
		print(string.rep("\t", indent) .. tostring(key) .. " (" .. type(key) .. "): " .. tostring(value) .. " (" .. type(value) .. ")")
	end
end

-- Configuration readers
readers = {}
function readers.simple(filename)
	-- Code taken from http://rosettacode.org/wiki/Read_a_configuration_file#Lua
	local conf = {}
	local io = require("io")
	local fp = io.open(filename, "r" )

	for line in fp:lines() do
		line = line:match( "%s*(.+)" )
		if line and line:sub( 1, 1 ) ~= "#" and line:sub( 1, 1 ) ~= ";" then
			option = line:match( "%S+" ):lower()
			value  = line:match( "%S*%s*(.*)" )

			if not value then
				conf[option] = true
			else
				if not value:find( "," ) then
					conf[option] = value
				else
					value = value .. ","
					conf[option] = {}
					for entry in value:gmatch( "%s*(.-)," ) do
						conf[option][#conf[option]+1] = entry
					end
				end
			end
		end
	end

	fp:close()
	
	return conf
end

function readers.json(filename)
	local io = require("io")
	local fp = io.open(filename, "r" )
	local json = require("json")

	return json.decode(fp:read("*all"))
end

function print_config(conf, indent)
	if not (type(indent) == "number") then indent = 0 end
	for key, value in pairs(conf) do
		p = key
		if not ("string" == type(key) or "number" == type(key) or "boolean" == type(key)) then
			p = tostring(key)
			print("Non string configuration parameter '" .. p .. "' found")
		end

		if "table" == type(value) then
			print(string.rep("\t", indent) .. key .. ":")
			print_config(value, indent + 1)
		elseif "string" == type(value) or "boolean" == type(value) or "number" == type(value) then
			utils.print_pair(key, value, indent)
		end
	end
end

configs = {}
for reader in pairs(readers) do
	configs[reader] = readers[reader](reader .. ".conf")
end
print_config(configs)
