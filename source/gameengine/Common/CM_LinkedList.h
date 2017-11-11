#ifndef __CM_LINKED_LIST_H__
#define __CM_LINKED_LIST_H__

#include "BLI_utildefines.h"

template <class Item>
class CM_LinkedList
{
public:
	class Link
	{
	friend CM_LinkedList;

	private:
		// Stored item pointer.
		Item *m_item;
		// The list link before this one.
		Link *m_prev;
		// The list link after this one.
		Link *m_next;
		/// The first link of the list used to add a link.
		Link *m_begin;

	public:
		Link(Item *item, Link *begin)
			:m_item(item),
			m_prev(begin),
			m_next(begin),
			m_begin(begin)
		{
		}

		Link()
			:Link(nullptr, nullptr)
		{
		}

		~Link()
		{
			// If the list died before the iterators are left alone with m_begin to nullptr.
			if (m_begin) {
				Remove();
			}
		}

		void Add()
		{
			if (!IsRemoved()) {
				return;
			}

			BLI_assert(m_prev && m_next);
			m_next = m_prev->m_next;
			m_next->m_prev = this;
			m_prev->m_next = this;
		}

		void Remove()
		{
			if (IsRemoved()) {
				return;
			}

			BLI_assert(m_prev && m_next);
			m_next->m_prev = m_prev;
			m_prev->m_next = m_next;
			m_next = m_prev = m_begin;
		}

		bool IsRemoved() const
		{
			return (m_prev == m_next);
		}
	};

	class Iterator
	{
	private:
		Link *m_current;

	public:
		Iterator(Link *link)
			:m_current(link)
		{
			BLI_assert(m_current);
		}

		Iterator& operator++()
		{
			BLI_assert(m_current);
			m_current = m_current->m_next;

			return *this;
		}

		Item *operator*() const
		{
			BLI_assert(m_current);
			return m_current->m_item;
		}

		bool operator!=(const Iterator& other) const
		{
			return m_current != other.m_current;
		}
	};

private:
	Link m_begin;
	Link m_end;

public:
	CM_LinkedList()
	{
		m_begin.m_next = &m_end;
		m_end.m_prev = &m_begin;
	}

	~CM_LinkedList()
	{
		Link *link = &m_begin;
		while (link) {
			Link *next = link->m_next;
			link->m_next = link->m_prev = link->m_begin = nullptr;
			link = next;
		}
	}

	CM_LinkedList(const CM_LinkedList& other) = delete;
	CM_LinkedList(CM_LinkedList&& other) = delete;

	Link *NewLink(Item *item)
	{
		Link *link = new Link(item, &m_begin);
		return link;
	}

	bool IsEmpty() const
	{
		return (m_begin.m_next = &m_end);
	}

	Iterator begin()
	{
		return Iterator(m_begin.m_next);
	}
	Iterator end()
	{
		return Iterator(&m_end);
	}
};

#endif  // __CM_LINKED_LIST_H__
