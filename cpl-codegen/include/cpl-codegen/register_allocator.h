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

struct StackSlot
{
	Uint32 offset;
	Uint32 size;
};

struct Slot
{
	std::variant<Reg, StackSlot> storage;
	const IRGenerate::Value* value;
};

// Store Destinations
struct AnyDestination {};

struct RegDestination
{
	Reg reg;
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
	virtual void OnNewStackSlot(const StackSlot&) = 0;
	virtual void OnSpill(const Reg&, const StackSlot&) = 0;
	virtual void OnImmediateStore(const std::variant<Reg, StackSlot>&, const IRGenerate::ImmediateValue*) = 0;
	virtual void OnMove(const std::variant<Reg, StackSlot>&, const std::variant<Reg, StackSlot>&) = 0;
	virtual void OnFreeReg(const Reg&) = 0;
};

class RegisterDistributor
{
public:
	RegisterDistributor(std::initializer_list<Reg> availableRegisters);
public:
	void FreeReg(const Reg& reg);
	// automatically 'takes' the register if any was found
	std::optional<Reg> GetFreeRegForType(const IRGenerate::Type* typ);
	bool RegSupportsType(const Reg& reg, const IRGenerate::Type* typ);
private:	
	void TakeFreeReg(const Reg& reg);
private:
	Array<Reg> m_regs;
	Array<Uint8> m_freeRegs;
};

class BasicSlotAllocator
{
public:
	BasicSlotAllocator(RegisterDistributor* regDistro, AllocatorHandler* handler);
	virtual ~BasicSlotAllocator() = default;

public: // LocalValue
	const StackSlot& StoreLocal(const IRGenerate::LocalValue* local);
	const StackSlot GetLocal(const IRGenerate::LocalValue* local);

public:
	const Reg LoadValueInReg(const IRGenerate::Value* value);
	const Slot& StoreOrUpdateValue(const IRGenerate::Value* value, StoreDestination dest = AnyDestination{});
	void FreeReg(const Reg& reg);

private:
	StackSlot GetNewStackSlotFromValue(const IRGenerate::Value* value);
	StackSlot GetNewStackSlotFromSize(Uint32 size);
	Reg GetAnyReg(const IRGenerate::Value* targetValue);
	Reg GetSpecificReg(const String& regName, const IRGenerate::Value* targetValue);
	Reg SpillReg(const String& regName);

private:
	String GetSlotKey(const IRGenerate::Value* value);
	bool ValueIsStored(const IRGenerate::Value* value);
	Slot& GetSlotByKey(String slotKey);
	void InsertValueSlot(const IRGenerate::Value* value, std::variant<Reg, StackSlot> slotStorage, bool rewrite = false);
	void DeleteValueSlot(const String& slotKey);

private:
	RegisterDistributor* m_regDistro = nullptr;
	AllocatorHandler* m_handler = nullptr;

	std::map<String, Slot> m_slots;

	std::map<String, StackSlot> m_locals;
	Uint32 m_nextOffset = 0;
};

} // namespace Codegen

