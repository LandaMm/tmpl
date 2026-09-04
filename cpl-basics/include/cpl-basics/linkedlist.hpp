#pragma once

#include "allocator/arena.hpp"

namespace Basics
{

template<typename T>
class DoubleLinkedList;

template<typename T>
struct ListIterator;

template<typename T>
class Node
{
public:
	Node(const T& aData)
		: data(aData), prev(nullptr), next(nullptr) { }
public:
	T& Data() const noexcept { return data; }
private:
	T data;
	Node* prev = nullptr;
	Node* next = nullptr;

	friend class DoubleLinkedList<T>;
	friend struct ListIterator<T>;
};

template<typename T>
struct ListIterator
{
public:
	explicit ListIterator(Node<T>* start)
		: current(start) { }
public:
	Node<T>* operator*()
	{
		return current;
	}

	Node<T>* operator->()
	{
		return current;
	}

	ListIterator& operator++()
	{
		current = current->next;
		return *this;
	}

	inline Node<T>* Current() const noexcept { return current; }

	inline T& Data() const noexcept { return current->data; }

	inline bool operator==(const ListIterator& other) const noexcept { return current == other.current; }
	inline bool operator!=(const ListIterator& other) const noexcept { return current != other.current; }
private:
	Node<T>* current = nullptr;

	friend class DoubleLinkedList<T>;
};

template<typename T>
class DoubleLinkedList
{
public:
	DoubleLinkedList(T initialData)
	{
		Append(initialData);
	}
	DoubleLinkedList() = default;
public:
	ListIterator<T> Append(T item)
	{
		if (!m_begin)
		{
			m_begin = CreateNewNode(item);
			m_last = m_begin;
			return ListIterator<T>(m_begin);
		}
		else
		{
			assert(m_last);
			Node<T>* node = CreateNewNode(item);
			m_last->next = node;
			node->prev = m_last;
			m_last = node;
			return ListIterator<T>(node);
		}
	}

	ListIterator<T> InsertAfter(T item, const ListIterator<T>& after)
	{
		Node<T>* node = after.Current();
		assert(node);

		Node<T>* newNode = CreateNewNode(item);
		Node<T>* originalNext = node->next;
		node->next = newNode;
		newNode->prev = node;
		newNode->next = originalNext;
		return ListIterator<T>(newNode);
		
		// TODO: better error maybe exception throwing
		assert(false && "the target node does not exist in the list");
		return ListIterator<T>(nullptr);
	}

	void Remove(const ListIterator<T>& it)
	{
		Node<T>* node = it.Current();
		assert(node);

		node->prev->next = node->next;
		node->next->prev = node->prev;
	}
public:
	inline ListIterator<T> begin() const noexcept { return ListIterator<T>(m_begin); }
	inline ListIterator<T> end() const noexcept { return ListIterator<T>(nullptr); }
private:
	Node<T>* CreateNewNode(const T& data) { return m_arena.Alloc<Node<T>>(data); }
private:
	Node<T>* m_begin = nullptr;
	Node<T>* m_last = nullptr;
	ArenaAllocator<> m_arena;
};

}

