//////////////////////////////////////////////////////////////////////
// OpenTibia - an opensource roleplaying game
//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//////////////////////////////////////////////////////////////////////


#ifndef __OTSERV_SPELLS_H__
#define __OTSERV_SPELLS_H__

#include "game.h"
#include "luascript.h"
#include "player.h"
#include "actions.h"
#include "talkaction.h"
#include "baseevents.h"

class InstantSpell;
class ConjureSpell;
class RuneSpell;
class Spell;

typedef std::map<uint32_t, RuneSpell*> RunesMap;
typedef std::map<std::string, InstantSpell*> InstantsMap;

class Spells : public BaseEvents
{
public:
	Spells();
	virtual ~Spells();
	
	Spell* getSpellByName(const std::string& name);
	RuneSpell* getRuneSpell(uint32_t id);
	RuneSpell* getRuneSpellByName(const std::string& name);
	
	InstantSpell* getInstantSpell(const std::string words);
	InstantSpell* getInstantSpellByName(const std::string& name);

	TalkActionResult_t playerSaySpell(Player* player, SpeakClasses type, const std::string& words);

	static int32_t spellExhaustionTime;
	static int32_t spellInFightTime;

	static Position getCasterPosition(Creature* creature, Direction dir);
	
protected:
	virtual void clear();
	virtual LuaScriptInterface& getScriptInterface();
	virtual std::string getScriptBaseName();
	virtual Event* getEvent(const std::string& nodeName);
	virtual bool registerEvent(Event* event, xmlNodePtr p);	
	
	RunesMap runes;
	InstantsMap instants;

	LuaScriptInterface m_scriptInterface;
};

typedef bool (InstantSpellFunction)(const InstantSpell* spell, Creature* creature, const std::string& param);
typedef bool (ConjureSpellFunction)(const ConjureSpell* spell, Creature* creature, const std::string& param);
typedef bool (RuneSpellFunction)(const RuneSpell* spell, Creature* creature, Item* item, const Position& posFrom, const Position& posTo);

class BaseSpell{
public:
	BaseSpell() {};
	virtual ~BaseSpell(){};

	virtual bool castSpell(Creature* creature) = 0;
	virtual bool castSpell(Creature* creature, Creature* target) = 0;
};

class CombatSpell : public BaseSpell{
public:
	CombatSpell(Combat* _combat, bool _needTarget, bool _needDirection);
	virtual ~CombatSpell(){};

	virtual bool castSpell(Creature* creature);
	virtual bool castSpell(Creature* creature, Creature* target);

private:
	bool needDirection;
	bool needTarget;
	Combat* combat;
};

class Spell : public BaseSpell{
public:
	Spell();
	virtual ~Spell(){};
	
	bool configureSpell(xmlNodePtr xmlspell);
	
	const std::string& getName() const {return name;}

	//virtual bool castSpell(Creature* creature) = 0;
	//virtual bool castSpell(Creature* creature, Creature* target) = 0;

	void postCastSpell(Player* player) const;
	void postCastSpell(Player* player, uint32_t manaCost, uint32_t soulCost) const;

	int32_t getManaCost(const Player* player) const;
	int32_t getSoulCost(const Player* player) const;
	
	virtual bool isInstant() const = 0;

	static ReturnValue CreateIllusion(Creature* creature, const Outfit_t outfit, int32_t time);
	static ReturnValue CreateIllusion(Creature* creature, const std::string& name, int32_t time);
	static ReturnValue CreateIllusion(Creature* creature, uint32_t itemId, int32_t time);

protected:
	bool playerSpellCheck(const Player* player) const;
	bool playerInstantSpellCheck(const Player* player, const Position& toPos);
	bool playerRuneSpellCheck(const Player* player, const Position& toPos);
	
	bool enabled;
	bool premium;
	int32_t level;
	int32_t magLevel;
	
	int32_t mana;
	int32_t manaPercent;
	int32_t soul;
	bool exhaustion;
	bool needTarget;
	bool selfTarget;
	bool blockingSolid;
	bool blockingCreature;
	bool isAggressive;
	
	typedef std::map<int32_t, bool> VocSpellMap;
	VocSpellMap vocSpellMap;

private:
	std::string name;
};

class InstantSpell : public TalkAction, public Spell
{
public:
	InstantSpell(LuaScriptInterface* _interface);
	virtual ~InstantSpell();
	
	virtual bool configureEvent(xmlNodePtr p);
	virtual bool loadFunction(const std::string& functionName);
	
	virtual bool playerCastInstant(Player* player, const std::string& param);

	virtual bool castSpell(Creature* creature);
	virtual bool castSpell(Creature* creature, Creature* target);

	//scripting
	bool executeCastSpell(Creature* creature, const LuaVariant& var);
	InstantSpellFunction* function;
	
	virtual bool isInstant() const { return true;}
	bool getHasParam() const { return hasParam;}

protected:	
	virtual std::string getScriptEventName();
	
	static InstantSpellFunction HouseGuestList;
	static InstantSpellFunction HouseSubOwnerList;
	static InstantSpellFunction HouseDoorList;
	static InstantSpellFunction HouseKick;
	static InstantSpellFunction SearchPlayer;
	static InstantSpellFunction SummonMonster;
	static InstantSpellFunction Levitate;
	static InstantSpellFunction Illusion;
	
	static House* getHouseFromPos(Creature* creature);
	
	bool internalCastSpell(Creature* creature, const LuaVariant& var);
	
	bool needDirection;
	bool hasParam;
};

class ConjureSpell : public InstantSpell
{
public:
	ConjureSpell(LuaScriptInterface* _interface);
	virtual ~ConjureSpell();
	
	virtual bool configureEvent(xmlNodePtr p);
	virtual bool loadFunction(const std::string& functionName);
	
	virtual bool playerCastInstant(Player* player, const std::string& param);

	virtual bool castSpell(Creature* creature) {return false;}
	virtual bool castSpell(Creature* creature, Creature* target) {return false;}

	uint32_t getConjureId() const {return conjureId;}
	uint32_t getConjureCount() const {return conjureCount;}
	uint32_t getReagentId() const {return conjureReagentId;}
	
protected:	
	virtual std::string getScriptEventName();

	static bool internalConjureItem(Player* player, uint32_t conjureId, uint32_t conjureCount);
	static bool internalConjureItem(Player* player, uint32_t conjureId, uint32_t conjureCount, uint32_t reagentId, slots_t slot);

	static ConjureSpellFunction ConjureItem;
	static ConjureSpellFunction ConjureFood;
	
	bool internalCastSpell(Creature* creature, const LuaVariant& var);
	Position getCasterPosition(Creature* creature);

	ConjureSpellFunction* function;

	uint32_t conjureId;
	uint32_t conjureCount;
	uint32_t conjureReagentId;
};

class RuneSpell : public Action, public Spell
{
public:
	RuneSpell(LuaScriptInterface* _interface);
	virtual ~RuneSpell();
	
	virtual bool configureEvent(xmlNodePtr p);
	virtual bool loadFunction(const std::string& functionName);
	
	virtual ReturnValue canExecuteAction(const Player* player, const Position& toPos);

	virtual bool executeUse(Player* player, Item* item, const PositionEx& posFrom, const PositionEx& posTo, bool extendedUse);
	
	virtual bool castSpell(Creature* creature);
	virtual bool castSpell(Creature* creature, Creature* target);

	//scripting
	bool executeCastSpell(Creature* creature, const LuaVariant& var);
	
	virtual bool isInstant() const { return false;}
	uint32_t getRuneItemId(){return runeId;};
	
protected:
	virtual std::string getScriptEventName();
	
	static RuneSpellFunction Illusion;
	static RuneSpellFunction Convince;

	bool internalCastSpell(Creature* creature, const LuaVariant& var);

	bool hasCharges;
	uint32_t runeId;
	
	RuneSpellFunction* function;
};

#endif
