local ke = require "ke"
local simpledb = require "simpledb"
local db = ke.newservice("SIMPLEDB")
ke.call(db, 'lua', "anmeng")
