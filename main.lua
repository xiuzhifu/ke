local ke1 = require "ke"
local ke2 = require "ke"
local simpledb = require "simpledb"
local db = ke1.newservice("SIMPLEDB")
ke1.call(db, 'lua', "anmeng")
