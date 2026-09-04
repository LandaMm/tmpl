#pragma once

#include <variant>
#include <list>
#include <map>
#include <functional>
#include <initializer_list>
#include <cpl-basics/string.hpp>
#include <cpl-basics/array.hpp>
#include <cpl-basics/file.hpp>
#include <cpl-basics/linkedlist.hpp>
#include <cpl-basics/memsize.hpp>
#include <cpl-ir/type.hpp>
#include <cpl-ir/value.hpp>

namespace Codegen
{

struct Reg
{
	String name;
	MemSize size;
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
	MemSize offset;
	MemSize size;
	bool taken;
	
	StackSlot(MemSize offset_, MemSize size_, bool taken_)
		: offset(offset_), size(size_), taken(taken_) { }
};

using SlotStorage = std::variant<RegSlot, StackSlot>;

struct Slot
{
	SlotStorage storage;
	const IRGenerate::Value* value{ nullptr };
};

// Store Destinations
struct AnyDestination {};

struct RegTarget
{
	String group;
	MemSize minSize;

	RegTarget(const String& groupName, MemSize minRegSize = {})
		: group(groupName), minSize(minRegSize) { }
};

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
	virtual void OnImmediateStore(const SlotStorage& slot, const IRGenerate::ImmediateValue* value) = 0;
	virtual void OnGlobalLoad(const SlotStorage& slot, const IRGenerate::GlobalValue* value) = 0;
	virtual void OnMove(const SlotStorage& from, const SlotStorage& to) = 0;
	virtual void OnFreeReg(const RegSlot& reg) = 0;
	virtual void OnStackRelease(MemSize offset) = 0;
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
	void FreeReg(const String& group);
	// automatically 'takes' the register if any was found
	std::optional<RegSlot> GetFreeRegForType(const IRGenerate::Type* typ);
	std::optional<RegSlot> GetSpecificFreeRegForType(const RegTarget& target, const IRGenerate::Type* typ);
	bool RegSupportingType(const Reg& regSlot, const IRGenerate::Type* typ, const std::optional<RegTarget>& target = std::nullopt);
	std::optional<Reg> RegSupportingType(const RegGroup& group, const IRGenerate::Type* typ, const std::optional<RegTarget>& target = std::nullopt);
private:	
	void TakeFreeRegGroup(const RegGroup& group);
private:
	// Array<Reg> m_regs;
	std::map<String, RegGroup> m_regGroups;
	Array<String> m_freeRegs;
};

class StackDistributor
{
	using StackList = std::list<StackSlot>;
public:
	StackDistributor(AllocatorHandler* handler)
		: m_handler(handler) { }
public:
	inline MemSize GetStackOffset() const noexcept { return m_nextOffset; }

	void ResetState();
public:
	StackSlot GetStackSlotForSize(MemSize size);
	StackSlot AllocateStackSlotForSize(MemSize size);
	StackSlot GetStackSlotForValue(const IRGenerate::Value* value);

	std::optional<StackSlot> Align(Uint32 bytes);
	void ReleaseStackSlot(const StackList::iterator& slot);
	
	StackList::iterator FindStackSlot(const StackSlot& slot);
private:
	MemSize m_nextOffset;
	AllocatorHandler* m_handler = nullptr;
	StackList m_stack;
};

class BasicSlotAllocator
{
public:
	BasicSlotAllocator(RegisterDistributor* regDistro, StackDistributor* stackDistro, AllocatorHandler* handler);
	virtual ~BasicSlotAllocator() = default;

public:
	void ResetState();

public:
	const RegSlot LoadValueInReg(const IRGenerate::Value* value, std::optional<RegTarget> targetReg = std::nullopt);
	const Slot& StoreOrUpdateValue(const IRGenerate::Value* value, StoreDestination dest = AnyDestination{});
	void FreeValueWithReg(const RegSlot& reg);
	void SpillRegGroup(const String& groupName);

	std::optional<StackSlot> AlignStack(Uint32 bytes);
	StackSlot AllocateTempStackSlot(MemSize size);
	void ReleaseTempStackSlot(const StackSlot& tempSlot);

private:
	RegSlot GetAnyReg(const IRGenerate::Value* targetValue);
	RegSlot GetSpecificRegGroup(const RegTarget& target, const IRGenerate::Value* targetValue);

private:
	String GetSlotKey(const IRGenerate::Value* value);
	bool ValueIsStored(const IRGenerate::Value* value);
	Slot& GetSlotByKey(String slotKey);
	void InsertValueSlot(const IRGenerate::Value* value, SlotStorage slotStorage, bool rewrite = false);
	void DeleteValueSlot(const String& slotKey);

private:
	RegisterDistributor* m_regDistro = nullptr;
	StackDistributor* m_stackDistro = nullptr;
	AllocatorHandler* m_handler = nullptr;

	std::map<String, Slot> m_slots;
};

} // namespace Codegen

