local simpledb = {}
local ke = require "ke"
function simpledb:set(k, v)
	print("simpledb:set")
	self.db[k] = v
end

function simpledb:get(k)
	local ret = ke.call(self.simpledb1, 'lua', 'get2', "anmeng")
	print("simpledb:get", ret)
	return ret, 12
end

function simpledb:init()
	self.simpledb1 = ke.newservice("simpledb1")
	self.db = { }
end

function simpledb:hotfix( ... )
	-- body
end
function simpledb:receive(source, session, cmd, ...)
	if simpledb[cmd] then
		return simpledb[cmd](self)
	end
end

return simpledb

