local simpledb = {}
function simpledb:set(k, v)
	print("simpledb:set")
	self.db[k] = v
end

function simpledb:get2(k)
	print("simpledb1:get")
	--return self.db[k]
	return "simpledb1", 11
end

function simpledb:init()
	self.db = { }
end

function simpledb:hotfix( ... )
	-- body
end
function simpledb:receive(source, session, cmd, ...)
	if simpledb[cmd] then
		return simpledb[cmd]()
	end
end

return simpledb

