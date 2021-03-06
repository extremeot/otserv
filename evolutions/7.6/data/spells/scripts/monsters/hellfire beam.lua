local combat = createCombatObject()
setCombatParam(combat, COMBAT_PARAM_TYPE, COMBAT_FIREDAMAGE)
setCombatFormula(combat, COMBAT_FORMULA_LEVELMAGIC, -1.3, -1200, -1.7, -1500)

local arr = {
{0, 1, 0},
{0, 1, 0},
{0, 1, 0},
{0, 1, 0},
{0, 1, 0},
{0, 1, 0},
{0, 3, 0},
}

local arrDiag = {
{1, 0, 0, 0, 0, 0, 0},
{0, 1, 0, 0, 0, 0, 0},
{0, 0, 1, 0, 0, 0, 0},
{0, 0, 0, 1, 0, 0, 0},
{0, 0, 0, 0, 1, 0, 0},
{0, 0, 0, 0, 0, 1, 0},
{0, 0, 0, 0, 0, 0, 3},
}

local area = createCombatArea(arr, arrDiag)

setCombatArea(combat, area)

function onCastSpell(cid, var)
	return doCombat(cid, combat, var)
end
