local c = require "kissevent.c"
local ke = { 
	service = {},
	instance = {},
	name = {},
 }

local copool = {}
local proto = {}
local metaservice = {}
function metaservice:name(name)
	if not ke.name[name] then
		ke.name[name] = self.handle
	end
end

function yield_call(handle, session)
	local msg, sz
	copool[session] = coroutine.create(function()
	msg, sz = coroutine.yield('CALL', session)
	end)
	coroutine.resume(co)
	return msg, sz
end
function ke.call(name, protoname, ... )
	local p = proto[protoname]
	local handle = name
	if type(handle) = "string" then handle = ke.name[handle] end
	local inst = ke.instance[handle]
	assert(inst)
	local session = c.send(inst.c, p.pack(...))
	if session = nil then 
		error("call to invalid address " .. name)
	end
	return p.unpack(yield_call(handle, session))
end

function ke.send(name, ... )
	
end

function ke.registerservice(service, name)
	assert(service)
	assert(name)
	assert(ke.service[name] == nil)
	setmetatable(service, metaservice)
	ke.service[name] = service
end

function ke.newservice(name, ...)
	if ke.service[name] then
		local inst = setmetatable({}, ke.service[name])
		inst:init(...)
		inst.handle = c.new()
		c.callback(inst.handle, inst.receive)
		ke.instence[inst.handle] = inst
	end
end

function ke.dispatch(session, msg, sz)
	co = copool[session]
	if co then coroutine.resume(co, msg, sz) end
end



function ke.start()
return ke