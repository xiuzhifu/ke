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
local sleep_co = {}

function yield_call(handle, session)
	copool[session] = coroutine.running()
	print("yield_call", session, copool[session])
	local msg, sz = coroutine.yield('CALL', session)
	return msg, sz
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

function ke.newservice(name, ...)
	local chunk = assert(loadfile('./service/'..name..'.lua'))
	local ret, metatable = pcall(chunk)
	if not ret then
		print(metatable)
		return 
	end
	metatable.__index = metatable
	local inst = setmetatable({}, metatable)
	inst.handle = c.new()
	if inst.init then 
		ke.start(-inst.handle, inst.init, inst)
	end
	ke.instance[inst.handle] = inst
	return inst.handle
end
 
function ke.dispatch(source, session, msg, sz)
	local inst = ke.instance[source]
	ke.start(inst.handle, function() 
		local co = copool[session]
		copool[session] = nil
		local msg1, sz1 = inst:receive(source, session, msg, sz)
		print("dis", co, source, session,msg1, sz1)
		if co then coroutine.resume(co, msg1, sz1) end
	end)
end

function ke.start(session, mainco, ...)
	local p = ...
	copool[session] = coroutine.create(function()
		mainco(p)
	end)
	print('ke.start',session, copool[session])
	coroutine.resume(copool[session])
end
c.callback(ke.dispatch)
return ke