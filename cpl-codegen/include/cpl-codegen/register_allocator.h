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
	AllocatorHandler() = default;
	virtual ~AllocatorHandler() = default;
public:
	void OnNewStackSlot(std::function<void(const StackSlot&)> handler)
	{
		m_newStackSlotHandler = handler;
	}
	void OnSpill(std::function<void(const Reg&, const StackSlot&)> handler)
	{
		m_spillHandler = handler;
	}
	void OnImmediateStore(std::function<void(const std::variant<Reg, StackSlot>&, const IRGenerate::ImmediateValue*)> handler)
	{
		m_immediateStoreHandler = handler;
	}
private:
	friend class BasicSlotAllocator;
	[[nodiscard]] inline const std::function<void(const StackSlot&)>& NewStackSlotHandler() const noexcept { return m_newStackSlotHandler; }
	[[nodiscard]] inline const std::function<void(const Reg&, const StackSlot&)>& SpillHandler() const noexcept { return m_spillHandler; }
	[[nodiscard]] inline const std::function<void(const std::variant<Reg, StackSlot>&, const IRGenerate::ImmediateValue*)>& ImmediateStoreHandler() const noexcept { return m_immediateStoreHandler; }
private:
	std::function<void(const StackSlot&)> m_newStackSlotHandler;
	std::function<void(const Reg&, const StackSlot&)> m_spillHandler;
	std::function<void(const std::variant<Reg, StackSlot>&, const IRGenerate::ImmediateValue*)> m_immediateStoreHandler;
};

class BasicSlotAllocator
{
public:
	BasicSlotAllocator(std::initializer_list<Reg> availableRegisters, AllocatorHandler handler);
	virtual ~BasicSlotAllocator() = default;

public: // LocalValue
	const StackSlot& StoreLocal(const IRGenerate::LocalValue* local);
	const StackSlot GetLocal(const IRGenerate::LocalValue* local);

public:
	const Reg LoadValueInReg(const IRGenerate::Value* value, const std::optional<String>& regName = std::nullopt);
	const Slot& StoreOrUpdateValue(const IRGenerate::Value* value, StoreDestination dest = AnyDestination{});

private:
	StackSlot GetNewStackSlotFromValue(const IRGenerate::Value* value);
	StackSlot GetNewStackSlotFromSize(Uint32 size);
	Reg GetAnyReg(std::optional<const IRGenerate::Value*> targetValue = std::nullopt);
	Reg GetSpecificReg(const String& regName, std::optional<const IRGenerate::Value*> targetValue = std::nullopt);
	Reg SpillReg(const String& regName);

private:
	String GetSlotKey(const IRGenerate::Value* value);
	bool ValueIsStored(const IRGenerate::Value* value);
	Slot& GetSlotByKey(String slotKey);
	void InsertValueSlot(const IRGenerate::Value* value, std::variant<Reg, StackSlot> slotStorage, bool rewrite = false);

private:
	AllocatorHandler m_handler;
	Array<Reg> m_regs;
	Array<Uint8> m_freeRegs;

	std::map<String, Slot> m_slots;

	std::map<String, StackSlot> m_locals;
	Uint32 m_nextOffset = 0;
};

} // namespace Codegen

