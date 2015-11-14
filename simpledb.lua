local ke = require "ke"
local simpledb = {}
function simpledb:set(k, v)
	print("simpledb:set")
	self.db[k] = v
end

function simpledb:get(k)
	print("simpledb:get")
	local r = ke.call("abc", 'lua', "1123")
	return self.db[k]
end

function simpledb:init()
	self.db = { }
end

function simpledb:hotfix( ... )
	-- body
end
function simpledb.receive(session, source, cmd, ...)
	print("sim.rece")
	if simpledb[cmd] then
		return simpledb[cmd]()
	end
end
ke.registerservice(simpledb, "SIMPLEDB")
return simpledb
