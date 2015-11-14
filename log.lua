local ke = require "ke"
local logger = {}
function logger:print(k, v)
	self.db[k] = v
end

function logger:init()
	self.db = { }
end

function logger:hotfix( ... )
	-- body
end
function logger.receive(session, source, cmd, ...)
	if logger[cmd] then
		return logger[cmd]()
	end
end
ke.registerservice(logger, "LOG")
return logger