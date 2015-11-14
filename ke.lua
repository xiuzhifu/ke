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

metaservice.__index = metaservice

function yield_call(handle, session)
	local msg, sz
	copool[session] = coroutine.create(function()
	msg, sz = coroutine.yield('CALL', session)
	end)
	coroutine.resume(copool[session])
	return msg, sz
end
function ke.call2(name, protoname, ... )
	local p = proto[protoname]
	local handle = name
	if type(handle) == "string" then handle = ke.name[handle] end
	local inst = ke.instance[handle]
	assert(inst)
	local session = c.send(inst.handle, p.pack(...))
	if session == nil then 
		error("call to invalid address " .. name)
	end
	return p.unpack(yield_call(handle, session))
end

function ke.call(name, protoname, ... )
	local handle = name
	local inst = ke.instance[handle]
	assert(inst)
	local session = c.send(inst.handle, ...)
	if session == nil then 
		error("call to invalid address " .. name)
	end
	return yield_call(handle, session)
end

function ke.send(name, ... )
	
end

function ke.registerservice(service, name)
	assert(service)
	assert(name)
	assert(ke.service[name] == nil)
	service.__index = service
	setmetatable(service, metaservice)	
	ke.service[name] = service
end

function ke.newservice(name, ...)
	if ke.service[name] then
		local inst = setmetatable({}, ke.service[name])
		inst:init(...)
		inst.handle = c.new()
		print(inst.handle)
		ke.instance[inst.handle] = inst
		return inst.handle
	end
end

function ke.dispatch(...)
--inst.receive
--session, msg, sz
	print(...)
	co = copool[session]
	print("dispatch", session)
	if co then coroutine.resume(co, msg, sz) end
end



function ke.start()
	print('c.callback(ke.dispatch)')
	c.callback(ke.dispatch)
end
ke.start()
return ke