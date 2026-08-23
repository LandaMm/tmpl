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
	using NewStackSlotFn = std::function<void(const StackSlot&)>;
	using SpillFn = std::function<void(const Reg&, const StackSlot&)>;
	using MoveFn = std::function<void(const std::variant<Reg, StackSlot>&, const std::variant<Reg, StackSlot>&)>;
	using ImmediateStoreFn = std::function<void(const std::variant<Reg, StackSlot>&, const IRGenerate::ImmediateValue*)>;
	using FreeRegFn = std::function<void(const Reg&)>;
public:
	AllocatorHandler() = default;
	virtual ~AllocatorHandler() = default;
public:
	void OnNewStackSlot(NewStackSlotFn handler)
	{
		m_newStackSlotHandler = handler;
	}
	void OnSpill(SpillFn handler)
	{
		m_spillHandler = handler;
	}
	void OnImmediateStore(ImmediateStoreFn handler)
	{
		m_immediateStoreHandler = handler;
	}
	void OnMove(MoveFn handler)
	{
		m_moveRegHandler = handler;
	}
	void OnFreeReg(FreeRegFn handler)
	{
		m_freeRegHandler = handler;
	}
private:
	friend class BasicSlotAllocator;
	[[nodiscard]] inline const NewStackSlotFn& NewStackSlotHandler() const noexcept { return m_newStackSlotHandler; }
	[[nodiscard]] inline const SpillFn& SpillHandler() const noexcept { return m_spillHandler; }
	[[nodiscard]] inline const MoveFn& MoveHandler() const noexcept { return m_moveRegHandler; }
	[[nodiscard]] inline const ImmediateStoreFn& ImmediateStoreHandler() const noexcept { return m_immediateStoreHandler; }
	[[nodiscard]] inline const FreeRegFn& FreeRegHandler() const noexcept { return m_freeRegHandler; }

private:
	NewStackSlotFn m_newStackSlotHandler;
	SpillFn m_spillHandler;
	ImmediateStoreFn m_immediateStoreHandler;
	MoveFn m_moveRegHandler;
	FreeRegFn m_freeRegHandler;
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
	AllocatorHandler m_handler;
	Array<Reg> m_regs;
	Array<Uint8> m_freeRegs;

	std::map<String, Slot> m_slots;

	std::map<String, StackSlot> m_locals;
	Uint32 m_nextOffset = 0;
};

} // namespace Codegen

