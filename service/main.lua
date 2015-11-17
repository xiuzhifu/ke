local path = ...
local ke = require "ke"
local main = {}
function main:init()
	local db = ke.newservice("simpledb")
	print(77886,ke.call(db, 'lua', 'get', "anmeng"))
end
return main
