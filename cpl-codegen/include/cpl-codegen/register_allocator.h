#pragma once

#include <variant>
#include <map>
#include <functional>
#include <initializer_list>
#include <cpl-basics/string.hpp>
#include <cpl-basics/array.hpp>
#include <cpl-basics/file.hpp>
#include <cpl-ir/type.hpp>
#include <cpl-ir/value.hpp>

namespace Codegen
{

struct Reg
{
	String name;
	Uint32 size = 0;
};

struct RegSlot : public Reg
{
	String groupName;
	
	RegSlot(Reg reg, const String& group)
		: Reg(reg), groupName(group) {}
	RegSlot() = default;
};

struct StackSlot
{
	Uint32 offset;
	Uint32 size;
};

struct Slot
{
	std::variant<RegSlot, StackSlot> storage;
	const IRGenerate::Value* value;
};

// Store Destinations
struct AnyDestination {};

struct RegDestination
{
	RegSlot reg;
};

struct StackDestination
{
	std::optional<StackSlot> stackSlot;
};

using StoreDestination = std::variant<AnyDestination, RegDestination, StackDestination>;

class BasicSlotAllocator;

class AllocatorHandler
{
public:
	virtual ~AllocatorHandler() = default;
public:
	virtual void OnNewStackSlot(const StackSlot& slot) = 0;
	virtual void OnSpill(const RegSlot& from, const StackSlot& to) = 0;
	virtual void OnImmediateStore(const std::variant<RegSlot, StackSlot>& slot, const IRGenerate::ImmediateValue* value) = 0;
	virtual void OnGlobalLoad(const std::variant<RegSlot, StackSlot>& slot, const IRGenerate::GlobalValue* value) = 0;
	virtual void OnMove(const std::variant<RegSlot, StackSlot>& from, const std::variant<RegSlot, StackSlot>& to) = 0;
	virtual void OnFreeReg(const RegSlot& reg) = 0;
};

struct RegGroup
{
	String name;
	Array<Reg> regs;
};

class RegisterDistributor
{
public:
	RegisterDistributor(std::initializer_list<RegGroup> availableRegisters);
public:
	void ResetState();
public:
	const RegGroup& GetGroupByName(const String& groupName);
	void FreeReg(const RegSlot& reg);
	// automatically 'takes' the register if any was found
	std::optional<RegSlot> GetFreeRegForType(const IRGenerate::Type* typ);
	std::optional<RegSlot> GetSpecificFreeRegForType(const String& groupName, const IRGenerate::Type* typ);
	bool RegSupportingType(const Reg& reg, const IRGenerate::Type* typ);
	std::optional<Reg> RegSupportingType(const RegGroup& group, const IRGenerate::Type* typ);
private:	
	void TakeFreeRegGroup(const RegGroup& group);
private:
	// Array<Reg> m_regs;
	std::map<String, RegGroup> m_regGroups;
	Array<String> m_freeRegs;
};

class BasicSlotAllocator
{
public:
	BasicSlotAllocator(RegisterDistributor* regDistro, AllocatorHandler* handler);
	virtual ~BasicSlotAllocator() = default;

public:
	void ResetState();
	void SetExplicitOffset(Uint32 offset);
	inline Uint32 GetStackOffset() const noexcept { return m_nextOffset; }

public:
	const RegSlot LoadValueInReg(const IRGenerate::Value* value, std::optional<String> regGroup = std::nullopt);
	const Slot& StoreOrUpdateValue(const IRGenerate::Value* value, StoreDestination dest = AnyDestination{});
	void FreeReg(const RegSlot& reg);

	StackSlot GetNewStackSlotFromSize(Uint32 size);

	void SpillRegGroup(const String& groupName);

private:
	StackSlot GetNewStackSlotFromValue(const IRGenerate::Value* value);
	RegSlot GetAnyReg(const IRGenerate::Value* targetValue);
	RegSlot GetSpecificRegGroup(const String& groupName, const IRGenerate::Value* targetValue);

private:
	String GetSlotKey(const IRGenerate::Value* value);
	bool ValueIsStored(const IRGenerate::Value* value);
	Slot& GetSlotByKey(String slotKey);
	void InsertValueSlot(const IRGenerate::Value* value, std::variant<RegSlot, StackSlot> slotStorage, bool rewrite = false);
	void DeleteValueSlot(const String& slotKey);

private:
	RegisterDistributor* m_regDistro = nullptr;
	AllocatorHandler* m_handler = nullptr;

	std::map<String, Slot> m_slots;

	Uint32 m_nextOffset = 0;
};

} // namespace Codegen

