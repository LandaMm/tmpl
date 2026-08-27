#include "cpl-codegen/register_allocator.h"

#include <format>
#include <algorithm>

namespace Codegen
{

RegisterDistributor::RegisterDistributor(std::initializer_list<Reg> availableRegisters)
	: m_regs(availableRegisters)
{
	for (Uint8 i = 0; i < m_regs.Size(); ++i) m_freeRegs.Push(i);
}

void RegisterDistributor::FreeReg(const Reg& reg)
{
	for (Uint8 i = 0; i < m_regs.Size(); ++i)
	{
		if (m_regs[i].name == reg.name)
		{
			m_freeRegs.Push(i);
		}
	}
}

void RegisterDistributor::TakeFreeReg(const Reg& reg)
{
	Array<Uint8> newFreeRegs;
	for (Uint8 regIndex : m_freeRegs)
	{
		if (m_regs[regIndex].name != reg.name) newFreeRegs.Push(regIndex);
	}
	m_freeRegs = newFreeRegs;
}

std::optional<Reg> RegisterDistributor::GetFreeRegForType(const IRGenerate::Type* typ)
{
	using namespace IRGenerate;

	// TODO: Allocation Strategy
	for (size_t i = 0; i < m_freeRegs.Size(); ++i)
	{
		auto reg = m_regs[m_freeRegs[i]];
		if (RegSupportsType(reg, typ)) {
			TakeFreeReg(reg);
			return reg;
		}
	}

	return std::nullopt;
}

bool RegisterDistributor::RegSupportsType(const Reg& reg, const IRGenerate::Type* typ)
{
	auto typeSize = IRGenerate::TypeResolver::ResolveTypeSize(typ);
	return reg.size == typeSize.size;
}

BasicSlotAllocator::BasicSlotAllocator(RegisterDistributor* regDistro, AllocatorHandler* handler)
	: m_regDistro(regDistro), m_handler(handler)
{
}

const Reg BasicSlotAllocator::LoadValueInReg(const IRGenerate::Value* value)
{
	using namespace IRGenerate;

	Reg targetRegister = GetAnyReg(value);

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
			m_handler->OnSpill(moveReg, newSlot);

			InsertValueSlot(slot.value, newSlot, true);

			// add register to freeRegs
			m_regDistro->FreeReg(moveReg);

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
			m_handler->OnImmediateStore(slotStorage, value->As<ImmediateValue>());
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
			auto freeReg = m_regDistro->GetFreeRegForType(value->Typ());
			if (freeReg)
			{
				InsertValueSlot(value, freeReg.value());
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
					m_handler->OnMove(slot.storage, dest.reg);
					FreeReg(std::get<Reg>(slot.storage));
					InsertValueSlot(value, dest.reg);
				}
				else
				{
					// 'unspill' (moving from stack into the register
					m_handler->OnMove(slot.storage, dest.reg);
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
		m_handler->OnFreeReg(std::get<Reg>(slot.storage));
		m_regDistro->FreeReg(std::get<Reg>(slot.storage));
	}

	m_slots.erase(slotKey);
}

Reg BasicSlotAllocator::GetAnyReg(const IRGenerate::Value* targetValue)
{
	using namespace IRGenerate;

	auto freeReg = m_regDistro->GetFreeRegForType(targetValue->Typ());
	if (freeReg)
	{
		return freeReg.value();
	}

	// Search for slots with register taken
	for (auto& [key, slot] : m_slots)
	{
		if (std::holds_alternative<Reg>(slot.storage) && m_regDistro->RegSupportsType(std::get<Reg>(slot.storage), targetValue->Typ()))
		{
			return SpillReg(std::get<Reg>(slot.storage).name);
		}
	}

	assert(false && "SHOULD BE UNREACHABLE");
	return {};
}

/*
Reg BasicSlotAllocator::GetSpecificReg(const String& regName, const IRGenerate::Value* targetValue)
{
	if (ValueIsStored(targetValue))
	{
		auto &slot = GetSlotByKey(GetSlotKey(targetValue));
		if (std::holds_alternative<Reg>(slot.storage) && std::get<Reg>(slot.storage).name == regName) return std::get<Reg>(slot.storage);
	}

	auto reg = m_regDistro->GetFreeRegForType(targe;
	if (reg.has_value())
	{
		m_regDistro->TakeFreeReg(reg.value());
		return reg.value();
	}

	// Register is occupied, search for value holding it and spill
	return SpillReg(regName);
}
*/

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
	m_handler->OnNewStackSlot(slot);
	return slot;
}

} // namespace Codegen
