#include "cpl-codegen/register_allocator.h"

#include <format>
#include <algorithm>

namespace Codegen
{

RegisterDistributor::RegisterDistributor(std::initializer_list<RegGroup> availableRegisters)
{
	for (RegGroup group : availableRegisters)
	{
		m_regGroups.insert({ group.name, group });
	}
	for (auto& [groupName, _] : m_regGroups) m_freeRegs.Push(groupName);
}

void RegisterDistributor::ResetState()
{
	m_freeRegs.Clear();
	for (auto& [groupName, _] : m_regGroups) m_freeRegs.Push(groupName);
}

void RegisterDistributor::FreeReg(const RegSlot& reg)
{
	m_freeRegs.Push(reg.groupName);
}

void RegisterDistributor::FreeReg(const String& group)
{
	if (m_regGroups.find(group) == m_regGroups.end()) return;
	m_freeRegs.Push(group);
}

const RegGroup& RegisterDistributor::GetGroupByName(const String& groupName)
{
	return m_regGroups.at(groupName);
}

void RegisterDistributor::TakeFreeRegGroup(const RegGroup& group)
{
	Array<String> newFreeRegs;
	for (String groupName : m_freeRegs)
	{
		if (groupName != group.name) newFreeRegs.Push(groupName);
	}
	m_freeRegs = newFreeRegs;
}

std::optional<RegSlot> RegisterDistributor::GetSpecificFreeRegForType(const RegTarget& target, const IRGenerate::Type* typ)
{
	using namespace IRGenerate;

	// TODO: Allocation Strategy
	for (String& freeGroupName : m_freeRegs)
	{
		if (freeGroupName != target.group) continue;
		RegGroup& regGroup = m_regGroups[freeGroupName];
		auto supportedReg = RegSupportingType(regGroup, typ, target);
		if (supportedReg) {
			TakeFreeRegGroup(regGroup);
			return RegSlot(supportedReg.value(), regGroup.name);
		}
	}

	return std::nullopt;
}

std::optional<RegSlot> RegisterDistributor::GetFreeRegForType(const IRGenerate::Type* typ)
{
	using namespace IRGenerate;

	// TODO: Allocation Strategy
	for (String& freeGroupName : m_freeRegs)
	{
		RegGroup& regGroup = m_regGroups[freeGroupName];
		auto supportedReg = RegSupportingType(regGroup, typ);
		if (supportedReg) {
			TakeFreeRegGroup(regGroup);
			return RegSlot(supportedReg.value(), regGroup.name);
		}
	}

	return std::nullopt;
}

std::optional<Reg> RegisterDistributor::RegSupportingType(const RegGroup& group, const IRGenerate::Type* typ, const std::optional<RegTarget>& target)
{
	for (const Reg& reg : group.regs)
	{
		if (RegSupportingType(reg, typ, target)) return reg;
	}
	return std::nullopt;
}

bool RegisterDistributor::RegSupportingType(const Reg& regSlot, const IRGenerate::Type* typ, const std::optional<RegTarget>& target)
{
	auto typeSize = IRGenerate::TypeResolver::ResolveTypeSize(typ);
	if (target.has_value() && target.value().minSize > regSlot.size) return false;
	return regSlot.size >= typeSize.size;
}

void StackDistributor::ResetState()
{
	m_nextOffset = {};
}

std::optional<StackSlot> StackDistributor::Align(Uint32 bytes)
{
	// byte alignment
	MemSize currentOffset = GetStackOffset();

	// Allocate remaining bytes
	MemSize alignment = GetStackOffset() % MemSize::FromBytes(bytes);
	if (alignment != MemSize::FromBits(0))
		return GetNewStackSlotFromSize(alignment);

	return std::nullopt;
}

void StackDistributor::ReleaseStackSlot(const StackList::iterator& it)
{
	assert(it != m_stack.end());
	if (std::next(it) == m_stack.end())
	{
		m_nextOffset -= it->size;
		m_handler->OnStackRelease(it->size);
		m_stack.erase(it);
	}
	else
	{
		it->taken = false;
	}
}

std::list<StackSlot>::iterator StackDistributor::FindStackSlot(const StackSlot& slot)
{
	for (auto it = m_stack.begin(); it != m_stack.end(); ++it)
	{
		if (it->offset == slot.offset) return it;
	}

	return m_stack.end();
}

StackSlot StackDistributor::GetNewStackSlotFromValue(const IRGenerate::Value* value)
{
	IRGenerate::TypeLayout layout = IRGenerate::TypeResolver::ResolveTypeSize(value->Typ());
	return GetNewStackSlotFromSize(layout.size);
}

StackSlot StackDistributor::GetNewStackSlotFromSize(MemSize size)
{
	assert(size != MemSize::FromBits(0));
	for (auto it = m_stack.begin(); it != m_stack.end(); ++it)
	{
		if (!it->taken && it->size == size)
		{
			it->taken = true;
			return *it;
		}
	}

	m_nextOffset += size;
	StackSlot slot(m_nextOffset, size, true);
	m_handler->OnNewStackSlot(slot);
	m_stack.push_back(slot);
	return slot;
}

BasicSlotAllocator::BasicSlotAllocator(RegisterDistributor* regDistro, StackDistributor* stackDistro, AllocatorHandler* handler)
	: m_regDistro(regDistro), m_stackDistro(stackDistro), m_handler(handler)
{
}

void BasicSlotAllocator::ResetState()
{
	m_regDistro->ResetState();
	m_stackDistro->ResetState();

	m_slots.clear();
}

const RegSlot BasicSlotAllocator::LoadValueInReg(const IRGenerate::Value* value, std::optional<RegTarget> targetReg)
{
	using namespace IRGenerate;

	RegSlot targetRegister = targetReg ? GetSpecificRegGroup(targetReg.value(), value) : GetAnyReg(value);

	RegDestination destination = RegDestination{ targetRegister };

	switch (value->Kind())
	{
	case ValueKind::GLOBAL:
	{
		assert(false && "NOT IMPLEMENTED");
	}
	break;
	case ValueKind::IMMEDIATE:
	case ValueKind::TEMPORAL:
	case ValueKind::LOCAL:
	{
		if (!ValueIsStored(value)) StoreOrUpdateValue(value, destination);
		assert(ValueIsStored(value));
		Slot& slot = GetSlotByKey(GetSlotKey(value));
		if (std::holds_alternative<StackSlot>(slot.storage))
		{
			StoreOrUpdateValue(value, destination);
		}
		else if (std::get<RegSlot>(slot.storage).name != destination.reg.name || std::get<RegSlot>(slot.storage).groupName != targetRegister.groupName)
		{
			StoreOrUpdateValue(value, destination);
		}

		return std::get<RegSlot>(GetSlotByKey(GetSlotKey(value)).storage);
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
	case ValueKind::LOCAL:
		return std::format("loc'{}'", value->As<LocalValue>()->Name().c_str());
	case ValueKind::GLOBAL:
		return std::format("global'{}'", value->As<GlobalValue>()->Name().c_str());
	case ValueKind::UNKNOWN:
	default:
		assert(false && "unsupported value kind for slot key");
		return {};
	}
}

std::optional<StackSlot> BasicSlotAllocator::AlignStack(Uint32 bytes)
{
	return m_stackDistro->Align(bytes);
}

StackSlot BasicSlotAllocator::AllocateTempStackSlot(MemSize size)
{
	return m_stackDistro->GetNewStackSlotFromSize(size);
}

void BasicSlotAllocator::ReleaseTempStackSlot(const StackSlot& tempSlot)
{
	m_stackDistro->ReleaseStackSlot(m_stackDistro->FindStackSlot(tempSlot));
}

void BasicSlotAllocator::SpillRegGroup(const String& groupName)
{
	for (auto& [key, slot] : m_slots)
	{
		if (std::holds_alternative<RegSlot>(slot.storage) && std::get<RegSlot>(slot.storage).groupName == groupName)
		{
			RegSlot moveReg = std::get<RegSlot>(slot.storage);

			StackSlot newSlot = m_stackDistro->GetNewStackSlotFromSize(moveReg.size);
			m_handler->OnSpill(moveReg, newSlot);

			InsertValueSlot(slot.value, newSlot, true);

			// add register to freeRegs
			m_handler->OnFreeReg(moveReg);
			m_regDistro->FreeReg(moveReg);
		}
	}
}

void BasicSlotAllocator::InsertValueSlot(const IRGenerate::Value* value, std::variant<RegSlot, StackSlot> slotStorage, bool rewrite)
{
	using namespace IRGenerate;
	if (!ValueIsStored(value))
	{
		switch (value->Kind())
		{
		case ValueKind::IMMEDIATE:
			m_handler->OnImmediateStore(slotStorage, value->As<ImmediateValue>());
			break;
		case ValueKind::GLOBAL:
			m_handler->OnGlobalLoad(slotStorage, value->As<GlobalValue>());
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
				InsertValueSlot(value, m_stackDistro->GetNewStackSlotFromValue(value));
			}
		}
		else if constexpr (std::is_same_v<T, RegDestination>)
		{
			// Temp already exists, so moving if required
			if (ValueIsStored(value))
			{
				auto& slot = GetSlotByKey(GetSlotKey(value));
				if (std::holds_alternative<RegSlot>(slot.storage))
				{
					if (std::get<RegSlot>(slot.storage).name == dest.reg.name)
					{
						return;
					}

					// the value IS in the register but not in the right one

					// move from register to the register
					m_handler->OnMove(slot.storage, dest.reg);
					FreeValueWithReg(std::get<RegSlot>(slot.storage));
					InsertValueSlot(value, dest.reg);
				}
				else
				{
					// 'unspill' (moving from stack into the register)
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
				SpillRegGroup(std::get<RegSlot>(slot.storage).groupName);
			}
			else
			{
				InsertValueSlot(value, m_stackDistro->GetNewStackSlotFromValue(value));
			}
		}
		else
		{
			assert(false && "UNSUPPORTED DESTINATION TARGET");
		}
	}, dest);

	return GetSlotByKey(GetSlotKey(value));
}

void BasicSlotAllocator::FreeValueWithReg(const RegSlot& reg)
{
	for (auto& [key, slot] : m_slots)
	{
		if (std::holds_alternative<RegSlot>(slot.storage) && std::get<RegSlot>(slot.storage).name == reg.name)
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

	if (std::holds_alternative<RegSlot>(slot.storage))
	{
		m_handler->OnFreeReg(std::get<RegSlot>(slot.storage));
		m_regDistro->FreeReg(std::get<RegSlot>(slot.storage));
	}

	m_slots.erase(slotKey);
}

RegSlot BasicSlotAllocator::GetAnyReg(const IRGenerate::Value* targetValue)
{
	using namespace IRGenerate;

	if (ValueIsStored(targetValue))
	{
		const Slot& slot = GetSlotByKey(GetSlotKey(targetValue));
		if (std::holds_alternative<RegSlot>(slot.storage) && m_regDistro->RegSupportingType(std::get<RegSlot>(slot.storage), targetValue->Typ()))
		{
			return std::get<RegSlot>(slot.storage);
		}
	}

	auto freeReg = m_regDistro->GetFreeRegForType(targetValue->Typ());
	if (freeReg)
	{
		return freeReg.value();
	}

	// Search for slots with register taken
	for (auto& [key, slot] : m_slots)
	{
		if (std::holds_alternative<RegSlot>(slot.storage))
		{
			const String& groupName = std::get<RegSlot>(slot.storage).groupName;
			std::optional<Reg> supportedReg = m_regDistro->RegSupportingType(m_regDistro->GetGroupByName(groupName), targetValue->Typ());
			if (supportedReg.has_value())
			{
				SpillRegGroup(std::get<RegSlot>(slot.storage).groupName);
				return RegSlot(supportedReg.value(), groupName);
			}
		}
	}

	assert(false && "SHOULD BE UNREACHABLE");
	return {};
}

RegSlot BasicSlotAllocator::GetSpecificRegGroup(const RegTarget& target, const IRGenerate::Value* targetValue)
{
	if (ValueIsStored(targetValue))
	{
		auto &slot = GetSlotByKey(GetSlotKey(targetValue));
		if (std::holds_alternative<RegSlot>(slot.storage))
		{
			const auto& regSlot = std::get<RegSlot>(slot.storage);
			if (regSlot.groupName == target.group && regSlot.size >= target.minSize)
				return regSlot;
		}
	}

	std::optional<RegSlot> reg = m_regDistro->GetSpecificFreeRegForType(target, targetValue->Typ());
	if (reg.has_value())
	{
		return reg.value();
	}

	// Register is occupied, search for value holding it and spill
	SpillRegGroup(target.group);

	reg = m_regDistro->GetSpecificFreeRegForType(target, targetValue->Typ());
	assert(reg.has_value() && "SHOULD BE FREE AND AVAILABLE AFTER SPILLING");

	return reg.value();
}

bool BasicSlotAllocator::ValueIsStored(const IRGenerate::Value* value)
{
	return m_slots.find(GetSlotKey(value)) != m_slots.end();
}

Slot& BasicSlotAllocator::GetSlotByKey(String slotKey)
{
	return m_slots.at(slotKey);
}

} // namespace Codegen
