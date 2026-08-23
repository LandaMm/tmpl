#include "cpl-codegen/register_allocator.h"

#include <format>
#include <algorithm>

namespace Codegen
{

BasicSlotAllocator::BasicSlotAllocator(std::initializer_list<Reg> availableRegisters, AllocatorHandler handler)
	: m_regs(availableRegisters), m_handler(handler)
{
	for (Uint8 i = 0; i < m_regs.Size(); ++i) m_freeRegs.Push(i);
}

const Reg BasicSlotAllocator::LoadValueInReg(const IRGenerate::Value* value, const std::optional<String>& regName)
{
	using namespace IRGenerate;

	Reg targetRegister = regName.has_value() ? GetSpecificReg(regName.value(), value) : GetAnyReg(value);

	RegDestination destination = RegDestination{ targetRegister };

	switch (value->Kind())
	{
	case ValueKind::GLOBAL:
	{
		assert(false && "NOT IMPLEMENTED");
	}
	break;
	// TODO: ValueKind::LOCAL
	case ValueKind::IMMEDIATE:
	case ValueKind::TEMPORAL:
	{
		if (!ValueIsStored(value)) StoreOrUpdateValue(value, destination);
		assert(ValueIsStored(value));
		Slot& slot = GetSlotByKey(GetSlotKey(value));
		if (std::holds_alternative<StackSlot>(slot.storage))
		{
			StoreOrUpdateValue(value, destination);
		}
		else if (std::get<Reg>(slot.storage).name != destination.reg.name)
		{
			StoreOrUpdateValue(value, destination);
		}

		return std::get<Reg>(GetSlotByKey(GetSlotKey(value)).storage);
	}
	break;
	case ValueKind::UNKNOWN:
	default:
		assert(false && "Not supported/handled value for loading into the register");
	}

	assert(false && "SHOULD BE UNREACHABLE");
	return {};
}

String BasicSlotAllocator::GetSlotKey(const IRGenerate::Value* value)
{
	using namespace IRGenerate;

	switch (value->Kind())
	{
	case ValueKind::TEMPORAL:
		return std::format("%{}", value->As<TemporalValue>()->Id());
	case ValueKind::IMMEDIATE:
		return std::format("imm{}", value->As<ImmediateValue>()->ImmValue());
	case ValueKind::UNKNOWN:
	default:
		assert(false && "unsupported value kind for slot key");
		return {};
	}
}

Reg BasicSlotAllocator::SpillReg(const String& regName)
{
	for (auto& [key, slot] : m_slots)
	{
		if (std::holds_alternative<Reg>(slot.storage) && std::get<Reg>(slot.storage).name == regName)
		{
			Reg moveReg = std::get<Reg>(slot.storage);

			StackSlot newSlot = GetNewStackSlotFromSize(moveReg.size);
			m_handler.SpillHandler().operator()(moveReg, newSlot);

			InsertValueSlot(slot.value, newSlot, true);

			// add register to freeRegs
			for (Uint8 i = 0; i < m_regs.Size(); ++i)
			{
				if (m_regs[i].name == moveReg.name)
				{
					m_freeRegs.Push(i);
				}
			}

			return moveReg;
		}
	}

	assert(false && "SHOULD BE UNREACHABLE");
	return {};
}

void BasicSlotAllocator::InsertValueSlot(const IRGenerate::Value* value, std::variant<Reg, StackSlot> slotStorage, bool rewrite)
{
	using namespace IRGenerate;
	if (!ValueIsStored(value))
	{
		switch (value->Kind())
		{
		case ValueKind::IMMEDIATE:
			m_handler.ImmediateStoreHandler().operator()(slotStorage, value->As<ImmediateValue>());
			break;
		}
	}
	if (!rewrite)
	{
		m_slots.insert({ GetSlotKey(value), Slot{slotStorage, value} });
	}
	else m_slots.insert_or_assign(GetSlotKey(value), Slot{ slotStorage, value });
}

const Slot& BasicSlotAllocator::StoreOrUpdateValue(const IRGenerate::Value* value, StoreDestination dest)
{
	std::visit([this, value](const auto& dest) {
		using T = std::decay_t<decltype(dest)>;

		if constexpr (std::is_same_v<T, AnyDestination>)
		{
			if (ValueIsStored(value)) return; // GetSlotByKey(GetSlotKey(value))
			if (!m_freeRegs.Empty())
			{
				InsertValueSlot(value, m_regs[m_freeRegs[0]]);
				m_freeRegs.Shift();
			}
			else
			{
				InsertValueSlot(value, GetNewStackSlotFromValue(value));
			}
		}
		else if constexpr (std::is_same_v<T, RegDestination>)
		{
			// Temp already exists, so moving if required
			if (ValueIsStored(value))
			{
				auto& slot = GetSlotByKey(GetSlotKey(value));
				if (std::holds_alternative<Reg>(slot.storage))
				{
					if (std::get<Reg>(slot.storage).name == dest.reg.name)
					{
						return;
					}

					// the value IS in the register but not in the right one

					// move from register to the register
					m_handler.MoveHandler().operator()(slot.storage, dest.reg);
					FreeReg(std::get<Reg>(slot.storage));
					InsertValueSlot(value, dest.reg);
				}
				else
				{
					// 'unspill' (moving from stack into the register
					m_handler.MoveHandler().operator()(slot.storage, dest.reg);
					// here temp is already stored but as stack
					InsertValueSlot(value, dest.reg, true);
				}
			}
			else
			{
				InsertValueSlot(value, dest.reg);
			}
		}
		else if constexpr (std::is_same_v<T, StackDestination>)
		{
			if (ValueIsStored(value))
			{
				auto& slot = GetSlotByKey(GetSlotKey(value));
				if (std::holds_alternative<StackSlot>(slot.storage))
				{
					return;
				}
				// here temp is already stored but as register
				// so we need to free the register first
				SpillReg(std::get<Reg>(slot.storage).name);
			}
			else
			{
				InsertValueSlot(value, GetNewStackSlotFromValue(value));
			}
		}
		else
		{
			assert(false && "UNSUPPORTED DESTINATION TARGET");
		}
	}, dest);

	return GetSlotByKey(GetSlotKey(value));
}

void BasicSlotAllocator::FreeReg(const Reg& reg)
{
	for (auto& [key, slot] : m_slots)
	{
		if (std::holds_alternative<Reg>(slot.storage) && std::get<Reg>(slot.storage).name == reg.name)
		{
			DeleteValueSlot(key);
			break;
		}
	}
}

void BasicSlotAllocator::DeleteValueSlot(const String& slotKey)
{
	if (m_slots.find(slotKey) == m_slots.end()) return;
	auto slot = m_slots[slotKey];

	if (std::holds_alternative<Reg>(slot.storage))
	{
		m_handler.FreeRegHandler().operator()(std::get<Reg>(slot.storage));
		for (size_t i = 0; i < m_regs.Size(); ++i)
		{
			if (m_regs[i].name == std::get<Reg>(slot.storage).name)
			{
				m_freeRegs.Push(i);
			}
		}
	}

	m_slots.erase(slotKey);
}

Reg BasicSlotAllocator::GetAnyReg(const IRGenerate::Value* targetValue)
{
	using namespace IRGenerate;

	auto sizedType = TypeResolver::ResolveTypeSize(targetValue->Typ());

	if (!m_freeRegs.Empty())
	{
		// TODO: Allocation Strategy
		for (size_t i = 0; i < m_freeRegs.Size(); ++i)
		{
			auto reg = m_regs[m_freeRegs[i]];
			if (reg.size == sizedType.size) {
				Array<Uint8> newFreeRegs;
				for (Uint8 j : m_freeRegs)
				{
					if (j != m_freeRegs[i]) newFreeRegs.Push(j);
				}
				m_freeRegs = newFreeRegs;
				return reg;
			}
		}
	}
	// Search for slots with register taken
	for (auto& [key, slot] : m_slots)
	{
		if (std::holds_alternative<Reg>(slot.storage))
		{
			return SpillReg(std::get<Reg>(slot.storage).name);
		}
	}

	assert(false && "SHOULD BE UNREACHABLE");
	return {};
}

Reg BasicSlotAllocator::GetSpecificReg(const String& regName, const IRGenerate::Value* targetValue)
{
	if (ValueIsStored(targetValue))
	{
		auto &slot = GetSlotByKey(GetSlotKey(targetValue));
		if (std::holds_alternative<Reg>(slot.storage) && std::get<Reg>(slot.storage).name == regName) return std::get<Reg>(slot.storage);
	}

	for (Uint8 index : m_freeRegs)
	{
		if (m_regs[index].name == regName)
		{
			return m_regs[index];
		}
	}

	// Register is occupied, search for value holding it and spill
	for (auto& [key, slot] : m_slots)
	{
		if (std::holds_alternative<Reg>(slot.storage) && std::get<Reg>(slot.storage).name == regName)
		{
			return SpillReg(regName);
		}
	}

	assert(false && "SHOULD BE UNREACHABLE");
	return {};
}

bool BasicSlotAllocator::ValueIsStored(const IRGenerate::Value* value)
{
	return m_slots.find(GetSlotKey(value)) != m_slots.end();
}

Slot& BasicSlotAllocator::GetSlotByKey(String slotKey)
{
	return m_slots.at(slotKey);
}

const StackSlot BasicSlotAllocator::GetLocal(const IRGenerate::LocalValue* local)
{
	if (m_locals.find(local->Name()) == m_locals.end())
	{
		return StoreLocal(local);
	}
	else
	{
		return m_locals.at(local->Name());
	}
}

const StackSlot& BasicSlotAllocator::StoreLocal(const IRGenerate::LocalValue* local)
{
	if (m_locals.find(local->Name()) != m_locals.end())
	{
		assert(false && "SHOULD BE UNREACHABLE TO REDECLARE LOCAL");
	}
	m_locals[local->Name()] = GetNewStackSlotFromValue(local);
	return m_locals.at(local->Name());
}

StackSlot BasicSlotAllocator::GetNewStackSlotFromValue(const IRGenerate::Value* value)
{
	IRGenerate::TypeLayout layout = IRGenerate::TypeResolver::ResolveTypeSize(value->Typ());
	return GetNewStackSlotFromSize(layout.size);
}

StackSlot BasicSlotAllocator::GetNewStackSlotFromSize(Uint32 size)
{
	m_nextOffset += size / 8;
	StackSlot slot = { m_nextOffset, size / 8 };
	m_handler.NewStackSlotHandler().operator()(slot);
	return slot;
}

} // namespace Codegen
